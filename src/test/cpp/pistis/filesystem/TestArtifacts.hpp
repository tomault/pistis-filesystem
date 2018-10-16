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
      std::string getResourcePath(const std::string& resourceFile);
      
      /** @brief Returns a directory where unit tests can write temporary
       *         files.
       *
       *  Equal to PISTIS_FILESYSTEM_TEST_SCRATCH_DIR if that environment
       *  variable is set.  Otherwise, the scratch directory is equal to
       *  "${TEST_EXECUTABLE_DIR}/../tmp," where TEST_EXECUTABLE_DIR is
       *  the directory that contains the unit test executable file.
       */
      std::string getScratchDir();
      
    }
  }
}
#endif
