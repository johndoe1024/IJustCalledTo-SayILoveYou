#pragma once

#include <string>

namespace ip {

class TrackLocation{
  public:
    // TODO: set a default scheme if URI doesn't have one
    TrackLocation(const std::string& uri) : uri_(uri) {}
    std::string Uri() const;
  private:
    std::string uri_;
};

}  // namespace imp
