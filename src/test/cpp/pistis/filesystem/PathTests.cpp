/** @file PathTests.cpp
 *
 *  Unit tests for the functions in pistis/filesystem/Path.hpp
 */
#include <pistis/filesystem/Path.hpp>

#include <pistis/exceptions/IOError.hpp>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace pistis::exceptions;
using namespace pistis::filesystem::path;

namespace {
  class TemporaryFile {
  public:
    TemporaryFile(const std::string& name):
        name_(name), fd_(open(name.c_str(), O_RDWR|O_CREAT|O_EXCL, 0666)) {
      if (fd_ < 0) {
	throw IOError::fromSystemError("Cannot create " + name + ": {ERR}",
				       PISTIS_EX_HERE);
      }
    }
    ~TemporaryFile() { close(); }

    const std::string& name() const { return name_; }
    int fd() const { return fd_; }

    void write(const void* data, size_t n) {
      auto result = ::write(fd_, data, n);
      if (result < 0) {
	std::ostringstream msg;
	msg << "Writing " << n << " bytes to " << name_ << " failed: {ERR}";
	throw IOError::fromSystemError(msg.str(), PISTIS_EX_HERE);
      } else if (result < n) {
	std::ostringstream msg;
	msg << "Writing " << n << " bytes to " << name_
	    << " failed: Only " << result << " bytes were written";
	throw IOError(msg.str(), PISTIS_EX_HERE);
      }
    }

    void flush() {
      fsync(fd_);
    }
    
    void close() {
      if (fd_) {
	::close(fd_);
	unlink(name_.c_str());
	fd_ = -1;
      }
    }

  private:
    std::string name_;
    int fd_;
  };

  class TemporaryDirectory {
  public:
    TemporaryDirectory(const std::string& name):
        name_(name) {
      if (mkdir(name.c_str(), 0777)) {
	throw IOError::fromSystemError(
            "Could not create directory " + name + ": {ERR}", PISTIS_EX_HERE
	);
      }
    }
    ~TemporaryDirectory() {
      rmdir(name_.c_str());
    }

    const std::string name() const { return name_; }

  private:
    std::string name_;
  };

  std::string createTempName(const std::string& prefix,
			     const std::string& extension) {
    std::ostringstream tmp;
    tmp << prefix << getpid() << extension;
    return tmp.str();
  }
  
  ::testing::AssertionResult endsWith(const std::string& s,
				      const std::string& suffix) {
    auto i = s.end();
    auto j = suffix.end();

    while (j != suffix.begin()) {
      if ((i == s.begin()) || (*--i != *--j)) {
	return ::testing::AssertionFailure()
	  << "[" << s << "] does not end with [" << suffix << "]";
      }
    }

    return ::testing::AssertionSuccess();
  }
}

TEST(PathTests, AbsolutePath) {
  static const std::string RELATIVE_PATH("foo/bar/faz");
  std::string absPath = absolutePath(RELATIVE_PATH);

  EXPECT_TRUE(endsWith(absPath, RELATIVE_PATH));
  EXPECT_NE(RELATIVE_PATH, absPath);
  EXPECT_EQ('/', absPath[0]);
  EXPECT_EQ(currentDirectory() + "/" + RELATIVE_PATH, absPath);
  EXPECT_EQ("/foo/bar/baz", absolutePath("/foo/bar/baz"));
}

TEST(PathTests, BaseName) {
  EXPECT_EQ("some_directory", baseName("some_directory"));
  EXPECT_EQ("baz.txt", baseName("/foo/bar/baz.txt"));
  EXPECT_EQ("", baseName("/foo/bar/baz/"));
}

