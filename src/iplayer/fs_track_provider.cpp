#include "iplayer/fs_track_provider.h"

#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <vector>

// needed to compute track length
#ifdef IPLAYER_DECODER_MAD
#include <mad.h>
#endif  // IPLAYER_DECODER_MAD

#include "iplayer/track_info.h"
#include "iplayer/track_location.h"
#include "iplayer/utils/file_mapping.h"
#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

namespace ip {

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
      files->push_back("file://" + dir + "/" + filename);
    }
    if (errno) {
      return {errno, std::generic_category()};
    }
  }
  return {};
}

std::error_code FsTrackProvider::List(
    const std::string& uri, std::vector<TrackLocation>* locations) const {
  // strip scheme
  std::string scheme{"file://"};
  auto scheme_pos = uri.find(scheme);
  if (scheme_pos == std::string::npos) {
    return std::make_error_code(std::errc::invalid_argument);
  }
  return ListDir(uri.substr(scheme.size()), locations);
}

#ifdef IPLAYER_DECODER_MAD

TrackInfo FsTrackProvider::GetTrackInfo(const TrackLocation& location) {
  TrackInfo info{location};

  struct mad_stream mad_stream;
  struct mad_header mad_header;

  // cleanup guard
  auto cleanup_guard = CreateScopeGuard([&]() {
    mad_header_finish(&mad_header);
    mad_stream_finish(&mad_stream);
  });

  mad_stream_init(&mad_stream);
  mad_header_init(&mad_header);

  // compute duration, not bullet proof way to do that and I think it would be
  // better to move this somewhere else (multiple provider might need this)
  //
  // see: https://sourcecodebrowser.com/sox/14.0.1/mp3-duration_8h.html
  // see also: idv3 tags

  // IDEA: TrackLocation should be more than a typedef on std::string,
  // until then...
  std::string separator("file://");
  auto separator_pos = location.find(separator);
  if (separator_pos == std::string::npos) {
    return{};
  }
  auto path = location.substr( + separator.size());

  FileMapping mapping(path);
  mad_timer_t total_time = mad_timer_zero;
  mad_stream_buffer(&mad_stream, mapping, mapping.size());
  while (true) {
    mad_stream.error = MAD_ERROR_NONE;
    if (mad_header_decode(&mad_header, &mad_stream) == -1) {
      if (mad_stream.error == MAD_ERROR_BUFLEN) {
        break;
      } else if (MAD_RECOVERABLE(mad_stream.error)) {
        continue;
      } else {
        LOG("MP3Stream: Unrecoverable error in mad_header_decode (%s)",
            mad_stream_errorstr(&mad_stream));
        break;
      }
    }
    mad_timer_add(&total_time, mad_header.duration);
  }
  info.SetDuration(std::chrono::seconds{total_time.seconds});
  info.SetCodec("mp3");
  return info;
}

#else

TrackInfo FsTrackProvider::GetTrackInfo(const TrackLocation& location) {
  TrackInfo info{location};
  info.SetCodec("mp3");
  return info;
}
#endif  // IPLAYER_DECODER_MAD

std::unique_ptr<ITrackIO> FsTrackProvider::OpenTrack(const TrackLocation&,
                                                     std::error_code&) {
  return {};
}

}  // namespace ip
