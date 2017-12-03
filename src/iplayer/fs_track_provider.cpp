#include "iplayer/fs_track_provider.h"

#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"
#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

namespace ip {

static uint32_t title_id = 0;

std::error_code FsTrackProvider::ListDir(
    std::string dir, std::vector<std::string>* files) const {
  DIR* dp = nullptr;
  auto cleanup = CreateScopeGuard([&]() {
    if (dp) {
      closedir(dp);
      dp = nullptr;
    }
  });

  struct dirent64* dirp;
  if ((dp = opendir(dir.c_str())) == nullptr) {
    std::error_code ec{errno, std::generic_category()};
    LOG("error opening '%s': %s", dir.c_str(), ec.message().c_str());
    return ec;
  }
  {
    // protect dirent64
    std::lock_guard<std::mutex> lock(mutex_);

    errno = 0;  // see manpages
    while ((dirp = readdir64(dp)) != nullptr) {
      std::string filename(dirp->d_name);

      const std::string ext(".mp3");
      if (!std::equal(std::rbegin(ext), std::rend(ext),
                      std::rbegin(filename))) {
        continue;
      }
      files->push_back(dir + "/" + filename);
    }
    if (errno) {
      return {errno, std::generic_category()};
    }
  }
  return {};
}

std::error_code FsTrackProvider::List(
    const std::string& uri, std::vector<TrackLocation>* locations) const {
  return ListDir(uri, locations);
}

TrackInfo FsTrackProvider::GetTrackInfo(const TrackLocation& location) {
  std::vector<std::string> codecs{{"mp3", "aac", "m4a", "flac", "wav"}};

  TrackInfo track_info{
      location, "foobar_" + std::to_string(title_id), title_id,
      std::chrono::seconds(5 + std::rand() % 20),
      codecs[static_cast<size_t>(std::rand()) % codecs.size()]};
  ++title_id;
  return track_info;
}

std::unique_ptr<ITrackIO> FsTrackProvider::OpenTrack(const TrackLocation&,
                                                     std::error_code&) {
  return {};
}

}  // namespace ip
