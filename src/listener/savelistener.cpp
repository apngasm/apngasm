#include "savelistener.h"
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>

namespace apngasm {
  namespace listener {

    // Called before save.
    // Return true if can save.
    bool SaveListener::onPreSave(const std::string& filePath) const
    {
      return true;
    }

    // Called after save.
    void SaveListener::onPostSave(const std::string& filePath) const
    {
      // nop
    }

    // Called when create output path of png file.
    // Return output path.
    const std::string SaveListener::onCreatePngPath(const std::string& outputDir, int index) const
    {
      std::ostringstream result;
      result  << outputDir
              << ( (outputDir.empty() || boost::algorithm::iends_with(outputDir, "/")) ? "" : "/" )
              << index
              << ".png"
              ;
      return result.str();
    }

  } // namespace listener
} // namespace apngasm
