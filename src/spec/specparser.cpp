#include "specparser.h"
#include "priv/specreader.h"
#include "../apngasm.h"
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace apngasm {
  namespace spec {

    namespace {
      // Return true if file is json.
      bool isJson(const std::string& filePath)
      {
        return boost::algorithm::iends_with(filePath, ".json");
      }

      // Return true if file is xml.
      bool isXml(const std::string& filePath)
      {
        return boost::algorithm::iends_with(filePath, ".xml");
      }
    } // unnamed namespace

    // Initialize SpecParser object.
    SpecParser::SpecParser(APNGAsm *pApngasm)
      : _pApngasm(pApngasm)
    {
      // nop
    }

    // Read APNGAsm object from spec file.
    // Return true if read succeeded.
    bool SpecParser::read(const std::string& filePath)
    {
      if( !_pApngasm )
        return false;

      boost::scoped_ptr<priv::ISpecReader> pReader;

      // json file.
      if( isJson(filePath) )
      {
        pReader.reset(new priv::JsonSpecReader());
      }
      // xml file.
      else if( isXml(filePath) )
      {
        pReader.reset(new priv::XmlSpecReader());
      }
      // unknown file.
      else
        return false;

      // Read frame information from spec file.
      if( !pReader->read(filePath) )
        return false;

      // Create frames from spec file.
      const std::vector<priv::FrameInfo>& frameInfos = pReader->getFrameInfos();
      const int count = frameInfos.size();
      for(int i = 0;  i < count;  ++i)
      {
        const priv::FrameInfo& current = frameInfos[i];
        _pApngasm->addFrame(current.filePath, current.delay.num, current.delay.den);
        std::cout << current.filePath << " => Delay=(" << current.delay.num << "/" << current.delay.den << ") sec" << std::endl;
      }

      return true;
    }

  } // namespace apngasm
} //namespace spec
