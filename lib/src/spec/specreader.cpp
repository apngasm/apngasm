#include "specreader.h"
#include "priv/specreaderimpl.h"
#include "../apngasm.h"
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace apngasm {
  namespace spec {

    namespace {
      // Return true if file is json.
      bool isJSON(const std::string& filePath)
      {
        return boost::algorithm::iends_with(filePath, ".json");
      }

      // Return true if file is xml.
      bool isXML(const std::string& filePath)
      {
        return boost::algorithm::iends_with(filePath, ".xml");
      }
    } // unnamed namespace

    // Initialize SpecReader object.
    SpecReader::SpecReader(APNGAsm *pApngasm)
      : _pApngasm(pApngasm)
    {
      // nop
    }

    // Read APNGAsm object from spec file.
    // Return true if read succeeded.
    bool SpecReader::read(const std::string& filePath)
    {
      if( !_pApngasm )
        return false;

      boost::scoped_ptr<priv::ISpecReaderImpl> pImpl;

      // json file.
      if( isJSON(filePath) )
      {
        pImpl.reset(new priv::JSONSpecReaderImpl());
      }
      // xml file.
      else if( isXML(filePath) )
      {
        pImpl.reset(new priv::XMLSpecReaderImpl());
      }
      // unknown file.
      else
        return false;

      // Read frame information from spec file.
      if( !pImpl->read(filePath) )
        return false;

      // Create frames from spec file.
      const std::vector<priv::FrameInfo>& frameInfos = pImpl->getFrameInfos();
      const int count = frameInfos.size();
      for(int i = 0;  i < count;  ++i)
      {
        const priv::FrameInfo& current = frameInfos[i];
        _pApngasm->addFrame(current.filePath, current.delay.num, current.delay.den);
      }

      // Set parameter from spec file.
      _pApngasm->setLoops(pImpl->getLoops());
      _pApngasm->setSkipFirst(pImpl->isSkipFirst());

      return true;
    }

  } // namespace spec
} // namespace apngasm
