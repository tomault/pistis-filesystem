#ifndef __PISTIS__FILESYSTEM__TESTARTIFACTS_HPP__
#define __PISTIS__FILESYSTEM__TESTARTIFACTS_HPP__

/** @file TestArtifacts.hpp
 *
 *  Some artifacts needed by the unit tests.
 */

#include <string>

namespace pistis {
  namespace filesystem {
    namespace testing {

      /** @brief Returns the directory containing the test executable */
      std::string getExecutableDir();

      /** @brief Returns the directory containing the test resources
       *
       *  If the PISTIS_FILESYSTEM_TEST_RESOURCE_DIR environment variable is
       *  set, then the resource directory is the value of that variable.
       *  Otherwise, the resource directory is equal to 
       *  "${TEST_EXECUTABLE_DIR}/../resources," where TEST_EXECUTABLE_DIR
       *  is the directory that contains the unit test executable file.
       */
      std::string getResourceDir();

      /** @brief Expands the resource filename into a full path to the
       *         resource.
       *
       *  If the resource filename is an absolute path, it is left as-is.
       *  Otherwise, this function prepends the resource directory to
       *  the filename to create a fully-qualified path.
       */
      std::string getResourcePath(const std::string& filename);
      
      /** @brief Returns a directory where unit tests can write temporary
       *         files.
       *
       *  Equal to PISTIS_FILESYSTEM_TEST_SCRATCH_DIR if that environment
       *  variable is set.  Otherwise, the scratch directory is equal to
       *  "${TEST_EXECUTABLE_DIR}/../tmp," where TEST_EXECUTABLE_DIR is
       *  the directory that contains the unit test executable file.
       */
      std::string getScratchDir();

      /** brief Expands the given filename to a fully-qualified path
       *        inside the scratch directory.
       *
       *  If the file name is an absolute path, it is returned as-is.
       *  Otherwise, it is joined with the scratch directory to create
       *  a fully-qualified path name to the file.
       *
       *  @param filename  The name of the scatch file
       *  @returns         A fully-qualified path to the named file
       */
      std::string getScratchFile(const std::string& filename);

      /** @brief Remove the named file.
       *
       *  If the file is not an absolute path, it is joined with the
       *  scratch directory to form a fully-qualified path to a file
       *  located relative to that directory.  If the file does not exist
       *  or cannot be removed, removeFile() gives up and does not
       *  report an error or throw an exception.
       *
       *  @param filename  The file to remove
       */
      void removeFile(const std::string& filename);
    }
  }
}
#endif
