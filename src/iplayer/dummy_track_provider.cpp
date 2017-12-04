#include "iplayer/dummy_track_provider.h"

#include <string>
#include <vector>

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"

namespace ip {

static uint32_t title_id = 0;

std::error_code DummyTrackProvider::List(
    const std::string& uri, std::vector<TrackLocation>* locations) const {
  *locations = {uri};
  return {};
}

TrackInfo DummyTrackProvider::GetTrackInfo(const TrackLocation& location) {
  TrackInfo track_info{location, "foobar_" + std::to_string(title_id), title_id,
                       std::chrono::seconds(5 + std::rand() % 20), "dummy"};
  ++title_id;
  return track_info;
}

std::unique_ptr<ITrackIO> DummyTrackProvider::OpenTrack(const TrackLocation&,
                                                        std::error_code&) {
  return {};
}

}  // namespace ip
