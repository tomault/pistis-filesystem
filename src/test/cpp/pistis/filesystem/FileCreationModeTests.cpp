#include <pistis/filesystem/FileCreationMode.hpp>
#include <gtest/gtest.h>

#include <sstream>
#include <fcntl.h>

using namespace pistis::filesystem;

TEST(FileCreationModeTests, FlagValues) {
  EXPECT_EQ(O_CREAT|O_EXCL, FileCreationMode::CREATE_ONLY.flags());
  EXPECT_EQ(0, FileCreationMode::OPEN_ONLY.flags());
  EXPECT_EQ(O_CREAT, FileCreationMode::CREATE_OR_OPEN.flags());
}

TEST(FileCreationModeTests, Name) {
  EXPECT_EQ("CREATE_ONLY", FileCreationMode::CREATE_ONLY.name());
  EXPECT_EQ("OPEN_ONLY", FileCreationMode::OPEN_ONLY.name());
  EXPECT_EQ("CREATE_OR_OPEN", FileCreationMode::CREATE_OR_OPEN.name());
}

TEST(FileCreationModeTests, EqualityAndInequality) {
  EXPECT_TRUE(FileCreationMode::CREATE_ONLY == FileCreationMode::CREATE_ONLY);
  EXPECT_TRUE(FileCreationMode::CREATE_ONLY != FileCreationMode::OPEN_ONLY);
  EXPECT_FALSE(FileCreationMode::CREATE_ONLY != FileCreationMode::CREATE_ONLY);
  EXPECT_FALSE(FileCreationMode::CREATE_ONLY == FileCreationMode::OPEN_ONLY);
}

TEST(FileCreationModeTests, WriteToStream) {
  std::ostringstream msg;
  msg << FileCreationMode::CREATE_ONLY << "|" << FileCreationMode::OPEN_ONLY
      << "|" << FileCreationMode::CREATE_OR_OPEN;
  EXPECT_EQ("CREATE_ONLY|OPEN_ONLY|CREATE_OR_OPEN", msg.str());
}
