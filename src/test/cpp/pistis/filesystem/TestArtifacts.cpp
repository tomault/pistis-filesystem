#include "TestArtifacts.hpp"
#include <pistis/exceptions/IOError.hpp>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>

using namespace pistis::exceptions;
using namespace pistis::filesystem::testing;

namespace {
  static std::string stripLastComponent(const std::string& path) {
    auto i = path.rfind('/');
    if (i == std::string::npos) {
      return path;
    }
    return path.substr(0, i ? i : 1);
  }

  static std::string stripTrailingPathSeparator(const std::string& path) {
    if ((path.size() > 1) && (path[path.size() - 1] == '/')) {
      size_t i = path.size() - 1;
      while ((i > 0) && (path[i - 1] == '/')) {
	--i;
      }
      return i ? path.substr(0, i) : std::string("/");
    }
    return path;
  }

  static std::string computeResourceDir() {
    const char* resourceDirEnvVar =
        getenv("PISTIS_FILESYSTEM_TEST_RESOURCE_DIR");
    if (resourceDirEnvVar) {
      return stripTrailingPathSeparator(std::string(resourceDirEnvVar));
    } else {
      std::string base = stripLastComponent(getExecutableDir());
      return base + "/resources";
    }
  }

  static std::string computeScratchDir() {
    const char* scratchDirEnvVar =
        getenv("PISTIS_FILESYSTEM_TEST_SCRATCH_DIR");
    if (scratchDirEnvVar) {
      return stripTrailingPathSeparator(std::string(scratchDirEnvVar));
    } else {
      std::string base = stripLastComponent(getExecutableDir());
      return base + "/resources";
    }
  }
}

std::string pistis::filesystem::testing::getExecutableDir() {
  char result[4096];
  ssize_t n = readlink("/proc/self/exe", result, sizeof(result));

  if (n < 0) {
    throw IOError::fromSystemError(
	"FATAL ERROR: Cannot determine executable directory (#ERR#)",
	PISTIS_EX_HERE
    );
  }

  std::string exeFile(result, n);
  return stripLastComponent(exeFile);  
}

std::string pistis::filesystem::testing::getResourceDir() {
  static const std::string RESOURCE_DIR = computeResourceDir();
  return RESOURCE_DIR;
}

std::string pistis::filesystem::testing::getResourcePath(
    const std::string& filename
) {
  if (filename.empty()) {
    return getResourceDir();
  } else if (filename[0] == '/') {
    return filename;
  } else {
    return getResourceDir() + "/" + filename;
  }
}

std::string pistis::filesystem::testing::getScratchDir() {
  static const std::string SCRATCH_DIR = computeScratchDir();
  return SCRATCH_DIR;
}
