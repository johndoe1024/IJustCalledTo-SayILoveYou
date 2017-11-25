#include "iplayer/decoder.h"

#include <assert.h>

#include "iplayer/log.h"

namespace ip {

Decoder::Decoder(const TrackLocation& track)
    : exit_decoder_thread_(false),
      status_(Status::kThreadRunning),
      decoder_thread_([this](TrackLocation track) { DecoderThread(track); },
                      track) {}

Decoder::~Decoder() {
  // Exit() must be called before to stop working thread
  assert(status_ == Status::kThreadExited);
}

void Decoder::Exit() {
  if (status_ == Status::kThreadExited) {
    return;
  }
  if (status_ == Status::kThreadPaused) {
    Unpause();
  }
  exit_decoder_thread_ = true;
  decoder_thread_.join();
  status_ = Status::kThreadExited;
}

void Decoder::Pause() {
  assert(status_ == Status::kThreadRunning);
  pause_mutex_.lock();
  status_ = Status::kThreadPaused;
}

void Decoder::Unpause() {
  assert(status_ == Status::kThreadPaused);
  pause_mutex_.unlock();
  status_ = Status::kThreadRunning;
}

void Decoder::DecoderThread(TrackLocation track) {
  TRACE();

  while (true) {
    if (exit_decoder_thread_) {
      LOG("[D] exiting");
      return;
    }
    { std::lock_guard<std::mutex> lock(pause_mutex_); }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOG("[D] chunk of %s", track.c_str());
  }
}

}  // namespace ip
