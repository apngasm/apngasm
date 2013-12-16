#include "specwriter.h"
#include "priv/specwriterimpl.h"
#include "../apngasm.h"

namespace apngasm {
  namespace spec {

    // Initialize SpecWriter object.
    SpecWriter::SpecWriter(const APNGAsm *pApngasm)
      : _pApngasm(pApngasm)
    {
      // nop
    }

    // Write APNGAsm object to json file.
    // Return true if write succeeded.
    bool SpecWriter::writeJson(const std::string& filePath, const std::string& currentDir) const
    {
      if( !_pApngasm )
        return false;

      priv::JsonSpecWriterImpl impl(_pApngasm);
      return impl.write(filePath, currentDir);
    }

    // Write APNGAsm object to xml file.
    // Return true if write succeeded.
    bool SpecWriter::writeXml(const std::string& filePath, const std::string& currentDir) const
    {
      return false;
    }

  } // namespace spec
} // namespace apngasm
