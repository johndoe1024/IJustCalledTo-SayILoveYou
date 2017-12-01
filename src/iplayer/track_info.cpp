#include "iplayer/track_info.h"

namespace ip {

bool operator==(const TrackInfo& lhs, const TrackInfo& rhs) {
  return lhs.Location() == rhs.Location();
}

bool operator!=(const TrackInfo& lhs, const TrackInfo& rhs) {
  return lhs.Location() != rhs.Location();
}

}  // namespace ip
