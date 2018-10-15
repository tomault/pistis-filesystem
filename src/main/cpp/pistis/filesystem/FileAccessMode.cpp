#include "FileAccessMode.hpp"
#include <vector>

#include <fcntl.h>

using namespace pistis::filesystem;

namespace {

  static const int FLAGS[] = { O_RDONLY, O_WRONLY, O_RDWR };
  static const std::string NAMES[]{
      std::string("O_RDONLY"), std::string("O_WRONLY"), std::string("O_RDWR")
  };

}

const FileAccessMode FileAccessMode::READ_ONLY(0);
const FileAccessMode FileAccessMode::WRITE_ONLY(1);
const FileAccessMode FileAccessMode::READ_WRITE(2);

int FileAccessMode::flags() const { return FLAGS[ordinal_]; }

const std::string& FileAccessMode::name() const { return NAMES[ordinal_]; }


