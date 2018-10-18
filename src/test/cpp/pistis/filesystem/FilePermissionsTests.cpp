#include <pistis/filesystem/FilePermissions.hpp>
#include <gtest/gtest.h>
#include <sstream>

#include <iostream>

#include <sys/stat.h>

using namespace pistis::filesystem;

TEST(FilePermissionsTests, FlagValues) {
  EXPECT_EQ(0, FilePermissions::NONE.flags());
  EXPECT_EQ(S_IRUSR, FilePermissions::USER_READ.flags());
  EXPECT_EQ(S_IWUSR, FilePermissions::USER_WRITE.flags());
  EXPECT_EQ(S_IXUSR, FilePermissions::USER_EXECUTE.flags());
  EXPECT_EQ(S_IRUSR | S_IWUSR, FilePermissions::USER_RW.flags());
  EXPECT_EQ(S_IRUSR | S_IXUSR, FilePermissions::USER_RE.flags());
  EXPECT_EQ(S_IRWXU, FilePermissions::USER_ALL.flags());
  EXPECT_EQ(S_IRGRP, FilePermissions::GROUP_READ.flags());
  EXPECT_EQ(S_IWGRP, FilePermissions::GROUP_WRITE.flags());
  EXPECT_EQ(S_IXGRP, FilePermissions::GROUP_EXECUTE.flags());
  EXPECT_EQ(S_IRGRP | S_IWGRP, FilePermissions::GROUP_RW.flags());
  EXPECT_EQ(S_IRGRP | S_IXGRP, FilePermissions::GROUP_RE.flags());
  EXPECT_EQ(S_IRWXG, FilePermissions::GROUP_ALL.flags());
  EXPECT_EQ(S_IROTH, FilePermissions::OTHER_READ.flags());
  EXPECT_EQ(S_IWOTH, FilePermissions::OTHER_WRITE.flags());
  EXPECT_EQ(S_IXOTH, FilePermissions::OTHER_EXECUTE.flags());
  EXPECT_EQ(S_IROTH | S_IWOTH, FilePermissions::OTHER_RW.flags());
  EXPECT_EQ(S_IROTH | S_IXOTH, FilePermissions::OTHER_RE.flags());
  EXPECT_EQ(S_IRWXO, FilePermissions::OTHER_ALL.flags());
  EXPECT_EQ(S_IRUSR | S_IRGRP | S_IROTH, FilePermissions::ALL_READ.flags());
  EXPECT_EQ(S_IWUSR | S_IWGRP | S_IWOTH, FilePermissions::ALL_WRITE.flags());
  EXPECT_EQ(S_IXUSR | S_IXGRP | S_IXOTH, FilePermissions::ALL_EXECUTE.flags());
  EXPECT_EQ(S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
	    FilePermissions::ALL_RW.flags());
  EXPECT_EQ(S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH,
	    FilePermissions::ALL_RE.flags());
  EXPECT_EQ(S_IRWXU | S_IRWXG | S_IRWXO, FilePermissions::ALL_RWX.flags());
  EXPECT_EQ(S_ISUID, FilePermissions::SET_USER_ID.flags());
  EXPECT_EQ(S_ISGID, FilePermissions::SET_GROUP_ID.flags());
  EXPECT_EQ(S_ISVTX, FilePermissions::STICKY.flags());
}

