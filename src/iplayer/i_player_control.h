#pragma once

namespace ip {

class TrackLocation;

class IPlayerControl{
  public:
    virtual ~IPlayerControl() {}

    virtual void Play() = 0;
//    virtual void Pause() = 0;
//    virtual void Next() = 0;
//    virtual void Previous() = 0;
//    virtual void RemoveDuplicateTrack() = 0;
//    virtual void SetEnabledRandom(bool enable) = 0;
//    virtual void SetEnabledRepeat(bool enable) = 0;
    virtual void AddTrack(TrackLocation track_location) = 0;
    virtual void Shutdown() = 0;
};

}  // namespace imp
