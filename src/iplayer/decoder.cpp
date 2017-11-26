#include "iplayer/decoder.h"

#include <assert.h>

#include "iplayer/log.h"
#include "iplayer/utils/scope_guard.h"

namespace ip {

Decoder::Decoder(const TrackLocation& track, CompletionCb cb)
    : exit_decoder_thread_(false),
      status_(Status::kThreadRunning),
      decoder_thread_([this](TrackLocation track,
                             CompletionCb cb) { DecoderThread(track, cb); },
                      track, cb) {}

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

void Decoder::DecoderThread(TrackLocation track, CompletionCb completion_cb) {
  LOG("[D] decoding %s", track.c_str());
  auto ec = std::make_error_code(std::errc::interrupted);

  auto interrupt_guard = CreateScopeGuard([&]() {
    if (completion_cb) {
      completion_cb(ec);
    }
  });

  // TODO: remove this when time is handled
  for (size_t i = 0; i < 4; ++i) {
    if (exit_decoder_thread_) {
      LOG("[D] exiting");
      return;
    }
    // pause (TODO: use a condition variable instead)
    { std::lock_guard<std::mutex> lock(pause_mutex_); }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOG("[D] chunk of %s", track.c_str());
  }
  ec.clear();  // don't call completion cb with ec set
}

}  // namespace ip