TEST(FilePermissionsTests, Name) {
  EXPECT_EQ("NONE", FilePermissions::NONE.name());
  EXPECT_EQ("USER_READ", FilePermissions::USER_READ.name());
  EXPECT_EQ("USER_WRITE", FilePermissions::USER_WRITE.name());
  EXPECT_EQ("USER_EXECUTE", FilePermissions::USER_EXECUTE.name());
  EXPECT_EQ("USER_READ|USER_WRITE", FilePermissions::USER_RW.name());
  EXPECT_EQ("USER_READ|USER_EXECUTE", FilePermissions::USER_RE.name());
  EXPECT_EQ("USER_READ|USER_WRITE|USER_EXECUTE",
	    FilePermissions::USER_ALL.name());
  EXPECT_EQ("GROUP_READ", FilePermissions::GROUP_READ.name());
  EXPECT_EQ("GROUP_WRITE", FilePermissions::GROUP_WRITE.name());
  EXPECT_EQ("GROUP_EXECUTE", FilePermissions::GROUP_EXECUTE.name());
  EXPECT_EQ("GROUP_READ|GROUP_WRITE", FilePermissions::GROUP_RW.name());
  EXPECT_EQ("GROUP_READ|GROUP_EXECUTE", FilePermissions::GROUP_RE.name());
  EXPECT_EQ("GROUP_READ|GROUP_WRITE|GROUP_EXECUTE",
	    FilePermissions::GROUP_ALL.name());
  EXPECT_EQ("OTHER_READ", FilePermissions::OTHER_READ.name());
  EXPECT_EQ("OTHER_WRITE", FilePermissions::OTHER_WRITE.name());
  EXPECT_EQ("OTHER_EXECUTE", FilePermissions::OTHER_EXECUTE.name());
  EXPECT_EQ("OTHER_READ|OTHER_WRITE", FilePermissions::OTHER_RW.name());
  EXPECT_EQ("OTHER_READ|OTHER_EXECUTE", FilePermissions::OTHER_RE.name());
  EXPECT_EQ("OTHER_READ|OTHER_WRITE|OTHER_EXECUTE",
	    FilePermissions::OTHER_ALL.name());
  EXPECT_EQ("USER_READ|GROUP_READ|OTHER_READ",
	    FilePermissions::ALL_READ.name());
  EXPECT_EQ("USER_WRITE|GROUP_WRITE|OTHER_WRITE",
	    FilePermissions::ALL_WRITE.name());
  EXPECT_EQ("USER_EXECUTE|GROUP_EXECUTE|OTHER_EXECUTE",
	    FilePermissions::ALL_EXECUTE.name());
  EXPECT_EQ("USER_READ|USER_WRITE|GROUP_READ|GROUP_WRITE|OTHER_READ|"
	    "OTHER_WRITE", FilePermissions::ALL_RW.name());
  EXPECT_EQ("USER_READ|USER_EXECUTE|GROUP_READ|GROUP_EXECUTE|OTHER_READ|"
	    "OTHER_EXECUTE", FilePermissions::ALL_RE.name());
  EXPECT_EQ("USER_READ|USER_WRITE|USER_EXECUTE|GROUP_READ|GROUP_WRITE|"
	    "GROUP_EXECUTE|OTHER_READ|OTHER_WRITE|OTHER_EXECUTE",
	    FilePermissions::ALL_RWX.name());
  EXPECT_EQ("SET_USER_ID", FilePermissions::SET_USER_ID.name());
  EXPECT_EQ("SET_GROUP_ID", FilePermissions::SET_GROUP_ID.name());
  EXPECT_EQ("STICKY", FilePermissions::STICKY.name());
}

TEST(FilePermissionsTests, BriefName) {
  EXPECT_EQ("", FilePermissions::NONE.briefName());
  EXPECT_EQ("u+r", FilePermissions::USER_READ.briefName());
  EXPECT_EQ("u+w", FilePermissions::USER_WRITE.briefName());
  EXPECT_EQ("u+x", FilePermissions::USER_EXECUTE.briefName());
  EXPECT_EQ("u+rw", FilePermissions::USER_RW.briefName());
  EXPECT_EQ("u+rx", FilePermissions::USER_RE.briefName());
  EXPECT_EQ("u+rwx", FilePermissions::USER_ALL.briefName());
  EXPECT_EQ("g+r", FilePermissions::GROUP_READ.briefName());
  EXPECT_EQ("g+w", FilePermissions::GROUP_WRITE.briefName());
  EXPECT_EQ("g+x", FilePermissions::GROUP_EXECUTE.briefName());
  EXPECT_EQ("g+rw", FilePermissions::GROUP_RW.briefName());
  EXPECT_EQ("g+rx", FilePermissions::GROUP_RE.briefName());
  EXPECT_EQ("g+rwx", FilePermissions::GROUP_ALL.briefName());
  EXPECT_EQ("o+r", FilePermissions::OTHER_READ.briefName());
  EXPECT_EQ("o+w", FilePermissions::OTHER_WRITE.briefName());
  EXPECT_EQ("o+x", FilePermissions::OTHER_EXECUTE.briefName());
  EXPECT_EQ("o+rw", FilePermissions::OTHER_RW.briefName());
  EXPECT_EQ("o+rx", FilePermissions::OTHER_RE.briefName());
  EXPECT_EQ("o+rwx", FilePermissions::OTHER_ALL.briefName());
  EXPECT_EQ("ugo+r", FilePermissions::ALL_READ.briefName());
  EXPECT_EQ("ugo+w", FilePermissions::ALL_WRITE.briefName());
  EXPECT_EQ("ugo+x", FilePermissions::ALL_EXECUTE.briefName());
  EXPECT_EQ("ugo+rw", FilePermissions::ALL_RW.briefName());
  EXPECT_EQ("ugo+rx", FilePermissions::ALL_RE.briefName());
  EXPECT_EQ("ugo+rwx", FilePermissions::ALL_RWX.briefName());
  EXPECT_EQ("u", FilePermissions::SET_USER_ID.briefName());
  EXPECT_EQ("g", FilePermissions::SET_GROUP_ID.briefName());
  EXPECT_EQ("s", FilePermissions::STICKY.briefName());
}

