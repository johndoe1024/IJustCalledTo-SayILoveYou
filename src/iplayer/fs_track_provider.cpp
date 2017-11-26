#include "iplayer/fs_track_provider.h"

#include <string>

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"

namespace ip {

static uint32_t title_id = 0;

TrackInfo FsTrackProvider::GetTrackInfo(const TrackLocation& location) {
  TrackInfo track_info{location, "foobar_" + std::to_string(title_id), title_id,
                       std::chrono::seconds(5), "mp3"};
  ++title_id;
  return track_info;
}

std::unique_ptr<ITrackIO> FsTrackProvider::OpenTrack(const TrackLocation&,
                                                     std::error_code&) {
  return {};
}

}  // namespace ip
