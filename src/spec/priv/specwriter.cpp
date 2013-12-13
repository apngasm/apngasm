#include "specwriter.h"
#include "../../apngasm.h"
#include <boost/property_tree/json_parser.hpp>

namespace apngasm {
  namespace spec {
    namespace priv {

        // Initialize AbstractSpecWriter object.
        AbstractSpecWriter::AbstractSpecWriter(const APNGAsm* pApngasm)
          : _pApngasm(pApngasm)
        {
          // nop
        }

        // Initialize JsonSpecWriter object.
        JsonSpecWriter::JsonSpecWriter(const APNGAsm* pApngasm)
          : AbstractSpecWriter(pApngasm)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool JsonSpecWriter::write(const std::string& filePath, const std::string& currentDir) const
        {
          boost::property_tree::ptree root;

          write_json(filePath, root);
          return true;
        }

        // Initialize XmlSpecWriter object.
        XmlSpecWriter::XmlSpecWriter(const APNGAsm* pApngasm)
          : AbstractSpecWriter(pApngasm)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool XmlSpecWriter::write(const std::string& filePath, const std::string& currentDir) const
        {
          return false;
        }


    } // namespace priv
  } // namespace spec
} // namespace apngasm
