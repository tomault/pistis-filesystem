/** @file Path.hpp
 *
 *  Filesystem path manipulation functions based on Python's os.path package.
 *  These functions are a temporary workaround until std::filesystem becomes
 *  available.  Currently, they only support POSIX paths and filesystems and
 *  narrow character strings.
 */

#ifndef __PISTIS__FILESYSTEM__PATH_HPP__
#define __PISTIS__FILESYSTEM__PATH_HPP__

#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace pistis {
  namespace filesystem {
    namespace path {

      std::string currentDirectory();
    
      inline bool isAbsolute(const std::string& path) {
	return !path.empty() && (path[0] == '/');
      }
    
      std::string join(const std::string& firstPath,
		       const std::string& secondPath);
    
      template <typename... Paths>
      std::string join(const std::string& firstPath,
		       const std::string& secondPath,
		       Paths&&... remainingPaths) {
	return join(join(firstPath, secondPath),
		    std::forward<Paths>(remainingPaths)...);
      }

      template <typename InputIterator>
      std::string joinSequence(InputIterator startOfPaths,
			       InputIterator endOfPaths) {
	if (startOfPaths == endOfPaths) {
	  return std::string();
	} else {
	  InputIterator i = startOfPaths;
	  std::string joined = *i;
	  
	  while (++i != endOfPaths) {
	    joined = join(joined, *i);
	  }
	  return std::move(joined);
	}
      }
    
      std::string absolutePath(const std::string& path) {
	return isAbsolute(path) ? path : join(currentDirectory(), path);
      }
    
      std::string baseName(const std::string& path);
      
      std::string commonPrefix(const std::string& firstPath,
			       const std::string& secondPath);

      template <typename... Paths>
      std::string commonPrefix(const std::string& firstPath,
			       const std::string& secondPath,
			       Paths&&... rest) {
	return commonPrefix(commonPrefix(firstPath, secondPath),
			    std::forward<Paths>(rest)...);
      }

      template <typename InputIterator>
      std::string commonPrefixFor(InputIterator startOfPaths,
				  InputIterator endOfPaths) {
	if (startOfPaths == endOfPaths) {
	  return std::string();
	} else {
	  InputIterator i = startOfPaths;
	  std::string prefix = *i;
	  while ((++i != endOfPaths) && !prefix.empty()) {
	    prefix = commonPrefix(prefix, *i);
	  }
	  return std::move(prefix);
	}
      }

      std::string directoryName(const std::string& path);

      bool exists(const std::string& path);
      std::string expandUser(const std::string& path);
      std::string expandVars(const std::string& path);
      std::string extension(const std::string& path);

      uint64_t fileSize(const std::string& path);

      bool isFile(const std::string& path);
      bool isDirectory(const std::string& path);
      bool isSymbolicLink(const std::string& path);
      bool isSameFile(const std::string& path1, const std::string& path2);

      uint64_t lastAccessTime(const std::string& path);
      uint64_t lastModifiedTime(const std::string& path);
    
      std::string normalizePath(const std::string& path);

      std::string relativePath(const std::string& path,
			       const std::string& base);

      inline std::string relativePath(const std::string& path) {
	return relativePath(path, currentDirectory());
      }
    
      template <typename Function>
      void splitAndCall(const std::string& path, Function f) {
	static const std::string EMPTY_STRING;
	static const std::string BACKSLASH("/");

	if (path.empty()) {
	  return;
	}

	size_t i = 0;

	if (path[0] == '/') {
	  // Send the backslash that represents the root and skip backslashes
	  // at the start of the string
	  f(std::string("/"));
	  i = path.find_first_not_of('/');

	  // Check this here, so we're guaranteed to enter the loop below
	  // at least once, and we don't need a flag to track whether
	  // any path elements besides the root were emitted.
	  if (i == std::string::npos) {
	    return;
	  }
	}

	while (i != std::string::npos) {
	  // At this point, path[i] != '/', so we have at least one separator
	  size_t start = i;
	  i = path.find('/', i);
	  if (i == std::string::npos) {
	    f(path.substr(start));
	  } else {
	    f(path.substr(start, i - start));
	    i = path.find_first_not_of('/', i);
	  }
	}

	// If the path ends with '/', send the empty string as the last
	// component.  The logic prior to the while loop ensures the while
	// loop executes at least once, and at least one component besides
	// the root was sent to f.
	if (path.back() == '/') {
	  f(std::string());
	}
      }
    
      inline std::vector<std::string> split(const std::string& path) {
	std::vector<std::string> components;
	splitAndCall(path, [&components](std::string&& component) {
	    components.push_back(component);
	  });
	return std::move(components);
      }

      template <typename OutputIterator>
      OutputIterator split(const std::string& path, OutputIterator output) {
	splitAndCall(path, [&output](std::string&& component) {
	    *output++ = std::move(component);
	});
	return output;
      }

      std::tuple<std::string, std::string> splitFile(const std::string& path);
      std::tuple<std::string, std::string> splitExtension(
	  const std::string& path
      );

    }
  }
}
#endif

