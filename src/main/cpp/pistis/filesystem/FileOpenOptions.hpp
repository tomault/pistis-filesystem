#ifndef __PISTIS__FILESYSTEM__FILEOPENOPTIONS_HPP__
#define __PISTIS__FILESYSTEM__FILEOPENOPTIONS_HPP__

#include <string>

namespace pistis {
  namespace filesystem {

    /** @brief Flags that modify aspects of a file's behavior */
    class FileOpenOptions {
    public:
      /** @brief No options */
      static const FileOpenOptions NONE;
      
      /** @brief Always write to end of file */
      static const FileOpenOptions APPEND;

      /** @brief Close file when exec() called */
      static const FileOpenOptions CLOSE_ON_EXEC;

      /** @brief Don't change the file's last access time */
      static const FileOpenOptions DONT_UPDATE_LAST_ACCESS_TIME;

      /** @brief Fail to open the file if it is a symlink */
      static const FileOpenOptions DONT_FOLLOW_SYMLINKS;

      /** @brief Truncate the file after opening */
      static const FileOpenOptions TRUNCATE;

      /** @brief Ensure data integrity by flushing data and any metadata
       *         needed to read that data back to the underlying hardware
       *         before any write operation returns.
       */
      //static const FileOpenOptions ENSURE_DATA_INTEGRITY;

      /** @brief Ensure file integrity by flushing data and metadata to
       *         the underlying hardware before any write operation returns.
       *
       *  The difference between ENSURE_FILE_INTEGRITY and
       *  ENSURE_DATA_INTEGRITY is the former flushes all metadata, while
       *  the latter only flushes metadata needed to read the data written
       *  back correctly.
       */
      static const FileOpenOptions ENSURE_FILE_INTEGRITY;
      
    public:
      FileOpenOptions() : flags_(0) { }
      
      int flags() const { return flags_; }
      std::string name() const;

      operator bool() const { return (bool)flags_; }

      FileOpenOptions operator|(FileOpenOptions o) const {
	return FileOpenOptions(flags() | o.flags());
      }
      
      FileOpenOptions& operator|=(FileOpenOptions o) {
	flags_ |= o.flags();
	return *this;
      }
      
      FileOpenOptions operator&(FileOpenOptions o) const {
	return FileOpenOptions(flags() & o.flags());
      }
      
      FileOpenOptions& operator&=(FileOpenOptions o) {
	flags_ &= o.flags();
	return *this;
      }

      FileOpenOptions operator~() const;
      
      bool operator==(FileOpenOptions o) const { return flags() == o.flags(); }
      bool operator!=(FileOpenOptions o) const { return flags() != o.flags(); }
      
    private:
      int flags_;

      explicit FileOpenOptions(int f): flags_(f) { }
    };

    inline std::ostream& operator<<(std::ostream& out, FileOpenOptions o) {
      return out << o.name();
    }
    
  }
}
#endif
