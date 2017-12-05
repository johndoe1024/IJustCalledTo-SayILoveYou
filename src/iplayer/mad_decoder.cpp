#include "iplayer/mad_decoder.h"

#include <assert.h>
#include <mad.h>
#include <pulse/error.h>
#include <pulse/simple.h>

#include "iplayer/utils/file_mapping.h"
#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

//
// Inspired from:
// https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html
//
// See also:
// https://github.com/njh/madjack/blob/master/src/minimad.c
// https://github.com/bbc/audiowaveform/blob/master/src/Mp3AudioFileReader.cpp
// http://read.pudn.com/downloads143/sourcecode/book/624943/libmad_05_0319/src/decoder.c__.htm?scrajs=wslfd1
//

// Some helper functions, to be cleaned up in the future
int scale(mad_fixed_t sample) {
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

namespace ip {

MadDecoder::MadDecoder(const TrackInfo& track_info, CompletionCb cb)
    : paused_(false),
      exit_decoder_thread_(false),
      played_time_(std::chrono::seconds(0)),
      device_(nullptr) {
  decoder_future_ = std::async(std::launch::async, &MadDecoder::DecoderThread,
                               this, track_info, cb);
}

MadDecoder::~MadDecoder() {
  exit_decoder_thread_ = true;
  Unpause();
}

void MadDecoder::Pause() { paused_ = true; }

void MadDecoder::Unpause() {
  {
    std::lock_guard<std::mutex> lock(pause_mutex_);
    paused_ = false;
    pause_cv_.notify_one();
  }
}

std::chrono::seconds MadDecoder::GetPlayedTime() const { return played_time_; }

int MadDecoder::Output(struct mad_header const*, struct mad_pcm* pcm) {
  int error = 0;

  int nsamples = pcm->length;
  mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];
  static char stream[1152 * 4];
  if (pcm->channels == 2) {
    while (nsamples--) {
      signed int sample;
      sample = scale(*left_ch++);
      stream[(pcm->length - nsamples) * 4] =
          static_cast<char>(((sample >> 0) & 0xff));
      stream[(pcm->length - nsamples) * 4 + 1] =
          static_cast<char>(((sample >> 8) & 0xff));
      sample = scale(*right_ch++);
      stream[(pcm->length - nsamples) * 4 + 2] =
          static_cast<char>(((sample >> 0) & 0xff));
      stream[(pcm->length - nsamples) * 4 + 3] =
          static_cast<char>(((sample >> 8) & 0xff));
    }
    if (pa_simple_write(device_, stream, static_cast<size_t>(1152 * 4),
                        &error) < 0) {
      LOG("pa_simple_write() failed: %s\n", pa_strerror(error));
      return error;
    }
  } else {
    LOG("Mono not supported!");
  }
  return 0;
}

void MadDecoder::DecoderThread(TrackInfo info, CompletionCb completion_cb) {
  std::error_code ec;
  try {
    ec = Decode(info.Location());
  } catch (const std::system_error& ex) {
    ec = ex.code();
  } catch (const std::exception& ex) {
    UNUSED(ex);
    LOG("caught exception: %s", ex.what());
    ec = std::make_error_code(std::errc::bad_message);
  }
  if (completion_cb) {
    completion_cb(ec);
  }
}

std::error_code MadDecoder::Decode(const TrackInfo& info) {
  // IDEA: should use ITrackIO instead of direct file access
  LOG("[D] decoding %s", info.Location().c_str());
  int error = EINTR;
  struct mad_stream mad_stream;
  struct mad_frame mad_frame;
  struct mad_synth mad_synth;

  mad_timer_t timer = mad_timer_zero;

  // set up PulseAudio 16-bit 44.1kHz stereo output
  static const pa_sample_spec ss = {PA_SAMPLE_S16LE, 44100, 2};

  // cleanup guard
  auto cleanup_guard = CreateScopeGuard([&]() {
    LOG("[D] end of decoding %s", info.Location().c_str());
    if (device_) {
      pa_simple_free(device_);
      device_ = nullptr;
    }
    mad_synth_finish(&mad_synth_);
    mad_frame_finish(&mad_frame);
    mad_stream_finish(&mad_stream);
  });

  mad_stream_init(&mad_stream);
  mad_frame_init(&mad_frame);
  mad_synth_init(&mad_synth);

  if (!(device_ = pa_simple_new(NULL, "MP3 player", PA_STREAM_PLAYBACK, NULL,
                                "playback", &ss, NULL, NULL, &error))) {
    LOG("pa_simple_new() failed");
    return {errno, std::generic_category()};
  }

  // IDEA: TrackLocation should be more than a typedef on std::string and
  // provide access to uri elements, until then...
  std::string separator("file://");
  auto separator_pos = info.Location().find(separator);
  if (separator_pos == std::string::npos) {
    return {};
  }
  auto path = info.Location().substr(separator.size());

  FileMapping file_mapping(path);  // MAD_BUFFER_GUARD can cause issue
  mad_stream_buffer(&mad_stream, file_mapping, file_mapping.size());
  while (true) {
    {
      std::unique_lock<std::mutex> lock(pause_mutex_);
      pause_cv_.wait(lock, [this]() { return paused_ == false; });
    }
    if (exit_decoder_thread_) {
      return std::make_error_code(std::errc::operation_canceled);
    }
    if (mad_frame_decode(&mad_frame, &mad_stream)) {
      if (MAD_RECOVERABLE(mad_stream.error)) {
        continue;
      } else if (mad_stream.error == MAD_ERROR_BUFLEN) {
        break;  // it means eof as file is completely mapped
      } else {
        // ideally mad's error should be mapped to custom error_code
        return std::make_error_code(std::errc::bad_message);
      }
    }
    mad_synth_frame(&mad_synth, &mad_frame);

    // update ellapsed time
    mad_timer_add(&timer, mad_frame.header.duration);
    played_time_ = std::chrono::seconds(timer.seconds);

    error = Output(&mad_frame.header, &mad_synth.pcm);
    if (error) {
      return std::make_error_code(std::errc::bad_message);
    }
  }
  return {};
}

}  // namespace ip
