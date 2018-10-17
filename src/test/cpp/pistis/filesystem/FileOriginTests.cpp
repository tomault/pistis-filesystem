#include <pistis/filesystem/FileOrigin.hpp>
#include <gtest/gtest.h>
#include <sstream>

#include <unistd.h>

using namespace pistis::filesystem;

TEST(FileOriginTests, FlagValues) {
  EXPECT_EQ(SEEK_CUR, FileOrigin::HERE.value());
  EXPECT_EQ(SEEK_SET, FileOrigin::START.value());
  EXPECT_EQ(SEEK_END, FileOrigin::END.value());
}

TEST(FileOriginTests, Name) {
  EXPECT_EQ("HERE", FileOrigin::HERE.name());
  EXPECT_EQ("START", FileOrigin::START.name());
  EXPECT_EQ("END", FileOrigin::END.name());
}

TEST(FileOriginTests, EqualityAndInequality) {
  EXPECT_TRUE(FileOrigin::HERE == FileOrigin::HERE);
  EXPECT_TRUE(FileOrigin::HERE != FileOrigin::START);
  EXPECT_FALSE(FileOrigin::HERE == FileOrigin::START);
  EXPECT_FALSE(FileOrigin::HERE != FileOrigin::HERE);
}

TEST(FileOriginTests, WriteToStream) {
  std::ostringstream out;
  out << FileOrigin::END;
  EXPECT_EQ("END", out.str());
}
