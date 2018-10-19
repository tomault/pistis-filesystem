#include "File.hpp"

#include <pistis/exceptions/IOError.hpp>

#include <algorithm>
#include <sstream>

#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

using namespace pistis::filesystem;
using namespace pistis::exceptions;

File::Buffer::Buffer(size_t initialSize, size_t maxSize):
    data_(nullptr), initialSize_(initialSize), maxSize_(maxSize), size_(0),
    current_(0), end_(0) {
}

size_t File::Buffer::fill(File* file) {
  if (!data_) {
    // Initial fill
    data_ = std::unique_ptr<uint8_t[]>(new uint8_t[initialSize_]);
    size_ = initialSize_;
    current_ = 0;
    end_ = file->read_(data_.get(), size_);
    return end_;
  } else {
    shift_();

    if (size_ == end_) {
      return 0;
    } else {
      size_t nRead = file->read_(data_.get() + end_, size_ - end_);
      end_ += nRead;
      return nRead;
    }
  }
}

size_t File::Buffer::doubleAndFill(File* file) {
  if (data_ && (size_ < maxSize_)) {
    size_t newSize = size_ << 1;
    // If size_ << 1 overflows, its value will be <= size_
    if ((newSize > maxSize_) || (newSize <= size_)) {
      newSize = maxSize_;
    }
    if (newSize > size_) {
      std::unique_ptr<uint8_t[]> newData(new uint8_t[newSize]);
      size_t nInBuffer = end_ - current_;
      if (nInBuffer) {
	::memcpy((void*)newData.get(), data_.get() + current_, nInBuffer);
      }
      data_.swap(newData);
      size_ = newSize;
      current_ = 0;
      end_ = nInBuffer;
    }
  }
  return fill(file);
}

size_t File::Buffer::empty(uint8_t* buffer, size_t n) {
  size_t nToUse = std::min(n, remaining());
  if (nToUse) {
    ::memcpy((void*)buffer, data_.get() + current_, nToUse);
    current_ += nToUse;
  }
  return nToUse;
}

void File::Buffer::clear() {
  current_ = 0;
  end_ = 0;
}

std::string File::Buffer::nextLine(File* file) {
  // First try: see if there is a line already in the buffer
  const uint8_t* pStart = data();
  const uint8_t* p = findLineEnd_(data());
  if (p) {
    current_ += p - pStart;
    return std::string(pStart, p);
  }

  // Second try: Fill the buffer and look for the end of a line.
  //             The call to fill() will shift data to the buffer start
  size_t nScanned = end_ - current_;

  fill(file);
  pStart = data();
  p = findLineEnd_(pStart + nScanned);
  if (p) {
    current_ += p - pStart;
    return std::string(pStart, p);
  }
    
  // Third try: Double the buffer size and keep looking for the end of a line.
  while (size_ < maxSize_) {
    nScanned = end_;  // Call to fill() has shifted current to 0
    doubleAndFill(file);
    pStart = data();
    p = findLineEnd_(pStart + nScanned);
    if (p) {
      current_ += p - pStart;
      return std::string(pStart, p);
    }
  }

  // The line won't fit into the buffer, even at maximum size.  Accumulate
  // the line into an ostringstream
  std::ostringstream text;
  while (true) {
    text.write((const char*)pStart, end_);
    current_ = end_;

    fill(file);
    p = findLineEnd_(pStart);
    if (p) {
      current_ += (p - pStart);
      text.write((const char*)pStart, p - pStart);
      return text.str();
    }
  }
}

void File::Buffer::shift_() {
  if (current_) {
    size_t nInBuffer = remaining();
    if (nInBuffer) {
      ::memmove(data_.get(), data_.get() + current_, nInBuffer);
    }
    current_ = 0;
    end_ = nInBuffer;
  }
}

