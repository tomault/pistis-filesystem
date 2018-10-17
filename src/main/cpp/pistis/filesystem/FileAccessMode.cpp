#include "FileAccessMode.hpp"
#include <vector>

#include <fcntl.h>

using namespace pistis::filesystem;

namespace {

  static const int FLAGS[] = { O_RDONLY, O_WRONLY, O_RDWR };
  static const std::string NAMES[]{
      std::string("READ_ONLY"), std::string("WRITE_ONLY"),
      std::string("READ_WRITE")
  };

}

const FileAccessMode FileAccessMode::READ_ONLY(0);
const FileAccessMode FileAccessMode::WRITE_ONLY(1);
const FileAccessMode FileAccessMode::READ_WRITE(2);

int FileAccessMode::flags() const { return FLAGS[ordinal_]; }

const std::string& FileAccessMode::name() const { return NAMES[ordinal_]; }


