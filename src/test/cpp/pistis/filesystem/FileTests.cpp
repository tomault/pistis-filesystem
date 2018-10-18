#include <pistis/filesystem/File.hpp>
#include <pistis/exceptions/IOError.hpp>

#include "TestArtifacts.hpp"

#include <gtest/gtest.h>
#include <sstream>

using namespace pistis::exceptions;
using namespace pistis::filesystem;
namespace pt = pistis::filesystem::testing;

TEST(OpenExistingFileAndRead) {
  std::string fileName = pt::getResourcePath("test_file_1.txt");
  File file = File::open(fileName, FileCreationMode::OPEN_ONLY,
			 FileAccessMode::READ_ONLY);

  
}
