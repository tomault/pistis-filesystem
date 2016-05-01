/** @file Path.cpp
 *
 *  Implementation of the functions in Path.hpp
 */

#include "Path.hpp"
#include <pistis/exceptions/IOError.hpp>
#include <sstream>
#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace pistis::exceptions;

namespace {
  std::string stripTrailingBackslashes(const std::string& p) {
    static const std::string PATH_SEPARATOR("/");
    if (p.empty()) {
      return p;
    } else {
      size_t i = p.find_last_not_of('/');
      return (i == std::string::npos) ? PATH_SEPARATOR : p.substr(0, i + 1);
    }
  }

  std::string stripLeadingBackslashes(const std::string& p) {
    static const std::string PATH_SEPARATOR("/");
    if (p.empty()) {
      return p;
    } else {
      size_t i = p.find_first_not_of('/');
      return (i == std::string::npos) ? PATH_SEPARATOR : p.substr(i);
    }    
  }

  void readStatistics(const std::string& path, struct stat& statistics) {
    if (stat(path.c_str(), &statistics) < 0) {
      throw IOError::fromSystemError("Call to stat(\"" + path +
				     "\") failed - {#ERR}", errno,
				     PISTIS_EX_HERE);
    }
  }

  bool checkFileType(const std::string& path, mode_t fileType) {
    struct stat info;
    if (!stat(path.c_str(), &info)) {
      return (info.st_mode & S_IFMT) == fileType;
    } else if ((errno == ENOENT) || (errno == ENOTDIR)) {
      return false;
    } else {
      throw IOError::fromSystemError("Call to stat(\"" + path +
				     "\") failed - {#ERR}", errno,
				     PISTIS_EX_HERE);
    }
  }

  template <typename LookupFunction, typename ErrorPrefixFunction>
  std::string lookupUserDirectory(LookupFunction lookup,
				  ErrorPrefixFunction errorPrefix) {
    static const uint32_t INITIAL_BUFFER_SIZE = 4096;
    static const uint32_t MAX_BUFFER_SIZE = 32768;
    uint32_t bufferSize = INITIAL_BUFFER_SIZE;
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    struct passwd pwd;
    struct passwd* pwdPtr = nullptr;

    while (bufferSize <= MAX_BUFFER_SIZE) {
      int result = lookup(&pwd, &pwdPtr, buffer.get(), bufferSize);
      if (pwdPtr) {
	return std::string(pwd.pw_dir);
      } else if (!result) {
	return std::string();
      } else if (result == EINTR) {
	// Loop around to try again...
      } else if (result == ERANGE) {
	bufferSize <<= 1;
      } else {
	std::ostringstream msg;
	msg << errorPrefix() << " -- {ERR}";
	throw IOError::fromSystemError(msg.str(), PISTIS_EX_HERE);
      }
    }

    std::ostringstream msg;
    msg << errorPrefix() << "-- required buffer exceeds maximum size";
    throw IOError(msg.str(), PISTIS_EX_HERE);
  }
  
  std::string lookupThisUsersDirectory() {
    return lookupUserDirectory(
	[](struct passwd* pwd, struct passwd** pwdPtr, char* buffer,
	   uint32_t bufferSize) {
	  return getpwuid_r(getuid(), pwd, buffer, bufferSize, pwdPtr);
	},
	[]() {
	  return std::string("Resolving directory for current user failed.");
	}
    );
  }

  std::string lookupUserDirectory(const std::string& userName) {
    return lookupUserDirectory(
	[&userName](struct passwd* pwd, struct passwd** pwdPtr, char* buffer,
		    uint32_t bufferSize) {
	  return getpwnam_r(userName.c_str(), pwd, buffer, bufferSize, pwdPtr);
	},
	[&userName]() {
	  std::ostringstream msg;
	  msg << "Resolving directory for user [" << userName << "] failed";
	  return msg.str();
	}
    );
  }

  inline bool isEnvVarStart(char c) {
    return isalpha(c) || (c == '_');
  }

  inline bool isEnvVarChar(char c) {
    return isalnum(c) || (c == '_');
  }
    
}

namespace pistis {
  namespace filesystem {
    namespace path {

      std::string baseName(const std::string& path) {
	size_t i = path.rfind('/');
	if (i == std::string::npos) {
	  return path;
	} else if ((i + 1) == path.size()) {
	  return std::string();
	} else {
	  return path.substr(i + 1);
	}
      }

      std::string commonPrefix(const std::string& firstPath,
			       const std::string& secondPath) {
	size_t shortLength = std::min(firstPath.size(), secondPath.size());

	if (!shortLength || (firstPath[0] != secondPath[0])) {
	  return std::string();
	} else {
	  size_t i = 1;
	  size_t j = 1;
	  size_t sameUntil = (firstPath[0] == '/') ? 1 : 0;

	  while ((i != shortLength) && (firstPath[i] == secondPath[i])) {
	    if ((firstPath[i] == '/') && (firstPath[i - 1] != '/')) {
	      sameUntil = i;
	    }
	    ++i;
	  }

	  if (i == firstPath.size()) {
	    return firstPath;  // Prefix covers entire first path
	  }
	  return firstPath.substr(0, sameUntil);
	}
      }
    
