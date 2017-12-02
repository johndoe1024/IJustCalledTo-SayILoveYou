#include "iplayer/mad_decoder.h"

#include <assert.h>
#include <pulse/error.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

// inspired from:
// https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html

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

MadDecoder::MadDecoder(std::unique_ptr<ITrackProvider> provider,
                       const TrackLocation &track, CompletionCb cb)
    : paused_(false),
      exit_decoder_thread_(false),
      played_time_(std::chrono::seconds(0)),
      device_(nullptr) {
  decoder_future_ = std::async(std::launch::async, &MadDecoder::DecoderThread,
                               this, std::move(provider), track, cb);
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

int MadDecoder::Output(struct mad_header const *header, struct mad_pcm *pcm) {
  int error = 0;
  register int nsamples = pcm->length;
  mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];
  static char stream[1152 * 4];
  if (pcm->channels == 2) {
    while (nsamples--) {
      signed int sample;
      sample = scale(*left_ch++);
      stream[(pcm->length - nsamples) * 4] = ((sample >> 0) & 0xff);
      stream[(pcm->length - nsamples) * 4 + 1] = ((sample >> 8) & 0xff);
      sample = scale(*right_ch++);
      stream[(pcm->length - nsamples) * 4 + 2] = ((sample >> 0) & 0xff);
      stream[(pcm->length - nsamples) * 4 + 3] = ((sample >> 8) & 0xff);
    }
    if (pa_simple_write(device_, stream, (size_t)1152 * 4, &error) < 0) {
      fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
      return error;
    }
  } else {
    printf("Mono not supported!");
  }
}

void MadDecoder::DecoderThread(std::unique_ptr<ITrackProvider> provider,
                               TrackLocation location,
                               CompletionCb completion_cb) {
  LOG("[D] decoding %s", location.c_str());
  auto ec = std::make_error_code(std::errc::interrupted);
  int error = 0;

  // TODO: better raii
  FILE *fp = nullptr;

  // make sure completion handler will always be called and cleanup
  auto interrupt_guard = CreateScopeGuard([&]() {
    LOG("[D] end of decoding %s", location.c_str());
    if (completion_cb) {
      if (error) {
        ec.assign(error, std::system_category());  // TODO: check if it's true
      }
      completion_cb(ec);
    }
    if (fp) {
      fclose(fp);
      fp = nullptr;
    }
    if (device_) {
      pa_simple_free(device_);
      device_ = nullptr;
    }
    mad_synth_finish(&mad_synth_);
    mad_frame_finish(&mad_frame_);
    mad_stream_finish(&mad_stream_);
  });

  // TODO: better raii
  mad_stream_init(&mad_stream_);
  mad_synth_init(&mad_synth_);
  mad_frame_init(&mad_frame_);

  // Set up PulseAudio 16-bit 44.1kHz stereo output
  static const pa_sample_spec ss = {
      .format = PA_SAMPLE_S16LE, .rate = 44100, .channels = 2};
  if (!(device_ = pa_simple_new(NULL, "MP3 player", PA_STREAM_PLAYBACK, NULL,
                                "playback", &ss, NULL, NULL, &error))) {
    printf("pa_simple_new() failed\n");
    return;
  }
  fp = fopen(location.c_str(), "r");
  int fd = fileno(fp);

  // Fetch file size, etc
  struct stat metadata;
  if (fstat(fd, &metadata) < 0) {
    LOG("Failed to stat %s\n", location.c_str());
    return;
  }

  // TODO: leak ?
  // NOTE: this is a complete mapping of the file !
  unsigned char *input_stream =
      (unsigned char *)mmap(0, metadata.st_size, PROT_READ, MAP_SHARED, fd, 0);
  mad_stream_buffer(&mad_stream_, input_stream, metadata.st_size);
  while (1) {
    {
      std::unique_lock<std::mutex> lock(pause_mutex_);
      pause_cv_.wait(lock, [this]() { return paused_ == false; });
    }
    if (exit_decoder_thread_) {
      return;
    }
    if (mad_frame_decode(&mad_frame_, &mad_stream_)) {
      if (MAD_RECOVERABLE(mad_stream_.error)) {
        continue;
      } else if (mad_stream_.error == MAD_ERROR_BUFLEN) {
        break;  // it is eof as file is completely mapped
      } else {
        return;
      }
    }
    mad_synth_frame(&mad_synth_, &mad_frame_);
    error = Output(&mad_frame_.header, &mad_synth_.pcm);
  }
  ec.clear();  // clear error code on success for completion_handler
}

}  // namespace ip
