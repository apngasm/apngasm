#ifndef _SPECWRITER_H_
#define _SPECWRITER_H_

#include <string>

namespace apngasm {

  class APNGAsm;
  namespace listener { class IAPNGAsmListener; }

  namespace spec {

    class SpecWriter
    {
    public:
      // Initialize SpecWriter object.
      SpecWriter(const APNGAsm *pApngasm, const listener::IAPNGAsmListener* pListener);

      // Write APNGAsm object to json file.
      // Return true if write succeeded.
      bool writeJSON(const std::string& filePath, const std::string& imageDir="") const;

      // Write APNGAsm object to xml file.
      // Return true if write succeeded.
      bool writeXML(const std::string& filePath, const std::string& imageDir="") const;

    private:
      const APNGAsm* const _pApngasm;
      const listener::IAPNGAsmListener* const _pListener;
    };  // class SpecWriter

  } // namespace spec
} // namespace apngasm

#endif  // _SPECWRITER_H_