      std::string currentDirectory() {
	char* currentDir = getcwd(nullptr, 0);
	if (!currentDir) {
	  throw IOError::fromSystemError(
	      "Could not determine current directory ({ERR})",
	      errno, PISTIS_EX_HERE
	  );
	}
	std::string result(currentDir);
	free(currentDir);
	return result;
      }
    
      std::string directoryName(const std::string& path) {
	static const std::string ROOT_DIRECTORY("/");
	size_t i = path.rfind('/');
	if (i == std::string::npos) {
	  return std::string();
	} else if (i) {
	  // Forward slash in the middle of the path.
	  // There could be multiple forward slashes here.  Move i to the last
	  // character that is not a forward slash before its current position.
	  // Everything at or before this character is the directory name.
	  // If no such character exists, the path begins with all forward
	  // slashes, and so we should return a single forward slash
	  // representing the root directory.
	  i = path.find_last_not_of('/', i - 1);
	  return (i == std::string::npos) ? ROOT_DIRECTORY
	                                  : path.substr(0, i + 1);
	} else {
	  // Single forward slash at the start of the path => directory is
	  // the root directory
	  return ROOT_DIRECTORY;
	}
      }

      bool exists(const std::string& path) {
	struct stat statistics;
	if (!stat(path.c_str(), &statistics)) {
	  return true;
	} else if ((errno == ENOENT) || (errno == ENOTDIR)) {
	  return false;
	} else {
	  throw IOError::fromSystemError("Call to stat(\"" + path +
					 "\") failed - {#ERR}", errno,
					 PISTIS_EX_HERE);
	}
      }

      std::string expandUser(const std::string& path) {
	if (path.empty() || path[0] != '~') {
	  return path;
	} else if ((path.size() == 1) || (path[1] == '/')) {
	  return join(lookupThisUsersDirectory(), path.substr(2));
	} else {
	  size_t ndx = path.find('/');
	  if (ndx == std::string::npos) {
	    return lookupUserDirectory(path.substr(1));
	  } else {
	    std::string userDir = lookupUserDirectory(path.substr(1, ndx - 1));
	    if (userDir.empty()) {
	      return path;
	    } else {
	      size_t rest = path.find_first_not_of('/', ndx);
	  
	      if (rest == std::string::npos) {
		return userDir;
	      } else {
		return join(userDir, path.substr(rest));
	      }
	    }
	  }
	}
      }

      std::string expandVars(const std::string& path) {
	size_t start = 0;
	size_t ndx = path.find('$');
	std::ostringstream expansion;
      
	while ((ndx != std::string::npos) && (ndx < (path.size() - 1))) {
	  ++ndx;
	  if (ndx == path.size()) {
	    break;
	  } else if (path[ndx] == '{') {
	    // Possible variable reference of the form "${name}".
	    size_t varStart = ndx - 1;

	    // Consume the "{", then see if what follows is a legal
	    // environment variable name followed by a closing "}" character.
	    // If so, replace the variable reference with its value.
	    ++ndx;
	    if ((ndx != path.size()) && isEnvVarStart(path[ndx])) {
	      size_t nameStart = ndx;
	      while ((ndx != path.size()) && isEnvVarChar(path[ndx])) {
		++ndx;
	      }
	      if ((ndx == path.size()) || (ndx == nameStart)) {
		// This is either "${" at the end of the string, or
		// "${}".  Neither is a legal form, so leave as-is
	      } else if (path[ndx] != '}') {
		// Illegal character in variable name.  Skip to closing
		// '}' and insert everything from start.
		ndx = path.find('}', ndx + 1);
		if (ndx == std::string::npos) {
		  expansion << path.substr(start);
		  start = path.size();
		  break;
		}
		expansion << path.substr(start, ndx + 1 - start);
		start = ndx + 1;

	      } else {
		std::string varName = path.substr(nameStart, ndx - nameStart);
		char* varValue = getenv(varName.c_str());

		if (varStart != start) {
		  expansion << path.substr(start, varStart - start);
		}
		if (varValue) {
		  expansion << varValue;
		}
		start = ndx + 1;
	      }
	    }
	  
	  } else if (isEnvVarStart(path[ndx])) {
	    // Variable reference of the form "$name"
	    if (start != (ndx - 1)) {
	      expansion << path.substr(start, ndx - start - 1);
	    }

	    // Find the end of the environment variable name
	    start = ndx++;
	    while ((ndx != path.size()) && isEnvVarChar(path[ndx])) {
	      ++ndx;
	    }

	    // Expand the variable
	    std::string varName = path.substr(start, ndx - start);
	    const char* varValue = getenv(varName.c_str());
	    if (varValue) {
	      expansion << varValue;
	    }
	    start = ndx;
	  }

	  ndx = path.find('$', ndx);
	}

	if ((start != std::string::npos) && (start != path.size())) {
	  expansion << path.substr(start);
	}
	return expansion.str();
      }
    