TEST(PathTests, CommonPrefix) {
  EXPECT_EQ("/foo/bar",
	    commonPrefix("/foo/bar/alpha/beta", "/foo/bar/alpha",
			 "/foo/bar/delta/gamma/epsilon"));
  EXPECT_EQ("/", commonPrefix("/alpha", "/beta"));
  EXPECT_EQ("/alpha/beta", commonPrefix("/alpha/beta", "/alpha/beta"));
  EXPECT_EQ("", commonPrefix("alpha", "beta"));
  EXPECT_EQ("", commonPrefix("/alpha/beta/gamma", ""));
  EXPECT_EQ("", commonPrefix("", "/alpha/beta/gamma"));
  EXPECT_EQ("/alpha", commonPrefix("/alpha//beta", "/alpha//beatbox"));
  EXPECT_EQ("alpha/beta/gamma",
	    commonPrefix("alpha/beta/gamma/delta",
			 "alpha/beta/gamma/epsilon"));

  std::vector<std::string> paths{
      "/foo/bar/alpha/beta", "/foo/bar/alpha", "/foo/bar/delta/gamma/epsilon"
  };
  EXPECT_EQ("/foo/bar", commonPrefixFor(paths.begin(), paths.end()));
}

TEST(PathTests, DirectoryName) {
  EXPECT_EQ("", directoryName("some_directory"));
  EXPECT_EQ("/foo/bar", directoryName("/foo/bar/baz.txt"));
  EXPECT_EQ("/foo", directoryName("/foo//bar"));
  EXPECT_EQ("/foo/bar/baz", directoryName("/foo/bar/baz/"));
}

TEST(PathTests, Exists) {
  TemporaryFile tempFile(createTempName("testing", ".txt"));
  EXPECT_TRUE(exists(tempFile.name()));
  EXPECT_FALSE(exists(createTempName("does_not_exist", ".txt")));
}

TEST(PathTests, ExpandUser) {
  std::string expanded = expandUser("~/some/path");

  EXPECT_EQ("/foo/bar/baz", expandUser("/foo/bar/baz"));
  EXPECT_TRUE(endsWith(expanded, "/some/path"));
  EXPECT_EQ('/', expanded[0]);
  EXPECT_LT(11, expanded.size());

  EXPECT_EQ("/root/foo/bar", expandUser("~root/foo/bar"));
}

TEST(PathTests, ExpandVars) {
  setenv("tweepadock", "elephant", 1);
  unsetenv("a");

  EXPECT_EQ("/dont/ignore/the/elephant/in/the/room",
	    expandVars("/dont/ignore/the/${tweepadock}/in/the/room"));
  EXPECT_EQ("/foo/bar/.txt", expandVars("/foo/bar/${a}.txt"));
  EXPECT_EQ("/foo/elephant/bar/elephant",
	    expandVars("/foo/${tweepadock}/bar/${tweepadock}"));
  EXPECT_EQ("/foo/${9stop}/bar/${", expandVars("/foo/${9stop}/bar/${"));
  EXPECT_EQ("/foo/${}/${bad$name}", expandVars("/foo/${}/${bad$name}"));
  EXPECT_EQ("/foo/${bad$name", expandVars("/foo/${bad$name"));
  EXPECT_EQ("/foo/elephantelephant${bad-name}elephant",
	    expandVars(
	        "/foo/${tweepadock}${tweepadock}${bad-name}${tweepadock}"
	    ));
  EXPECT_EQ("/foo/${not_closed", expandVars("/foo/${not_closed"));

  EXPECT_EQ("/foo/elephant/bar", expandVars("/foo/$tweepadock/bar"));
  EXPECT_EQ("elephant-elephant.txt",
	    expandVars("$tweepadock-$tweepadock.txt"));
  EXPECT_EQ("/this/is/an/elephant", expandVars("/this/is/an/${tweepadock}"));
  EXPECT_EQ("/foo/.txt", expandVars("/foo/${a}.txt"));
  EXPECT_EQ("/foo/$-bad/bar/elephant",
	    expandVars("/foo/$-bad/bar/$tweepadock"));
  EXPECT_EQ("$", expandVars("$"));
  EXPECT_EQ("", expandVars(""));
}

