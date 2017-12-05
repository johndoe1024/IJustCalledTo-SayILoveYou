#pragma once

#include <string>
#include <system_error>

namespace ip {

class FileMapping {
 public:
  FileMapping(const std::string& path);
  virtual ~FileMapping();

  void* address() const;
  size_t size() const;

  operator unsigned char*() { return static_cast<unsigned char*>(address_); }

 private:
  std::error_code MapFile(const std::string& path);
  void UnMap();

  void* address_;
  size_t size_;
};

}  // namespace ip
