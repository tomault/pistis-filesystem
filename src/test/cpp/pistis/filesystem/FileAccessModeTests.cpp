#include <pistis/filesystem/FileAccessMode.hpp>
#include <gtest/gtest.h>

#include <sstream>
#include <fcntl.h>

using namespace pistis::filesystem;

TEST(FileAccessModeTests, FlagValues) {
  EXPECT_EQ(O_RDONLY, FileAccessMode::READ_ONLY.flags());
  EXPECT_EQ(O_WRONLY, FileAccessMode::WRITE_ONLY.flags());
  EXPECT_EQ(O_RDWR, FileAccessMode::READ_WRITE.flags());
}

TEST(FileAccessModeTests, Name) {
  EXPECT_EQ("READ_ONLY", FileAccessMode::READ_ONLY.name());
  EXPECT_EQ("WRITE_ONLY", FileAccessMode::WRITE_ONLY.name());
  EXPECT_EQ("READ_WRITE", FileAccessMode::READ_WRITE.name());
}

TEST(FileAccessModeTests, EqualityAndInequality) {
  EXPECT_TRUE(FileAccessMode::READ_ONLY == FileAccessMode::READ_ONLY);
  EXPECT_FALSE(FileAccessMode::READ_ONLY == FileAccessMode::WRITE_ONLY);
  EXPECT_TRUE(FileAccessMode::READ_ONLY != FileAccessMode::WRITE_ONLY);
  EXPECT_FALSE(FileAccessMode::READ_ONLY != FileAccessMode::READ_ONLY);
}

TEST(FileAccessModeTests, WriteToStream) {
  std::ostringstream msg;
  msg << FileAccessMode::READ_ONLY << "|" << FileAccessMode::WRITE_ONLY << "|"
      << FileAccessMode::READ_WRITE;
  EXPECT_EQ("READ_ONLY|WRITE_ONLY|READ_WRITE", msg.str());
}


