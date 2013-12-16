#include "specwriterimpl.h"
#include "../../apngasm.h"
#include <boost/property_tree/json_parser.hpp>

namespace apngasm {
  namespace spec {
    namespace priv {

        // Initialize AbstractSpecWriterImpl object.
        AbstractSpecWriterImpl::AbstractSpecWriterImpl(const APNGAsm* pApngasm)
          : _pApngasm(pApngasm)
        {
          // nop
        }

        // Initialize JsonSpecWriterImpl object.
        JsonSpecWriterImpl::JsonSpecWriterImpl(const APNGAsm* pApngasm)
          : AbstractSpecWriterImpl(pApngasm)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool JsonSpecWriterImpl::write(const std::string& filePath, const std::string& currentDir) const
        {
          boost::property_tree::ptree root;

          write_json(filePath, root);
          return true;
        }

        // Initialize XmlSpecWriterImpl object.
        XmlSpecWriterImpl::XmlSpecWriterImpl(const APNGAsm* pApngasm)
          : AbstractSpecWriterImpl(pApngasm)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool XmlSpecWriterImpl::write(const std::string& filePath, const std::string& currentDir) const
        {
          return false;
        }


    } // namespace priv
  } // namespace spec
} // namespace apngasm