TEST(PathTests, Extension) {
  EXPECT_EQ(".txt", extension("/foo/bar.bar/baz.txt"));
  EXPECT_EQ("", extension("/foo/bar.bar/baz"));
  EXPECT_EQ("", extension(".hidden"));
  EXPECT_EQ("", extension("/home/somebody/.hidden"));
  EXPECT_EQ("", extension("."));
  EXPECT_EQ("", extension(".."));
  EXPECT_EQ(".", extension("file."));
}

TEST(PathTests, FileSize) {
  static const char TEXT[] = "This is a test.";
  TemporaryFile tempFile(createTempName("testing", ".txt"));

  tempFile.write(TEXT, sizeof(TEXT) - 1);
  tempFile.flush();
  
  EXPECT_EQ(sizeof(TEXT) - 1, fileSize(tempFile.name()));
}

TEST(PathTests, IsFile) {
  TemporaryFile tempFile(createTempName("testing", ".txt"));
  TemporaryDirectory tempDirectory(createTempName("dir", ""));

  EXPECT_TRUE(isFile(tempFile.name()));
  EXPECT_FALSE(isFile(tempDirectory.name()));
  EXPECT_FALSE(isFile(createTempName("does_not_exist", ".txt")));
}

TEST(PathTests, IsDirectory) {
  TemporaryFile tempFile(createTempName("testing", ".txt"));
  TemporaryDirectory tempDirectory(createTempName("dir", ""));

  EXPECT_FALSE(isDirectory(tempFile.name()));
  EXPECT_TRUE(isDirectory(tempDirectory.name()));
  EXPECT_FALSE(isDirectory(createTempName("does_not_exist", ".txt")));
}

TEST(PathTests, SameFile) {
  TemporaryFile tempFile(createTempName("testing", ".txt"));
  TemporaryFile secondFile(createTempName("second", ".txt"));
  std::string doesntExist = createTempName("does_not_exist", ".txt");

  EXPECT_TRUE(isSameFile(tempFile.name(), "./" + tempFile.name()));
  EXPECT_FALSE(isSameFile(tempFile.name(), secondFile.name()));
  EXPECT_FALSE(isSameFile(doesntExist, doesntExist));
  EXPECT_FALSE(isSameFile(tempFile.name(), doesntExist));
}

TEST(PathTests, Join) {
  EXPECT_EQ("foo/bar", join("foo", "bar"));
  EXPECT_EQ("/foo", join("/", "foo"));
  EXPECT_EQ("foo", join("foo", "/"));
  EXPECT_EQ("foo", join("foo", ""));
  EXPECT_EQ("foo", join("foo", "//"));
  EXPECT_EQ("foo", join("", "foo"));
  EXPECT_EQ("/foo/bar", join("/foo/", "//bar//"));
  EXPECT_EQ("/foo", join("//", "foo"));

  EXPECT_EQ("/foo/bar/baz", join("/foo", "bar", "baz"));

  std::vector<std::string> components{"foo", "bar", "baz"};
  EXPECT_EQ("foo/bar/baz", joinSequence(components.begin(), components.end()));
}

TEST(PathTests, NormalizePath) {
  TemporaryDirectory tempDir(createTempName("dir", ""));
  std::string tempFileName = createTempName("testing", ".txt");
  TemporaryFile tempFile(tempDir.name() + "/" + tempFileName);
  
  EXPECT_EQ(currentDirectory() + "/" + tempDir.name() + "/" + tempFileName,
	    normalizePath(tempDir.name() + "/../" + tempDir.name() +
			  "/./" + tempFileName));
}

TEST(PathTests, RelativePath) {
  EXPECT_EQ("../gamma/delta",
	    relativePath("/alpha/beta/gamma/delta", "/alpha/beta/epsilon"));
  EXPECT_EQ("../../epsilon",
	    relativePath("/alpha/beta/epsilon", "/alpha/beta/gamma/delta"));
  EXPECT_EQ("../..", relativePath("alpha/beta", "alpha/beta/delta/gamma"));
  EXPECT_EQ("delta/gamma", relativePath("alpha/beta/delta/gamma",
					"alpha/beta"));
  EXPECT_EQ(".", relativePath("alpha/beta/delta", "alpha/beta/delta"));
  EXPECT_EQ("alpha/beta", relativePath("alpha/beta", ""));
  EXPECT_EQ("../../..", relativePath("", "alpha/beta/delta"));
}

