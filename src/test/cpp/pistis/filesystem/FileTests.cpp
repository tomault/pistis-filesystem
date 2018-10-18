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
    "The text in this file is used by unit tests to verify the File "
     "implementation.\n"
     "This is the second line.\n"
     "This is the third line.\n"
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

TEST(FileTests, CreateNewFileAndWrite) {
  static const char TEXT[] = "This is a test.\nIt is only a test.\n";
  std::string fileName = pt::getScratchFile("temp_file_1.txt");

  pt::removeFile("temp_file_1.txt");
  File file = File::open(fileName, FileCreationMode::CREATE_ONLY,
			 FileAccessMode::WRITE_ONLY);
  size_t nWritten = file.write((const void*)TEXT, sizeof(TEXT) - 1);
  file.close();

  EXPECT_EQ(sizeof(TEXT) - 1, nWritten);

  file = File::open(fileName, FileCreationMode::OPEN_ONLY,
		    FileAccessMode::READ_ONLY);
  std::unique_ptr<char[]> buffer(new char[sizeof(TEXT)]);
  size_t nRead = file.read((void*)buffer.get(), nWritten);

  EXPECT_EQ(nWritten, nRead);
  EXPECT_EQ(TEXT, std::string(buffer.get(), buffer.get() + nRead));

  pt::removeFile("temp_file_1.txt");
}

TEST(FileTests, FailToOpenNonexistentFile) {
  std::string fileName = pt::getResourcePath("does_not_exist.txt");
  EXPECT_THROW(File::open(fileName, FileCreationMode::OPEN_ONLY,
			  FileAccessMode::READ_WRITE),
	       IOError);
}

TEST(FileTests, FailToOverwriteExistingFile) {
  static const char TEXT[] = "This is a test.\nIt is only a test.\n";
  std::string fileName = pt::getScratchFile("temp_file_1.txt");

  pt::removeFile("temp_file_1.txt");
  
  File file = File::open(fileName, FileCreationMode::CREATE_ONLY,
			 FileAccessMode::WRITE_ONLY);
  size_t nWritten = file.write((const void*)TEXT, sizeof(TEXT) - 1);
  file.close();

  ASSERT_EQ(sizeof(TEXT) - 1, nWritten);
  
  EXPECT_THROW(File::open(fileName, FileCreationMode::CREATE_ONLY,
			  FileAccessMode::READ_WRITE),
	       IOError);

  file = File::open(fileName, FileCreationMode::OPEN_ONLY,
		    FileAccessMode::READ_ONLY);
  std::unique_ptr<char[]> buffer(new char[sizeof(TEXT)]);
  size_t nRead = file.read((void*)buffer.get(), nWritten);

  EXPECT_EQ(nWritten, nRead);
  EXPECT_EQ(TEXT, std::string(buffer.get(), buffer.get() + nRead));

  pt::removeFile("temp_file_1.txt");
}

TEST(FileTests, SeekFromStart) {
  std::string fileName = pt::getResourcePath("test_file_1.txt");
  uint64_t fileSize = sizeOfFile(fileName);
  std::unique_ptr<char[]> data(new char[fileSize]);
  File file = File::open(fileName, FileCreationMode::OPEN_ONLY,
			 FileAccessMode::READ_ONLY);

  EXPECT_EQ(0, file.position());
  file.seek(FileOrigin::START, 16);
  EXPECT_EQ(16, file.position());

  std::unique_ptr<char[]> buffer(new char[fileSize]);
  size_t nRead = file.read((void*)buffer.get(), fileSize);

  EXPECT_EQ(fileSize - 16, nRead);
  EXPECT_EQ(TEST_FILE_1_CONTENT.substr(16),
	    std::string(buffer.get(), buffer.get() + nRead));
}

TEST(FileTests, SeekFromCurrentPosition) {
  std::string fileName = pt::getResourcePath("test_file_1.txt");
  uint64_t fileSize = sizeOfFile(fileName);
  std::unique_ptr<char[]> data(new char[fileSize]);
  File file = File::open(fileName, FileCreationMode::OPEN_ONLY,
			 FileAccessMode::READ_ONLY);

  EXPECT_EQ(0, file.position());
  file.seek(FileOrigin::START, 16);
  EXPECT_EQ(16, file.position());
  file.seek(FileOrigin::HERE, 8);
  EXPECT_EQ(24, file.position());

  std::unique_ptr<char[]> buffer(new char[fileSize]);
  size_t nRead = file.read((void*)buffer.get(), fileSize);

  EXPECT_EQ(fileSize - 24, nRead);
  EXPECT_EQ(TEST_FILE_1_CONTENT.substr(24),
	    std::string(buffer.get(), buffer.get() + nRead));
  
}

TEST(FileTests, SeekFromEnd) {
  std::string fileName = pt::getResourcePath("test_file_1.txt");
  uint64_t fileSize = sizeOfFile(fileName);
  std::unique_ptr<char[]> data(new char[fileSize]);
  File file = File::open(fileName, FileCreationMode::OPEN_ONLY,
			 FileAccessMode::READ_ONLY);

  EXPECT_EQ(0, file.position());
  file.seek(FileOrigin::END, -8);
  EXPECT_EQ(fileSize - 8, file.position());

  std::unique_ptr<char[]> buffer(new char[fileSize]);
  size_t nRead = file.read((void*)buffer.get(), fileSize);

  EXPECT_EQ(8, nRead);
  EXPECT_EQ(TEST_FILE_1_CONTENT.substr(TEST_FILE_1_CONTENT.size() - 8),
	    std::string(buffer.get(), buffer.get() + nRead));
}




