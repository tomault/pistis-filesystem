#include "FileCreationMode.hpp"
#include <vector>

#include <fcntl.h>

using namespace pistis::filesystem;

namespace {
  static const int FLAGS[]{ O_CREAT|O_EXCL, 0, O_CREAT };
  
  static const std::vector<std::string> NAMES{
    "CREATE_ONLY", "OPEN_ONLY", "CREATE_OR_OPEN"
  };

}

const FileCreationMode FileCreationMode::CREATE_ONLY(0);
const FileCreationMode FileCreationMode::OPEN_ONLY(1);
const FileCreationMode FileCreationMode::CREATE_OR_OPEN(2);

int FileCreationMode::flags() const { return FLAGS[ordinal_]; }

const std::string& FileCreationMode::name() const { return NAMES[ordinal_]; }
