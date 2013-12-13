#ifndef _SPECPARSER_H_
#define _SPECPARSER_H_

#include <string>

namespace apngasm {

  class APNGAsm;

  namespace spec {

    class SpecParser
    {
    public:
      // Initialize SpecParser object.
      SpecParser(APNGAsm *pApngasm);

      // Read APNGAsm object from spec file.
      // Return true if read succeeded.
      bool read(const std::string& filePath);

      // Write APNGAsm object to json file.
      // Return true if write succeeded.
      bool writeJson(const std::string& filePath, const std::string& currentDir="") const;

      // Write APNGAsm object to xml file.
      // Return true if write succeeded.
      bool writeXml(const std::string& filePath, const std::string& currentDir="") const;

    private:
      APNGAsm* const _pApngasm;

    };  // class SpecParser

  } // namespacce spec
} // namespacce apngasm

#endif  // _SPECPARSER_H_
