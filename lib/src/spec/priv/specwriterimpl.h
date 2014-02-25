#ifndef _SPECWRITERIMPL_H_
#define _SPECWRITERIMPL_H_

#include <string>

namespace apngasm {

  class APNGAsm;
  namespace listener { class IAPNGAsmListener; }

  namespace spec {
    namespace priv {

      // Interface.
      class ISpecWriterImpl
      {
      public:
        // Destructor.
        virtual ~ISpecWriterImpl(){ /* nop */ }

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        virtual bool write(const std::string& filePath, const std::string& currentDir="") const = 0;

      };  // class ISpecWriterImpl

      // Abstract class.
      class AbstractSpecWriterImpl : public ISpecWriterImpl
      {
      public:
        // Initialize AbstractSpecWriterImpl object.
        AbstractSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener);

      protected:
        const APNGAsm* const _pApngasm;
        const listener::IAPNGAsmListener* const _pListener;

      };  // class AbstractSpecWriterImpl

      // for JSON.
      class JsonSpecWriterImpl : public AbstractSpecWriterImpl
      {
      public:
        // Initialize JsonSpecWriterImpl object.
        JsonSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& imagePathPrefix="") const;
        
      };  // class JsonSpecWriterImpl

      // for XML.
      class XmlSpecWriterImpl : public AbstractSpecWriterImpl
      {
      public:
        // Initialize XmlSpecWriterImpl object.
        XmlSpecWriterImpl(const APNGAsm* pApngasm, const listener::IAPNGAsmListener* pListener);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& imagePathPrefix="") const;
        
      };  // class XmlSpecWriterImpl


    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECWRITERIMPL_H_
