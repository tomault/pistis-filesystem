#ifndef __PISTIS__FILESYSTEM__FILEACCESSMODE_HPP__
#define __PISTIS__FILESYSTEM__FILEACCESSMODE_HPP__

#include <ostream>

namespace pistis {
  namespace filesystem {

    class FileAccessMode {
    public:
      static const FileAccessMode READ_ONLY;
      static const FileAccessMode WRITE_ONLY;
      static const FileAccessMode READ_WRITE;

    public:
      int flags() const;
      const std::string& name() const;
	
    private:
      int ordinal_;

      FileAccessMode(int o): ordinal_(o) { }
    };

    inline std::ostream& operator<<(std::ostream& out, FileAccessMode m) {
      return out << m.name();
    }
    
  }
}
  
#endif
