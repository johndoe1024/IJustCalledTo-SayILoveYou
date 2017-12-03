#include "iplayer/utils/file_mapping.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <system_error>

#include "iplayer/utils/log.h"
#include "iplayer/utils/scope_guard.h"

namespace ip {

FileMapping::FileMapping(const std::string& path)
    : address_(nullptr), size_(0) {
  auto ec = MapFile(path);
  if (ec) {
    throw std::system_error(ec);
  }
}

FileMapping::~FileMapping() { UnMap(); }

void* FileMapping::address() const { return address_; }

size_t FileMapping::size() const { return size_; }

std::error_code FileMapping::MapFile(const std::string& path) {
  std::error_code ec;
  FILE* fp = nullptr;
  struct stat metadata;

  auto cleanup_guard = CreateScopeGuard([&]() {
    if (fp) {
      fclose(fp);
      fp = nullptr;
    }
  });

  fp = fopen(path.c_str(), "r");
  if (!fp) {
    return {errno, std::generic_category()};
  }
  int fd = fileno(fp);

  if (fstat(fd, &metadata) < 0) {
    LOG("Failed to stat %s", path.c_str());
    return {errno, std::generic_category()};
  }

  auto size = static_cast<size_t>(metadata.st_size);
  auto address = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
  if (address == MAP_FAILED) {
    return {errno, std::generic_category()};
  }
  address_ = address;
  size_ = size;
  return {};
}

void FileMapping::UnMap() {
  if (address_) {
    auto error = munmap(address_, size_);
    if (error) {
      LOG("failed to munmap input_stream: %d", errno);
    }
    address_ = nullptr;
  }
}

}  // namespace ip
