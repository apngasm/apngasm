#include "specreader.h"
#include "priv/specreaderimpl.h"
#include <boost/algorithm/string/predicate.hpp>

namespace apngasm {
  namespace spec {

    // Initialize SpecReader object.
    SpecReader::SpecReader(const std::string& filePath)
    {
      // File is JSON.
      if( boost::algorithm::iends_with(filePath, ".json") )
        _pImpl = new priv::JsonSpecReader(filePath);
      // File is XML.
      else
        _pImpl = new priv::XmlSpecReader(filePath);
    }

    // Finalize SpecReader object.
    SpecReader::~SpecReader()
    {
      if(_pImpl)
      {
        delete _pImpl;
        _pImpl = NULL;
      }
    }

    // Return animation name.
    const std::string& SpecReader::getName() const
    {
      return _pImpl->getName();
    }

    // Return loops.
    unsigned int SpecReader::getLoops() const
    {
      return _pImpl->getLoops();
    }

    // Return flag of skip first.
    bool SpecReader::getSkipFirst() const
    {
      return _pImpl->getSkipFirst();
    }

    // Return frame information vector.
    const std::vector<FrameInfo>& SpecReader::getFrameInfos() const
    {
      return _pImpl->getFrameInfos();
    }

  } // namespace apngasm
} //namespace spec