const uint8_t* File::Buffer::findLineEnd_(const uint8_t* start) {
  const uint8_t* p = start;
  const uint8_t* pEnd = end();
  
  while (p < pEnd) {
    if (*p == '\n') {
      return p + 1;
    }
    ++p;
  }

  if (end_ < size_) {
    // Last call to file->read_() did not fill the buffer, so the buffer must
    // hit the end of the file.  Return what we have.
    return pEnd;
  }

  return nullptr;
}

File::File(int fd, size_t initialBufferSize, size_t maxBufferSize):
    fd_(fd), name_(), buffer_(initialBufferSize, maxBufferSize) {
}

File::File(int fd, const std::string& name, size_t initialBufferSize,
	   size_t maxBufferSize):
    fd_(fd), name_(name), buffer_(initialBufferSize, maxBufferSize) {
}

File::~File() {
  if (fd_) {
    close();
  }
}

size_t File::position() const {
  size_t pos = ::lseek(fd_, 0, SEEK_CUR);
  if (pos == (size_t)-1) {
    throw IOError::fromSystemError(createErrorMessage_("reading position from"),
				   PISTIS_EX_HERE);
  }
  return pos;
}

size_t File::read(void* buffer, size_t n) {
  size_t nInBuffer = buffer_.remaining();
  if (nInBuffer >= n) {
    buffer_.empty((uint8_t*)buffer, n);
  } else if (nInBuffer) {
    buffer_.empty((uint8_t*)buffer, nInBuffer);
    return nInBuffer + read_(((uint8_t*)buffer) + nInBuffer, n - nInBuffer);
  } else {
    return read_((uint8_t*)buffer, n);
  }
}

size_t File::write(const void* buffer, size_t n) {
  ssize_t nWritten = ::write(fd_, buffer, n);
  if (nWritten < 0) {
    throw IOError::fromSystemError(createErrorMessage_("writing"),
				   PISTIS_EX_HERE);
  }
  buffer_.clear();
  return nWritten;
}

size_t File::seek(FileOrigin origin, ssize_t offset) {
  size_t pos = ::lseek(fd_, offset, origin.value());
  if (pos == (size_t)-1) {
    std::string msg =
        "seeking in " + (name_.size() ? name_ : std::string("file"));
    throw IOError::fromSystemError(createErrorMessage_("seeking in"),
				   PISTIS_EX_HERE);
  }
  buffer_.clear();
  return pos;
}

void File::truncate(size_t size) {
  if (::ftruncate(fd_, size) < 0) {
    throw IOError::fromSystemError(createErrorMessage_("truncating"),
				   PISTIS_EX_HERE);
  }
  buffer_.clear();
}

void File::close() noexcept {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
}


std::string File::readLine() {
  return buffer_.nextLine(this);
}

File File::open(const std::string& name, FileCreationMode creation,
		FileAccessMode access, FileOpenOptions options,
		FilePermissions permissions, size_t initialBufferSize,
		size_t maxBufferSize) {
  int fd = ::open(name.c_str(),
		  creation.flags() | access.flags() | options.flags(),
		  permissions.flags());
  if (fd < 0) {
    throw IOError::fromSystemError("Error opening " + name + ": #ERR#",
				   PISTIS_EX_HERE);
  }
  return File(fd, name, initialBufferSize, maxBufferSize);
}

void File::unlink(const std::string& name) {
  if (::unlink(name.c_str()) < 0) {
    throw IOError::fromSystemError(createErrorMessage_(name, "removing"),
				   PISTIS_EX_HERE);
  }
}

size_t File::read_(uint8_t* buffer, size_t n) {
  ssize_t nRead = ::read(fd_, (void*)buffer, n);
  if (nRead < 0) {
    std::string msg = "reading " + (name_.size() ? name_ : std::string("file"));
    throw IOError::fromSystemError(msg, PISTIS_EX_HERE);
  }
  return nRead;
}

std::string File::createErrorMessage_(const std::string& name,
				      const std::string& action) {
  std::ostringstream msg;
  msg << "Error " << action << " ";
  if (name.size()) {
    msg << name;
  } else {
    msg << "file";
  }
  msg << ": #ERR#";
  return msg.str();
}
