#include "FilePermissions.hpp"
#include <sstream>
#include <tuple>
#include <vector>
#include <sys/stat.h>

using namespace pistis::filesystem;

namespace {

  static const int ALL_BITS = S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO;
  
  static const std::vector< std::tuple<int, std::string> >&
      permissionToNameMap() {
    static const std::vector< std::tuple<int, std::string> > PERM_TO_NAME{
      std::tuple<int, std::string>{ S_ISUID, std::string("SET_USER_ID") },
      std::tuple<int, std::string>{ S_ISGID, std::string("SET_GROUP_ID") },
      std::tuple<int, std::string>{ S_ISVTX, std::string("STICKY") },
      std::tuple<int, std::string>{ S_IRUSR, std::string("USER_READ") },
      std::tuple<int, std::string>{ S_IWUSR, std::string("USER_WRITE") },
      std::tuple<int, std::string>{ S_IXUSR, std::string("USER_EXECUTE") },
      std::tuple<int, std::string>{ S_IRGRP, std::string("GROUP_READ") },
      std::tuple<int, std::string>{ S_IWGRP, std::string("GROUP_WRITE") },
      std::tuple<int, std::string>{ S_IXGRP, std::string("GROUP_EXECUTE") },
      std::tuple<int, std::string>{ S_IROTH, std::string("OTHER_READ") },
      std::tuple<int, std::string>{ S_IWOTH, std::string("OTHER_WRITE") },
      std::tuple<int, std::string>{ S_IXOTH, std::string("OTHER_EXECUTE") }
    };

    return PERM_TO_NAME;
  }

  static int computePermissionIndex(int flags, int read, int write,
				    int execute) {
    return ((flags & read) ? 1 : 0) | ((flags & write) ? 2 : 0) |
           ((flags & execute) ? 4 : 0);
  }

  
}

const FilePermissions FilePermissions::NONE(0);
const FilePermissions FilePermissions::USER_READ(S_IRUSR);
const FilePermissions FilePermissions::USER_WRITE(S_IWUSR);
const FilePermissions FilePermissions::USER_EXECUTE(S_IXUSR);
const FilePermissions FilePermissions::USER_RW(S_IRUSR|S_IWUSR);
const FilePermissions FilePermissions::USER_RE(S_IRUSR|S_IXUSR);
const FilePermissions FilePermissions::USER_ALL(S_IRWXU);
const FilePermissions FilePermissions::GROUP_READ(S_IRGRP);
const FilePermissions FilePermissions::GROUP_WRITE(S_IWGRP);
const FilePermissions FilePermissions::GROUP_EXECUTE(S_IXGRP);
const FilePermissions FilePermissions::GROUP_RW(S_IRGRP|S_IWGRP);
const FilePermissions FilePermissions::GROUP_RE(S_IRGRP|S_IXGRP);
const FilePermissions FilePermissions::GROUP_ALL(S_IRWXG);
const FilePermissions FilePermissions::OTHER_READ(S_IROTH);
const FilePermissions FilePermissions::OTHER_WRITE(S_IWOTH);
const FilePermissions FilePermissions::OTHER_EXECUTE(S_IXOTH);
const FilePermissions FilePermissions::OTHER_RW(S_IROTH|S_IWOTH);
const FilePermissions FilePermissions::OTHER_RE(S_IROTH|S_IXOTH);
const FilePermissions FilePermissions::OTHER_ALL(S_IRWXO);
const FilePermissions FilePermissions::ALL_READ(S_IRUSR|S_IRGRP|S_IROTH);
const FilePermissions FilePermissions::ALL_WRITE(
    S_IWUSR|S_IWGRP|S_IWOTH
);
const FilePermissions FilePermissions::ALL_EXECUTE(
    S_IXUSR|S_IXGRP|S_IXOTH
);
const FilePermissions FilePermissions::ALL_RW(
    S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH
);
const FilePermissions FilePermissions::ALL_RE(
    S_IRUSR|S_IRGRP|S_IROTH|S_IXUSR|S_IXGRP|S_IXOTH
);
const FilePermissions FilePermissions::ALL_RWX(S_IRWXU|S_IRWXG|S_IRWXO);
const FilePermissions FilePermissions::SET_USER_ID(S_ISUID);
const FilePermissions FilePermissions::SET_GROUP_ID(S_ISGID);
const FilePermissions FilePermissions::STICKY(S_ISVTX);

std::string FilePermissions::name() const {
  if (!flags()) {
    return "NONE";
  } else {
    std::ostringstream name;
    int cnt = 0;

    for (auto& permAndName : permissionToNameMap()) {
      if (flags() & std::get<0>(permAndName)) {
	if (cnt) {
	  name << "|";
	}
	name << std::get<1>(permAndName);
	++cnt;
      }
    }
    return name.str();
  }
}

std::string FilePermissions::briefName() const {
  static const std::string PERM_NAMES[] = { "", "r", "w", "rw", "x", "rx",
					    "wx", "rwx" };
  std::ostringstream name;
  const int flags = this->flags();
  const int userIndex =
      computePermissionIndex(flags, S_IRUSR, S_IWUSR, S_IXUSR);
  const int groupIndex =
      computePermissionIndex(flags, S_IRGRP, S_IWGRP, S_IXGRP);
  const int otherIndex =
      computePermissionIndex(flags, S_IROTH, S_IWOTH, S_IXOTH);
  
  if (flags & (S_ISVTX|S_ISGID|S_ISUID)) {
    if (flags & S_ISVTX) {
      name << "s";
    }
    if (flags & S_ISUID) {
      name << "u";
    }
    if (flags & S_ISGID) {
      name << "g";
    }
    if (userIndex | groupIndex | otherIndex) {
      name << "|";
    }
  }

  if (userIndex) {
    if (userIndex == groupIndex) {
      if (groupIndex == otherIndex) {
	name << "ugo+" << PERM_NAMES[userIndex];
      } else {
	name << "ug+" << PERM_NAMES[userIndex];
	if (otherIndex) {
	  name << "|o+" << PERM_NAMES[otherIndex];
	}
      }
    } else if (userIndex == otherIndex) {
      name << "uo+" << PERM_NAMES[userIndex];
      if (groupIndex) {
	name << "|g+" << PERM_NAMES[groupIndex];
      }
    } else {
      name << "u+" << PERM_NAMES[userIndex];
      if (groupIndex) {
	name << "|g+" << PERM_NAMES[groupIndex];
      }
      if (otherIndex) {
	name << "|o+" << PERM_NAMES[otherIndex];
      }
    }
  } else if (groupIndex) {
    if (groupIndex == otherIndex) {
      name << "go+" << PERM_NAMES[groupIndex];
    } else {
      name << "g+" << PERM_NAMES[groupIndex];
      if (otherIndex) {
	name << "|o+" << PERM_NAMES[otherIndex];
      }
    }
  } else if (otherIndex) {
    name << "o+" << PERM_NAMES[otherIndex];
  }
  return name.str();
}

FilePermissions FilePermissions::operator~() const {
  return FilePermissions(~flags() & ALL_BITS);
}
