#ifndef __PISTIS__FILESYSTEM__FILECREATIONMODE_HPP__
#define __PISTIS__FILESYSTEM__FILECREATIONMODE_HPP__

#include <iostream>
#include <string>

namespace pistis {
  namespace filesystem {

    class FileCreationMode {
    public:
      static const FileCreationMode CREATE_ONLY;
      static const FileCreationMode OPEN_ONLY;
      static const FileCreationMode CREATE_OR_OPEN;

    public:
      int flags() const;
      const std::string& name() const;

      bool operator==(FileCreationMode other) const {
	return ordinal_ == other.ordinal_;
      }

      bool operator!=(FileCreationMode other) const {
	return ordinal_ != other.ordinal_;
      }
      
    private:
      int ordinal_;

      FileCreationMode(int o): ordinal_(o) { }
    };

    inline std::ostream& operator<<(std::ostream& out, FileCreationMode m) {
      return out << m.name();
    }
  }
}
#endif