TEST(FilePermissionsTests, ToBool) {
  EXPECT_FALSE((bool)FilePermissions::NONE);
  EXPECT_TRUE((bool)FilePermissions::USER_READ);
}

TEST(FilePermissionsTests, EqualityAndInequality) {
  EXPECT_TRUE(FilePermissions::USER_ALL == FilePermissions::USER_ALL);
  EXPECT_TRUE(FilePermissions::USER_ALL != FilePermissions::GROUP_ALL);
  EXPECT_FALSE(FilePermissions::USER_ALL == FilePermissions::GROUP_ALL);
  EXPECT_FALSE(FilePermissions::USER_ALL != FilePermissions::USER_ALL);
}

TEST(FilePermissionsTests, BitwiseOr) {
  FilePermissions permissions(FilePermissions::USER_READ |
			      FilePermissions::OTHER_WRITE);
  EXPECT_EQ(S_IRUSR | S_IWOTH, permissions.flags());

  permissions |= FilePermissions::SET_USER_ID;
  EXPECT_EQ(S_IRUSR | S_IWOTH | S_ISUID, permissions.flags());

  EXPECT_EQ("u|u+r|o+w", permissions.briefName());
}

TEST(FilePermissionsTests, BitwiseAnd) {
  FilePermissions permissions(FilePermissions::USER_READ |
			      FilePermissions::OTHER_WRITE);
  ASSERT_EQ(S_IRUSR | S_IWOTH, permissions.flags());

  EXPECT_EQ(FilePermissions::USER_READ,
	    permissions & FilePermissions::USER_READ);
  EXPECT_EQ(FilePermissions::OTHER_WRITE,
	    permissions & FilePermissions::OTHER_WRITE);
  EXPECT_EQ(FilePermissions::NONE,
	    permissions & FilePermissions::SET_GROUP_ID);

  permissions &= FilePermissions::USER_READ;
  EXPECT_EQ(FilePermissions::USER_READ, permissions);
}

TEST(FilePermissionsTests, BitwiseNot) {
  FilePermissions permissions(FilePermissions::USER_ALL |
			      FilePermissions::SET_USER_ID);
  FilePermissions truth(FilePermissions::GROUP_ALL |
			FilePermissions::OTHER_ALL |
			FilePermissions::SET_GROUP_ID |
			FilePermissions::STICKY);

  ASSERT_EQ(S_IRWXU | S_ISUID, permissions.flags());
  EXPECT_EQ(truth, ~permissions);
}

TEST(FilePermissionsTests, WriteToStream) {
  FilePermissions permissions(FilePermissions::GROUP_ALL |
			      FilePermissions::OTHER_ALL |
			      FilePermissions::SET_GROUP_ID |
			      FilePermissions::STICKY);
  
  std::ostringstream out;

  out << permissions;
  EXPECT_EQ("sg|go+rwx", out.str());
}


