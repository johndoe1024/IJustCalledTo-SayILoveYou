#include "iplayer/playlist.h"

#include <assert.h>
#include <algorithm>
#include <functional>
#include <numeric>

// 'random' feature requirements:
// - when disabled original playlist must continue from current index
// - when enabled: all tracks get shuffled
// - when adding track: random list should keep its order (don't re-shuffle)
// - when removing track: random list should keep its order
// - when using 'previous': previous random track must be played
// - must be fast (access time and lookup for removal)
// - must not waste space (no deep-copy of playlist)
// - bonus: remove duplicates shouldn't remove current track

namespace ip {

Playlist::Playlist()
    : current_track_(0),
      repeat_playlist_(false),
      repeat_track_(false),
      random_mode_(false),
      prng_(dev_random_()) {}

Playlist::Playlist(int seed)
    : current_track_(0),
      repeat_playlist_(false),
      repeat_track_(false),
      random_mode_(false),
      prng_(seed) {}

void Playlist::AddTrack(const std::vector<TrackLocation>& tracks) {
  assert(random_mode_ ? (playlist_.size() == random_.size()) : true);

  std::transform(std::cbegin(tracks), std::cend(tracks),
                 std::back_inserter(playlist_),
                 [](const TrackLocation& loc) { return TrackInfo{loc}; });
  if (!random_mode_) {
    return;
  }

  // append to random map new indexes and randomize their positions
  for (size_t i = 0; i < tracks.size(); ++i) {
    random_.push_back(static_cast<TrackId>(playlist_.size() + i - 1));

    std::uniform_int_distribution<TrackId> random_index(
        0, static_cast<TrackId>(playlist_.size()));
    std::iter_swap(std::rbegin(random_),
                   std::begin(random_) + random_index(prng_));
  }
}

void Playlist::RemoveIf(std::function<bool(const TrackInfo&)> pred) {
  if (!random_mode_) {
    playlist_.erase(
        std::remove_if(std::begin(playlist_), std::end(playlist_), pred),
        std::end(playlist_));
    if (current_track_ >= playlist_.size()) {
      current_track_ = 0;
    }
    return;
  }

  // It was difficult to keep a complexity close to the one without random
  // playback, it would be easier to maintain consistency with a std::list.
  //
  // Delete entries while setting in 'offset' table the affected indexes
  std::vector<uint32_t> offsets(playlist_.size());

  auto first = std::begin(playlist_);
  for (auto it = std::begin(playlist_); it < std::end(playlist_); ++it) {
    if (pred(*it)) {
      uint32_t i =
          static_cast<uint32_t>(std::distance(std::begin(playlist_), it));
      offsets[i] = 1;
      continue;
    }
    *first = std::move(*it);
    ++first;
  }
  playlist_.erase(first, std::end(playlist_));

  // adjust random_ mapping values as some entries were deleted
  std::vector<TrackId> new_random(random_.size());
  std::vector<TrackId> offsets_sum(offsets.size());
  std::partial_sum(std::cbegin(offsets), std::cend(offsets),
                   std::begin(offsets_sum));

  for (size_t i = 0; i < random_.size(); ++i) {
    new_random[i] = random_[i] - offsets_sum[random_[i]];
  }

  // copy the map without the indexes to remove
  TrackId j = 0;
  for (TrackId i = 0; i < random_.size(); ++i) {
    if (offsets[random_[i]]) {
      continue;
    }
    new_random[j] = new_random[i];
    j++;
  }
  new_random.erase(std::begin(new_random) + j, std::end(new_random));
  std::swap(new_random, random_);

  if (current_track_ >= playlist_.size()) {
    current_track_ = 0;
  }
}

void Playlist::RemoveTrack(const std::unordered_set<TrackLocation>& tracks) {
  auto pred = [&](const TrackInfo& item) {
    return tracks.find(item.Location()) != std::cend(tracks);
  };
  RemoveIf(pred);
}

void Playlist::RemoveDuplicate() {
  std::unordered_set<TrackLocation> known;
  auto is_known = [&](const TrackInfo& item) {
    if (known.find(item.Location()) == std::end(known)) {
      known.insert(item.Location());
      return false;
    }
    return true;
  };
  RemoveIf(is_known);
}

Playlist::Container Playlist::GetTracks() const {
  if (!random_mode_) {
    return playlist_;
  }
  Playlist::Container playlist;
  for (size_t i = 0; i < playlist_.size(); ++i) {
    playlist.push_back(playlist_[random_[i]]);
  }
  return playlist;
}

void Playlist::SetTrackInfo(
    const std::unordered_map<TrackLocation, TrackInfo>& tracks) {
  for (auto it = std::begin(playlist_); it != std::end(playlist_); ++it) {
    auto found_it = tracks.find(it->Location());
    if (found_it != std::cend(tracks)) {
      *it = found_it->second;
    }
  }
}

std::error_code Playlist::SeekTrack(int64_t pos, SeekWay offset_type,
                                    TrackInfo* track) {
  if (repeat_track_) {
    if (track) {
      *track = CurrentTrack();
      return {};
    }
  }

  // SeekWay::kBegin
  if (offset_type == SeekWay::kBegin) {
    assert(pos >= 0);
    current_track_ = static_cast<TrackId>(pos);
    if (playlist_.size() == 0) {
      return make_error_code(std::errc::no_such_file_or_directory);
    }
  }

  // SeekWay::kCurrent
  int64_t new_pos = current_track_ + pos;
  if (new_pos < 0) {
    current_track_ = 0;
    return make_error_code(std::errc::no_such_file_or_directory);
  }
  if (static_cast<size_t>(new_pos) >= playlist_.size()) {
    current_track_ = 0;
    if (repeat_playlist_ == false) {
      return make_error_code(std::errc::no_such_file_or_directory);
    }
  } else {
    current_track_ = static_cast<TrackId>(new_pos);
  }

  // set the TrackInfo
  if (track) {
    *track = CurrentTrack();
  }
  return {};
}

TrackInfo Playlist::CurrentTrack() const {
  if (random_mode_) {
    auto it = std::next(std::begin(playlist_), random_[current_track_]);
    return *it;
  } else {
    auto it = std::next(std::begin(playlist_), current_track_);
    return *it;
  }
}

size_t Playlist::Remaining() const {
  assert(current_track_ == 0 ? true : current_track_ < playlist_.size());
  return playlist_.size() - 1 - current_track_;
}

void Playlist::SetRepeatPlaylistEnabled(bool value) {
  repeat_playlist_ = value;
}

void Playlist::SetRepeatTrackEnabled(bool value) { repeat_track_ = value; }

void Playlist::Shuffle() {
  // fill randomized with a range from 0 to playlist_.size and shuffle
  std::vector<TrackId> randomized(playlist_.size());
  std::iota(std::begin(randomized), std::end(randomized), 0);
  std::shuffle(std::begin(randomized), std::end(randomized), prng_);

  // create the mapping between real track id to real id (index on playlist_)
  random_.clear();
  std::copy(std::cbegin(randomized), std::cend(randomized),
            std::back_inserter(random_));
}

void Playlist::SetModeRandom(bool value) {
  random_mode_ = value;
  if (random_mode_) {
    Shuffle();
  } else {
    // keep current track with ordered mode
    current_track_ = random_[current_track_];
  }
}

bool Playlist::IsModeRandom() const { return random_mode_; }

}  // namespace ip
