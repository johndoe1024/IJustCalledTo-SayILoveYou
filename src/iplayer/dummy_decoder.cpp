#include "iplayer/decoder.h"

#include <assert.h>

#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

namespace ip {

Decoder::Decoder(std::unique_ptr<ITrackProvider> provider,
                 const TrackLocation& track, CompletionCb cb)
    : exit_decoder_thread_(false), played_time_(std::chrono::seconds(0)) {
  decoder_future_ = std::async(std::launch::async, &Decoder::DecoderThread,
                               this, std::move(provider), track, cb);
}

Decoder::~Decoder() {
  exit_decoder_thread_ = true;
  Unpause();
}

void Decoder::Pause() { paused_ = true; }

void Decoder::Unpause() {
  {
    std::lock_guard<std::mutex> lock(pause_mutex_);
    paused_ = false;
    pause_cv_.notify_one();
  }
}

std::chrono::seconds Decoder::GetPlayedTime() const { return played_time_; }

void Decoder::DecoderThread(std::unique_ptr<ITrackProvider> provider,
                            TrackLocation location,
                            CompletionCb completion_cb) {
  LOG("[D] decoding %s", location.c_str());
  auto ec = std::make_error_code(std::errc::interrupted);

  // make sure completion handler will always be called
  auto interrupt_guard = CreateScopeGuard([&]() {
    if (completion_cb) {
      completion_cb(ec);
    }
  });

  // simulate processing
  size_t loop_count = 0;
  std::chrono::milliseconds elapsed(0);
  auto track_info = provider->GetTrackInfo(location);
  auto duration = track_info.Duration();
  while (elapsed < duration) {
    if (exit_decoder_thread_) {
      LOG("[D] exiting");
      return;
    }

    {
      std::unique_lock<std::mutex> lock(pause_mutex_);
      pause_cv_.wait(lock, [this]() { return paused_ == false; });
    }

    // update time spent playing the track
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);

    played_time_.store(
        std::chrono::duration_cast<std::chrono::seconds>(elapsed));

    if (loop_count++ % 100 == 0) {
      using namespace std::chrono;
      LOG("[D] chunk of %s (%02lu:%02lu / %02lu:%02lu)", location.c_str(),
          duration_cast<minutes>(elapsed).count(),
          duration_cast<seconds>(elapsed).count() % 60,
          duration_cast<minutes>(duration).count(),
          duration_cast<seconds>(duration).count() % 60);
    }
  }

  ec.clear();  // clear error code on success for completion_handler
}

}  // namespace ip
