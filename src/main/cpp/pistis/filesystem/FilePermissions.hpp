#ifndef __PISTIS__FILESYSTEM__FILEPERMISSIONS_HPP__
#define __PISTIS__FILESYSTEM__FILEPERMISSIONS_HPP__

#include <ostream>
#include <string>

namespace pistis {
  namespace filesystem {

    /** @brief Enumerated constants that represent the UNIX file permissions */
    class FilePermissions {
    public:
      /** @brief No permissions are set */
      static const FilePermissions NONE;
      
      /** @brief User has read permission */
      static const FilePermissions USER_READ;

      /** @brief User has write permission */
      static const FilePermissions USER_WRITE;

      /** @brief User has execute permission */
      static const FilePermissions USER_EXECUTE;

      /** @brief User has read and write permissions */
      static const FilePermissions USER_RW;

      /** @brief User has read and execute permissions */
      static const FilePermissions USER_RE;

      /** @brief User has read, write and execute permissions */
      static const FilePermissions USER_ALL;

      /** @brief Group has read permission */
      static const FilePermissions GROUP_READ;

      /** @brief Group has write permission */
      static const FilePermissions GROUP_WRITE;

      /** @brief Group has execute permission */
      static const FilePermissions GROUP_EXECUTE;

      /** @brief Group has read and write permissions */
      static const FilePermissions GROUP_RW;

      /** @brief Group has read and execute permissions */
      static const FilePermissions GROUP_RE;

      /** @brief Group has read, write and execute permissions */
      static const FilePermissions GROUP_ALL;

      /** @brief Others have read permission */
      static const FilePermissions OTHER_READ;

      /** @brief Others have write permission */
      static const FilePermissions OTHER_WRITE;

      /** @brief Others have execute permission */
      static const FilePermissions OTHER_EXECUTE;

      /** @brief Others have read and write permissions */
      static const FilePermissions OTHER_RW;

      /** @brief Others have read and execute permissions */
      static const FilePermissions OTHER_RE;

      /** @brief User, group and others have read permission */
      static const FilePermissions ALL_READ;

      /** @brief User, group and others have write permission */
      static const FilePermissions ALL_WRITE;

      /** @brief User, group and others have execute permission */
      static const FilePermissions ALL_EXECUTE;

      /** @brief User, group and others have read and write permissions */
      static const FilePermissions ALL_RW;

      /** @brief User, group and others have read and execute permissions */
      static const FilePermissions ALL_RE;

      /** @brief User, group and others have read, write and execute
       *         permissions
       */
      static const FilePermissions ALL_RWX;
      
      /** @brief Others have read, write and execute permissions */
      static const FilePermissions OTHER_ALL;

      /** @brief File executes with priviliges of user who owns it
       *
       *  Has no effect if set on a directory.
       */
      static const FilePermissions SET_USER_ID;

      /** @brief File executes with priviliges of the group who owns it
       *
       *  If set on a directory, files created in that directory will belong
       *  to the directory's group.
       */
      static const FilePermissions SET_GROUP_ID;

      /** @brief Each file in the directory can only be renamed or deleted 
       *         by its owner, the directory's owner or priviliged processes.
       *
       *  This bit has no effect on files.
       */
      static const FilePermissions STICKY;
 
    public:
      FilePermissions(): flags_(0) { }

      int flags() const { return flags_; }
      std::string name() const;
      std::string briefName() const;

      operator bool() const { return (bool)flags_; }

      FilePermissions operator|(FilePermissions p) const {
	return FilePermissions(flags() | p.flags());
      }

      FilePermissions& operator|=(FilePermissions p) {
	flags_ |= p.flags();
	return *this;
      }

      FilePermissions operator&(FilePermissions p) const {
	return FilePermissions(flags() & p.flags());
      }

      FilePermissions operator&=(FilePermissions p) {
	flags_ &= p.flags();
	return *this;
      }

      FilePermissions operator~() const;
      
      bool operator==(FilePermissions o) const { return flags() == o.flags(); }
      bool operator!=(FilePermissions o) const { return flags() != o.flags(); }

    private:
      int flags_;

      FilePermissions(int f): flags_(f) { }
    };

    inline std::ostream& operator<<(std::ostream& out, FilePermissions p) {
      return out << p.briefName();
    }
    
  }
}
#endif
