#include <pistis/filesystem/File.hpp>
#include <pistis/exceptions/IOError.hpp>

#include "TestArtifacts.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

using namespace pistis::exceptions;
using namespace pistis::filesystem;
namespace pt = pistis::filesystem::testing;

namespace {
  uint64_t sizeOfFile(const std::string& path) {
    struct stat statistics;
    if (stat(path.c_str(), &statistics) < 0) {
      std::string msg = "Call to stat(" + path + ") failed: #ERR#";
      throw IOError::fromSystemError(msg, PISTIS_EX_HERE);
    }
    return statistics.st_size;
  }

  std::string TEST_FILE_1_CONTENT{
    ""
  };
}

TEST(FileTests, OpenExistingFileAndRead) {
  std::string fileName = pt::getResourcePath("test_file_1.txt");
  uint64_t fileSize = sizeOfFile(fileName);
  std::unique_ptr<char[]> data(new char[fileSize]);
  File file = File::open(fileName, FileCreationMode::OPEN_ONLY,
			 FileAccessMode::READ_ONLY);
  size_t nRead = file.read((void*)data.get(), fileSize);

  EXPECT_EQ(fileSize, nRead);
  EXPECT_EQ(TEST_FILE_1_CONTENT, std::string(data.get(), data.get() + nRead));
}
