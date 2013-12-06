#ifndef _SPECREADER_H_
#define _SPECREADER_H_

#include <string>
#include <vector>

namespace apngasm {
namespace specreader {

  // Delay parameter.
  typedef struct {
    unsigned int num;
    unsigned int den;
  } Delay;

  // Frame information.
  typedef struct {
    std::string filePath;
    Delay delay;
  } FrameInfo;

  class ISpecReaderImpl;

  class SpecReader
  {
  public:
    // Initialize SpecReader object.
    SpecReader(const std::string& filePath);

    // Finalize SpecReader object.
    ~SpecReader();

    // Return animation name.
    const std::string& getName() const;

    // Return loops.
    unsigned int getLoops() const;

    // Return flag of skip first.
    bool getSkipFirst() const;

    // Return frame information vector.
    const std::vector<FrameInfo>& getFrameInfos() const;

  private:
    // Implements object pointer.
    const ISpecReaderImpl* _pImpl;

  };  // class SpecReader

} // namespacce specreader
} // namespacce apngasm

#endif  // _SPECREADER_H_
