#include "specwriterimpl.h"
#include "../../apngasm.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

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
        bool JsonSpecWriterImpl::write(const std::string& filePath, const std::string& imagePathPrefix) const
        {
          boost::property_tree::ptree root;

          // Write apngasm fields.
          // root.put("name", _pApngasm->name());
          // root.put("loops", _pApngasm->loops());
          // root.put("skip_first", _pApngasm->skipFirst());

          {
            boost::property_tree::ptree child;
            std::vector<APNGFrame>& frames = const_cast<std::vector<APNGFrame>&>(_pApngasm->getFrames()); // gununu...
            const int count = frames.size();
            for(int i = 0;  i < count;  ++i)
            {
              std::ostringstream file;
              std::ostringstream delay;
              file << imagePathPrefix << i << ".png";
              delay << frames[i].delayNum() << "/" << frames[i].delayDen();

              boost::property_tree::ptree frame;
              frame.push_back(std::make_pair(file.str(), delay.str()));

              child.push_back(std::make_pair("", frame));
            }
            root.add_child("frames", child);
          }

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
        bool XmlSpecWriterImpl::write(const std::string& filePath, const std::string& imagePathPrefix) const
        {
          boost::property_tree::ptree root;

          // Write apngasm fields.
          // root.put("animation.<xmlattr>.name", _pApngasm->name());
          // root.put("animation.<xmlattr>.loops", _pApngasm->loops());
          // root.put("animation.<xmlattr>.skip_first", _pApngasm->skipFirst());

          {
            boost::property_tree::ptree child;
            std::vector<APNGFrame>& frames = const_cast<std::vector<APNGFrame>&>(_pApngasm->getFrames()); // gununu...
            const int count = frames.size();
            for(int i = 0;  i < count;  ++i)
            {
              std::ostringstream file;
              std::ostringstream delay;
              file << imagePathPrefix << i << ".png";
              delay << frames[i].delayNum() << "/" << frames[i].delayDen();

              boost::property_tree::ptree& frame = root.add("animation.frame", "");
              frame.put("<xmlattr>.src", file.str());
              frame.put("<xmlattr>.delay", delay.str());
            }
          }

          write_xml(filePath, root);
          return true;
        }


    } // namespace priv
  } // namespace spec
} // namespace apngasm
