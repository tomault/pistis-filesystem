#ifndef __PISTIS__FILESYSTEM__FILEORIGIN_HPP__
#define __PISTIS__FILESYSTEM__FILEORIGIN_HPP__

#include <ostream>
#include <string>

namespace pistis {
  namespace filesystem {

    class FileOrigin {
    public:
      /** @brief Seek relative to current position in the file */
      static const FileOrigin HERE;

      /** @brief Seek relative to the start of the file */
      static const FileOrigin START;

      /** @brief Seek relative to the end of the file */
      static const FileOrigin END;

    public:
      FileOrigin() : ordinal_(0) { }

      int value() const;
      const std::string& name() const;

      bool operator==(FileOrigin other) const {
	return value() == other.value();
      }
      bool operator!=(FileOrigin other) const {
	return value() != other.value();
      }

    private:
      int ordinal_;

      FileOrigin(int o): ordinal_(o) { }
    };

    inline std::ostream& operator<<(std::ostream& out, FileOrigin o) {
      return out << o.name();
    }
    
  }
}
#endif
