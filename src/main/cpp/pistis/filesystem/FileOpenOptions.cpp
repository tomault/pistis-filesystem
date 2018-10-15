#include "FileOpenOptions.hpp"
#include <sstream>
#include <tuple>
#include <vector>

#include <fcntl.h>

using namespace pistis::filesystem;

namespace {
  static const int ALL_BITS =
      O_APPEND|O_CLOEXEC|O_NOATIME|O_NOFOLLOW|O_TRUNC|O_DSYNC|O_SYNC;

  static const std::vector< std::tuple<int, std::string> >&
      optionToNameMap() {
    static const std::vector< std::tuple<int, std::string> > OPTION_TO_NAME{
      std::tuple<int, std::string>{ O_APPEND, std::string("APPEND") },
      std::tuple<int, std::string>{ O_CLOEXEC, std::string("CLOSE_ON_EXEC") },
      std::tuple<int, std::string>{
	  O_NOATIME,  std::string("DONT_UPDATE_LAST_ACCESS_TIME")
      },
      std::tuple<int, std::string>{ O_NOFOLLOW,
	                            std::string("DONT_FOLLOW_SYMLINKS") },
      std::tuple<int, std::string>{ O_TRUNC, std::string("TRUNCATE") },
      /* std::tuple<int, std::string>{
          (int)O_DYSNC, std::string("ENSURE_DATA_INTEGRITY") 
      } , */
      std::tuple<int, std::string>{ O_SYNC,
	                            std::string("ENSURE_FILE_INTEGRITY") }
    };

    return OPTION_TO_NAME;
  }
  
}

const FileOpenOptions FileOpenOptions::NONE(0);
const FileOpenOptions FileOpenOptions::APPEND(O_APPEND);
const FileOpenOptions FileOpenOptions::CLOSE_ON_EXEC(O_CLOEXEC);
const FileOpenOptions FileOpenOptions::DONT_UPDATE_LAST_ACCESS_TIME(O_NOATIME);
const FileOpenOptions FileOpenOptions::DONT_FOLLOW_SYMLINKS(O_NOFOLLOW);
const FileOpenOptions FileOpenOptions::TRUNCATE(O_TRUNC);
/* const FileOpenOptions FileOpenOptions::ENSURE_DATA_INTEGRITY(O_DSYNC); */
const FileOpenOptions FileOpenOptions::ENSURE_FILE_INTEGRITY(O_SYNC);

std::string FileOpenOptions::name() const {
  if (!flags()) {
    return "NONE";
  } else {
    std::ostringstream name;
    int cnt = 0;

    for (auto& optionAndName : optionToNameMap()) {
      if (flags() & std::get<0>(optionAndName)) {
	if (cnt) {
	  name << "|";
	}
	name << std::get<1>(optionAndName);
	++cnt;
      }
    }
    return name.str();
  }
}

FileOpenOptions FileOpenOptions::operator~() const {
  return FileOpenOptions(~flags() & ALL_BITS);
}


