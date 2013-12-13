#ifndef _SPECREADER_H_
#define _SPECREADER_H_

#include <string>

namespace apngasm {

  class APNGAsm;

  namespace spec {

    class SpecReader
    {
    public:
      // Initialize SpecReader object.
      SpecReader(APNGAsm *pApngasm);

      // Read APNGAsm object from spec file.
      // Return true if read succeeded.
      bool read(const std::string& filePath);

    private:
      APNGAsm* const _pApngasm;

    };  // class SpecReader

  } // namespacce spec
} // namespacce apngasm

#endif  // _SPECREADER_H_
