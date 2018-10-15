#include "FileOrigin.hpp"
#include <sys/types.h>
#include <unistd.h>

using namespace pistis::filesystem;

namespace {
  static const int SEEK_WHENCE[] = { SEEK_CUR, SEEK_SET, SEEK_END };
  static const std::string NAMES[]{ "HERE", "START", "END" };
}

const FileOrigin FileOrigin::HERE(0);
const FileOrigin FileOrigin::START(1);
const FileOrigin FileOrigin::END(2);

int FileOrigin::value() const { return SEEK_WHENCE[ordinal_]; }
const std::string& FileOrigin::name() const { return NAMES[ordinal_]; }
