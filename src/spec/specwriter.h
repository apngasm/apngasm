#ifndef _SPECWRITER_H_
#define _SPECWRITER_H_

#include <string>

namespace apngasm {

  class APNGAsm;

  namespace spec {

    class SpecWriter
    {
    public:
      // Initialize SpecWriter object.
      SpecWriter(const APNGAsm *pApngasm);

      // Write APNGAsm object to json file.
      // Return true if write succeeded.
      bool writeJson(const std::string& filePath, const std::string& imageDir="") const;

      // Write APNGAsm object to xml file.
      // Return true if write succeeded.
      bool writeXml(const std::string& filePath, const std::string& imageDir="") const;

    private:
      const APNGAsm* const _pApngasm;
    };  // class SpecWriter

  } // namespace spec
} // namespace apngasm

#endif  // _SPECWRITER_H_