      std::string extension(const std::string& path) {
	size_t i = path.size();
	while ((i > 1) && (path[i - 1] != '/')) {
	  --i;
	  if ((path[i] == '.') && (path[i - 1] != '/') &&
	      (path[i - 1] != '.')) {
	    return path.substr(i);
	  }
	}
	return std::string();
      }
    
      uint64_t fileSize(const std::string& path) {
	struct stat statistics;
	readStatistics(path, statistics);
	return statistics.st_size;
      }

      bool isFile(const std::string& path) {
	return checkFileType(path, S_IFREG);
      }

      bool isDirectory(const std::string& path) {
	return checkFileType(path, S_IFDIR);
      }

      bool isSymbolicLink(const std::string& path) {
	return checkFileType(path, S_IFLNK);
      }

      bool isSameFile(const std::string& firstPath,
		      const std::string& secondPath) {
	struct stat s1;
	struct stat s2;

	if (!stat(firstPath.c_str(), &s1)) {
	  if (!stat(secondPath.c_str(), &s2)) {
	    return (s1.st_dev == s2.st_dev) && (s1.st_ino == s2.st_ino);
	  } else if ((errno == ENOENT) || (errno == ENOTDIR)) {
	    return false;
	  } else {
	    throw IOError::fromSystemError("Could not stat file \"" +
					   secondPath + "\": {ERR}",
					   PISTIS_EX_HERE);
	  }
	} else if ((errno == ENOENT) || (errno == ENOTDIR)) {
	  return false;
	} else {
	  throw IOError::fromSystemError("Could not stat file \"" +
					 firstPath +"\": {ERR}",
					 PISTIS_EX_HERE);
	}
      }
    
      std::string join(const std::string& firstPath,
		       const std::string& secondPath) {
	std::string first = stripTrailingBackslashes(firstPath);
	std::string second =
	  stripLeadingBackslashes(stripTrailingBackslashes(secondPath));
	if (second.empty() || second == "/") {
	  return first;
	} else if (first.empty()) {
	  return second;
	} else if (first == "/") {
	  return first + second;
	} else {
	  return first + "/" + second;
	}
      }

      uint64_t lastAccessTime(const std::string& path) {
	struct stat statistics;
	readStatistics(path, statistics);
	return statistics.st_atime;
      }

      uint64_t lastModifiedTime(const std::string& path) {
	struct stat statistics;
	readStatistics(path, statistics);
	return statistics.st_mtime;
      }

      std::string normalizePath(const std::string& path) {
	char* tmp = realpath(path.c_str(), nullptr);
	if (!tmp) {
	  throw IOError::fromSystemError(
	      "Failed to normalize path \"" + path + "\"", PISTIS_EX_HERE
	  );
	} else {
	  std::string normalized(tmp);
	  free((void*)tmp);
	  return std::string(normalized);
	}
      }

      std::string relativePath(const std::string& path,
			       const std::string& base) {
	static const std::string THIS_DIR = ".";
	static const std::string PARENT_DIR = "..";
      
	if (isAbsolute(path) != isAbsolute(base)) {
	  return relativePath(absolutePath(path), absolutePath(base));
	} else {
	  std::vector<std::string> pathComponents = split(path);
	  std::vector<std::string> baseComponents = split(base);
	  uint32_t i = 0;
       
	  while ((i != pathComponents.size()) && (i != baseComponents.size()) &&
		 (pathComponents[i] == baseComponents[i])) {
	    ++i;
	  }

	  if ((i == baseComponents.size()) && (i == pathComponents.size())) {
	    return THIS_DIR;
	  } else {
	    std::vector<std::string> newComponents;
	    for (uint32_t j = i; j != baseComponents.size(); ++j) {
	      newComponents.push_back(PARENT_DIR);
	    }
	    while (i != pathComponents.size()) {
	      newComponents.push_back(pathComponents[i]);
	      ++i;
	    }
	    return joinSequence(newComponents.begin(), newComponents.end());
	  }
	}
      }
    
      std::tuple<std::string, std::string> splitFile(const std::string& path) {
	static const std::string ROOT_DIR("/");
	size_t i = path.rfind('/');
	if (i == std::string::npos) {
	  return std::make_tuple(std::string(), path);
	} else if (!i) {
	  return std::make_tuple(ROOT_DIR, path.substr(1));
	} else {
	  size_t j = path.find_last_not_of('/', i - 1);
	  if (j == std::string::npos) {
	    return std::make_tuple(ROOT_DIR, path.substr(i + 1));
	  }
	  return std::make_tuple(path.substr(0, j + 1), path.substr(i + 1));
	}
      }

      std::tuple<std::string, std::string> splitExtension(
	  const std::string& path
      ) {
	size_t i = path.size();
	while ((i > 1) && (path[i - 1] != '/')) {
	  --i;
	  if ((path[i] == '.') && (path[i - 1] != '/') &&
	      (path[i - 1] != '.')) {
	    return std::make_tuple(path.substr(0, i), path.substr(i));
	  }
	}
	return std::make_tuple(path, std::string());
      }

    }
  }
}

