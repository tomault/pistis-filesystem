#include <pistis/filesystem/FileOpenOptions.hpp>
#include <gtest/gtest.h>
#include <sstream>

#include <fcntl.h>

using namespace pistis::filesystem;

TEST(FileOpenOptionsTests, FlagValues) {
  EXPECT_EQ(0, FileOpenOptions::NONE.flags());
  EXPECT_EQ(O_APPEND, FileOpenOptions::APPEND.flags());
  EXPECT_EQ(O_CLOEXEC, FileOpenOptions::CLOSE_ON_EXEC.flags());
  EXPECT_EQ(O_NOATIME, FileOpenOptions::DONT_UPDATE_LAST_ACCESS_TIME.flags());
  EXPECT_EQ(O_NOFOLLOW, FileOpenOptions::DONT_FOLLOW_SYMLINKS.flags());
  EXPECT_EQ(O_TRUNC, FileOpenOptions::TRUNCATE.flags());
  EXPECT_EQ(O_SYNC, FileOpenOptions::ENSURE_FILE_INTEGRITY.flags());
}

TEST(FileOpenOptionsTests, Name) {
  EXPECT_EQ("NONE", FileOpenOptions::NONE.name());
  EXPECT_EQ("APPEND", FileOpenOptions::APPEND.name());
  EXPECT_EQ("CLOSE_ON_EXEC", FileOpenOptions::CLOSE_ON_EXEC.name());
  EXPECT_EQ("DONT_UPDATE_LAST_ACCESS_TIME",
	    FileOpenOptions::DONT_UPDATE_LAST_ACCESS_TIME.name());
  EXPECT_EQ("DONT_FOLLOW_SYMLINKS",
	    FileOpenOptions::DONT_FOLLOW_SYMLINKS.name());
  EXPECT_EQ("TRUNCATE", FileOpenOptions::TRUNCATE.name());
  EXPECT_EQ("ENSURE_FILE_INTEGRITY",
	    FileOpenOptions::ENSURE_FILE_INTEGRITY.name());
}

TEST(FileOpenOptionsTests, EqualityAndInequality) {
  EXPECT_TRUE(FileOpenOptions::APPEND == FileOpenOptions::APPEND);
  EXPECT_TRUE(FileOpenOptions::TRUNCATE != FileOpenOptions::APPEND);
  EXPECT_FALSE(FileOpenOptions::APPEND == FileOpenOptions::TRUNCATE);
  EXPECT_FALSE(FileOpenOptions::APPEND != FileOpenOptions::APPEND);
}

TEST(FileOpenOptionsTests, ToBoolean) {
  EXPECT_FALSE((bool)FileOpenOptions::NONE);
  EXPECT_TRUE((bool)FileOpenOptions::APPEND);
}

TEST(FileOpenOptionsTests, BitwiseOr) {
  FileOpenOptions options(FileOpenOptions::APPEND |
			  FileOpenOptions::DONT_FOLLOW_SYMLINKS);
  EXPECT_EQ(O_APPEND | O_NOFOLLOW, options.flags());
  EXPECT_EQ("APPEND|DONT_FOLLOW_SYMLINKS", options.name());

  options |= FileOpenOptions::ENSURE_FILE_INTEGRITY;
  EXPECT_EQ(O_APPEND | O_NOFOLLOW | O_SYNC, options.flags());
}

TEST(FileOpenOptionsTests, BitwiseAnd) {
  FileOpenOptions options(FileOpenOptions::APPEND |
			  FileOpenOptions::DONT_FOLLOW_SYMLINKS);
  ASSERT_EQ(O_APPEND | O_NOFOLLOW, options.flags());

  EXPECT_EQ(FileOpenOptions::APPEND, options & FileOpenOptions::APPEND);
  EXPECT_EQ(FileOpenOptions::DONT_FOLLOW_SYMLINKS,
	    options & FileOpenOptions::DONT_FOLLOW_SYMLINKS);
  EXPECT_EQ(FileOpenOptions::NONE, options & FileOpenOptions::TRUNCATE);

  options &= FileOpenOptions::APPEND;
  EXPECT_EQ(FileOpenOptions::APPEND, options);
}

TEST(FileOpenOptionsTests, BitwiseNot) {
  FileOpenOptions options(FileOpenOptions::APPEND |
			  FileOpenOptions::DONT_FOLLOW_SYMLINKS);
  FileOpenOptions truth(FileOpenOptions::CLOSE_ON_EXEC |
			FileOpenOptions::DONT_UPDATE_LAST_ACCESS_TIME |
			FileOpenOptions::TRUNCATE |
			FileOpenOptions::ENSURE_FILE_INTEGRITY);

  EXPECT_EQ(truth, ~options);
  EXPECT_EQ(truth.flags(), (~options).flags());
}

TEST(FileOpenOptionsTests, WriteToStream) {
  std::ostringstream out;

  out << (FileOpenOptions::APPEND | FileOpenOptions::TRUNCATE);
  EXPECT_EQ("APPEND|TRUNCATE", out.str());
}