TEST(PathTests, Split) {
  std::vector<std::string> components;

  components = std::vector<std::string>{"foo", "bar", "baz"};
  EXPECT_EQ(components, split("foo/bar/baz"));
  EXPECT_EQ(components, split("foo//bar///baz"));

  components = std::vector<std::string>{"/", "foo", "bar", "baz"};
  EXPECT_EQ(components, split("/foo/bar/baz"));
  EXPECT_EQ(components, split("//foo/bar/baz"));

  components = std::vector<std::string>{"/", "foo", "bar", "baz", ""};
  EXPECT_EQ(components, split("/foo/bar/baz/"));
  EXPECT_EQ(components, split("/foo/bar/baz///"));

  components = std::vector<std::string>{"/"};
  EXPECT_EQ(components, split("/"));
  EXPECT_EQ(components, split("//"));

  components = std::vector<std::string>{};
  EXPECT_EQ(components, split(""));
}

TEST(PathTests, SplitFile) {
  std::string directory, filename;

  std::tie(directory, filename) = splitFile("/foo/bar/baz.txt");
  EXPECT_EQ("/foo/bar", directory);
  EXPECT_EQ("baz.txt", filename);

  std::tie(directory, filename) = splitFile("/foo/bar//baz.txt");
  EXPECT_EQ("/foo/bar", directory);
  EXPECT_EQ("baz.txt", filename);
  
  std::tie(directory, filename) = splitFile("/foo/bar/baz/");
  EXPECT_EQ("/foo/bar/baz", directory);
  EXPECT_EQ("", filename);

  std::tie(directory, filename) = splitFile("/somefile.txt");
  EXPECT_EQ("/", directory);
  EXPECT_EQ("somefile.txt", filename);

  std::tie(directory, filename) = splitFile("somefile.txt");
  EXPECT_EQ("", directory);
  EXPECT_EQ("somefile.txt", filename);

  std::tie(directory, filename) = splitFile("");
  EXPECT_EQ("", directory);
  EXPECT_EQ("", filename);  
}

TEST(PathTests, SplitExtension) {
  std::string base, extension;

  std::tie(base, extension) = splitExtension("/foo/bar/baz.txt");
  EXPECT_EQ("/foo/bar/baz", base);
  EXPECT_EQ(".txt", extension);

  std::tie(base, extension) = splitExtension("/foo/bar.bar/baz");
  EXPECT_EQ("/foo/bar.bar/baz", base);
  EXPECT_EQ("", extension);

  std::tie(base, extension) = splitExtension(".hidden");
  EXPECT_EQ(".hidden", base);
  EXPECT_EQ("", extension);
  
  std::tie(base, extension) = splitExtension("/home/somebody/.hidden");
  EXPECT_EQ("/home/somebody/.hidden", base);
  EXPECT_EQ("", extension);

  std::tie(base, extension) = splitExtension(".");
  EXPECT_EQ(".", base);
  EXPECT_EQ("", extension);

  std::tie(base, extension) = splitExtension("/some/path/.");
  EXPECT_EQ("/some/path/.", base);
  EXPECT_EQ("", extension);

  std::tie(base, extension) = splitExtension("..");
  EXPECT_EQ("..", base);
  EXPECT_EQ("", extension);
  
  std::tie(base, extension) = splitExtension("/some/path/..");
  EXPECT_EQ("/some/path/..", base);
  EXPECT_EQ("", extension);

  std::tie(base, extension) = splitExtension("file.");
  EXPECT_EQ("file", base);
  EXPECT_EQ(".", extension);

  std::tie(base, extension) = splitExtension("/some/path/file.");
  EXPECT_EQ("/some/path/file", base);
  EXPECT_EQ(".", extension);
}

