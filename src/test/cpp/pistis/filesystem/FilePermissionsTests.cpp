#include <pistis/filesystem/FilePermissions.hpp>
#include <gtest/gtest.h>
#include <sstream>

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
}
