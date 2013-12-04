#ifndef _SPECREADERIMPL_H_
#define _SPECREADERIMPL_H_

#include <string>
#include <vector>
#include "specreader.h"

namespace apngasm {
namespace specreader {

  // Interface.
  class ISpecReaderImpl
  {
  public:
    // Return animation name.
    virtual const std::string& getName() const = 0;

    // Return loops.
    virtual unsigned int getLoops() const = 0;

    // Return flag of skip first.
    virtual bool getSkipFirst() const = 0;

    // Return frame vector.
    virtual const std::vector<FrameInfo>& getFrameInfos() const = 0;

  };  // interface ISpecReaderImpl

  // Abstract class.
  class AbstractSpecReader : public ISpecReaderImpl
  {
  public:
    // Return animation name.
    const std::string& getName() const { return _name; } ;

    // Return loops.
    unsigned int getLoops() const { return _loops; };

    // Return flag of skip first.
    bool getSkipFirst() const { return _skipFirst; };

    // Return frame information vector.
    const std::vector<FrameInfo>& getFrameInfos() const { return _frameInfos; };

  protected:
    // Initialize AbstractSpecReader object.
    AbstractSpecReader();

    // Fields.
    std::string _name;
    unsigned int _loops;
    bool _skipFirst;
    std::vector<FrameInfo> _frameInfos;

  };  // class AbstractSpecReader

  // for JSON.
  class JsonSpecReader : public AbstractSpecReader
  {
  public:
    // Initialize JsonSpecReader object.
    JsonSpecReader(const std::string& filePath);

  };  // class JsonSpecReader

  // for XML.
  class XmlSpecReader : public AbstractSpecReader
  {
  public:
    // Initialize XmlSpecReader object.
    XmlSpecReader(const std::string& filePath);

  };  // class XmlSpecReader

} // namespace specreader
} // namespace apngasm

#endif  // _SPECREADERIMPL_H_
