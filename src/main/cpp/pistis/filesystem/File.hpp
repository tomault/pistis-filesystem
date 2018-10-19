#ifndef __PISTIS__FILESYSTEM__FILE_HPP__
#define __PISTIS__FILESYSTEM__FILE_HPP__

/** @file File.hpp
 *
 *  Declaration of pistis::filesystem::File, a simple file representation.
 */

#include <pistis/filesystem/FileAccessMode.hpp>
#include <pistis/filesystem/FileCreationMode.hpp>
#include <pistis/filesystem/FileOpenOptions.hpp>
#include <pistis/filesystem/FileOrigin.hpp>
#include <pistis/filesystem/FilePermissions.hpp>

#include <memory>
#include <vector>

#include <stdint.h>

namespace pistis {
  namespace filesystem {

    /** @brief A simple wrapper around a file descriptor */
    class File {
    public:
      static const size_t INITIAL_BUFFER_SIZE = 1024;
      static const size_t MAX_BUFFER_SIZE = 128 * 1024 * 1024;
      
    public:
      File(int fd, size_t initialBufferSize = INITIAL_BUFFER_SIZE,
	   size_t maxBufferSize = MAX_BUFFER_SIZE);
      File(int fd, const std::string& name,
	   size_t initialBufferSize = INITIAL_BUFFER_SIZE,
	   size_t maxBufferSize = MAX_BUFFER_SIZE);
      File(const File&) = delete;
      File(File&& other) = default;
      ~File();

      int fd() const { return fd_; }
      const std::string& name() const { return name_; }
      size_t position() const;
      
      size_t read(void* buffer, size_t n);
      size_t write(const void* buffer, size_t n);
      size_t seek(ssize_t offset) { return seek(FileOrigin::HERE, offset); }
      size_t seek(FileOrigin origin, ssize_t offset);
      void truncate() { truncate(0); }
      void truncate(size_t size);
      
      void close() noexcept;

      std::string readLine();

      std::vector<std::string> readLines() {
	std::vector<std::string> lines;
	eachLine([&lines](const std::string& l) { lines.push_back(l); });
	return std::move(lines);
      }

      template <typename Function>
      void eachLine(Function f) {
	std::string tmp = readLine();
	while (!tmp.empty()) {
	  f(tmp);
	  tmp = readLine();
	}
      }
      
      template <typename Function>
      void eachChunk(size_t n, Function f) {
	std::unique_ptr<uint8_t[]> buffer(new uint8_t[n]);
	eachChunk(n, buffer.get(), f);
      }

      template <typename Function>
      void eachChunk(size_t n, uint8_t* buffer, Function f) {
	size_t nRead = read(buffer, n);
	while (nRead == n) {
	  f(buffer, nRead);
	  nRead = read(buffer, n);
	}
	if (nRead) {
	  f(buffer, nRead);
	}
      }

      File& operator=(const File& other) = delete;
      File& operator=(File&& other) {
	if (this != &other) {
	  close();
	  fd_ = other.fd_;
	  other.fd_ = -1;
	  name_ = std::move(other.name_);
	  buffer_ = std::move(other.buffer_);
	}
	return *this;
      }
      static File open(const std::string& name,
		       FileAccessMode access = FileAccessMode::READ_WRITE,
		       FileOpenOptions options = FileOpenOptions::NONE,
		       size_t initialBufferSize = INITIAL_BUFFER_SIZE,
		       size_t maxBufferSize = MAX_BUFFER_SIZE) {
	return File::open(name, FileCreationMode::CREATE_OR_OPEN, access,
			  options, FilePermissions::ALL_RW,
			  initialBufferSize, maxBufferSize);
      }

      static File open(const std::string& name, FileCreationMode creation,
		       FileAccessMode access,
		       FileOpenOptions options = FileOpenOptions::NONE,
		       FilePermissions permissions = FilePermissions::ALL_RW,
		       size_t initialBufferSize = INITIAL_BUFFER_SIZE,
		       size_t maxBufferSize = MAX_BUFFER_SIZE);
      static void unlink(const std::string& name);

    private:
      class Buffer {
      public:
	Buffer(size_t initialSize, size_t maxSize);
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&&) = default;

	uint8_t* data() const { return data_.get() + current_; }
	uint8_t* end() const { return data_.get() + end_; }
	size_t size() const { return size_; }
	size_t remaining() const { return end_ - current_; }

	size_t fill(File* file);
	size_t doubleAndFill(File* file);
	size_t empty(uint8_t* buffer, size_t n);
	std::string nextLine(File* file);
	void clear();

	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&&) = default;
	
      private:
	std::unique_ptr<uint8_t[]> data_;
	size_t initialSize_;
	size_t maxSize_;
	size_t size_;
	size_t current_;
	size_t end_;

	void shift_();
	const uint8_t* findLineEnd_(const uint8_t* start);
      };
	
    private:
      int fd_;
      std::string name_;
      Buffer buffer_;

      size_t read_(uint8_t* buffer, size_t n);
      std::string createErrorMessage_(const std::string& action) const {
	return createErrorMessage_(name_, action);
      }

      static std::string createErrorMessage_(const std::string& name,
					     const std::string& action);
      
      friend class File::Buffer;
    };
  }
}

#endif
