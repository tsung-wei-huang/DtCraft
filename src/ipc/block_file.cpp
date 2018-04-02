#include <dtc/ipc/block_file.hpp>

namespace dtc {

// Function: write
std::streamsize BlockFile::write(const void* buf, std::streamsize sz) const {

  issue_write:
  auto ret = ::write(_fd, buf, sz);
  
  // Case 1: error
  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_write;
    }
    else {
      throw std::system_error(
        std::make_error_code(static_cast<std::errc>(errno)), "BlockFile write failed"
      );
    }
  }
  
  return ret;
}

// Function: read
std::streamsize BlockFile::read(void* buf, std::streamsize sz) const {

  issue_read:
  auto ret = ::read(_fd, buf, sz);

  if(ret == -1) {
    if(errno == EINTR) {
      goto issue_read;
    }
    else {
      throw std::system_error(
        std::make_error_code(static_cast<std::errc>(errno)), "BlockFile read failed"
      );
    }
  }

  return ret;
}


// Function: make_block_file
std::shared_ptr<BlockFile> make_block_file(const std::filesystem::path& path, std::ios_base::openmode m) {

  int flag = O_CLOEXEC;
  
  // Find the read/write
  if((m & std::ios_base::in) && (m & std::ios_base::out)) {
    flag |= O_RDWR;
  }
  else if(m & std::ios_base::in) {
    flag |= O_RDONLY;
  }
  else if(m & std::ios_base::out) {
    flag |= O_WRONLY;
  }
  else {
    DTC_THROW("Must specify 'in' or 'out' to create a device");
  }

  // Append the write
  if(m & std::ios_base::app) {
    flag |= O_APPEND;
  }

  // Truncate
  if(m & std::ios_base::trunc) {
    flag |= O_TRUNC;
  }

  int fd = -1;

  // Temp file (O_TMPFILE cannot be used with O_CREAT)
  if(path == "") {
    fd = ::open(std::filesystem::temp_directory_path().c_str(), flag | O_TMPFILE | O_EXCL , S_IRUSR | S_IWUSR);
  }
  else {
    fd = ::open(path.c_str(), flag | O_CREAT, S_IRUSR | S_IWUSR);
  }

  if(fd == -1) {
    DTC_THROW("Failed to create a device on ", path, " (", strerror(errno), ")");
  }

  return std::make_shared<BlockFile>(fd);
}


};  // end of namespace dtc. ----------------------------------------------------------------------
