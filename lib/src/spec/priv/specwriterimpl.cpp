#include "specwriterimpl.h"
#include "../../apngasm.h"
#include "../../listener/apngasmlistener.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace apngasm {
  namespace spec {
    namespace priv {

        // Initialize AbstractSpecWriterImpl object.
        AbstractSpecWriterImpl::AbstractSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener)
          : _pApngasm(pApngasm)
          , _pListener(pListener)
        {
          // nop
        }

        // Initialize JSONSpecWriterImpl object.
        JSONSpecWriterImpl::JSONSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener)
          : AbstractSpecWriterImpl(pApngasm, pListener)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool JSONSpecWriterImpl::write(const std::string& filePath, const std::string& imagePathPrefix) const
        {
          boost::property_tree::ptree root;

          // Write apngasm fields.
          // root.put("name", _pApngasm->name());
          root.put("loops", _pApngasm->getLoops());
          root.put("skip_first", _pApngasm->isSkipFirst());

          {
            boost::property_tree::ptree child;
            std::vector<APNGFrame>& frames = const_cast<std::vector<APNGFrame>&>(_pApngasm->getFrames()); // gununu...
            const int count = frames.size();
            for(int i = 0;  i < count;  ++i)
            {
              const std::string file = _pListener->onCreatePngPath(imagePathPrefix, i);
              
              std::ostringstream delay;
              delay << frames[i].delayNum() << "/" << frames[i].delayDen();

              boost::property_tree::ptree frame;
              frame.push_back(std::make_pair( file, boost::property_tree::ptree(delay.str()) ));

              child.push_back(std::make_pair("", frame));
            }
            root.add_child("frames", child);
          }

          write_json(filePath, root);
          return true;
        }

        // Initialize XMLSpecWriterImpl object.
        XMLSpecWriterImpl::XMLSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener)
          : AbstractSpecWriterImpl(pApngasm, pListener)
        {
          // nop
        }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool XMLSpecWriterImpl::write(const std::string& filePath, const std::string& imagePathPrefix) const
        {
          boost::property_tree::ptree root;

          // Write apngasm fields.
          // root.put("animation.<xmlattr>.name", _pApngasm->name());
          root.put("animation.<xmlattr>.loops", _pApngasm->getLoops());
          root.put("animation.<xmlattr>.skip_first", _pApngasm->isSkipFirst());

          {
            boost::property_tree::ptree child;
            std::vector<APNGFrame>& frames = const_cast<std::vector<APNGFrame>&>(_pApngasm->getFrames()); // gununu...
            const int count = frames.size();
            for(int i = 0;  i < count;  ++i)
            {
              const std::string file = _pListener->onCreatePngPath(imagePathPrefix, i);
              
              std::ostringstream delay;
              delay << frames[i].delayNum() << "/" << frames[i].delayDen();

              boost::property_tree::ptree& frame = root.add("animation.frame", "");
              frame.put("<xmlattr>.src", file);
              frame.put("<xmlattr>.delay", delay.str());
            }
          }

          write_xml(filePath, root);
          return true;
        }


    } // namespace priv
  } // namespace spec
} // namespace apngasm
