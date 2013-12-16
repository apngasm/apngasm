#ifndef _SPECWRITERIMPL_H_
#define _SPECWRITERIMPL_H_

#include <string>

namespace apngasm {

  class APNGAsm;

  namespace spec {
    namespace priv {

      // Interface.
      class ISpecWriterImpl
      {
      public:
        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        virtual bool write(const std::string& filePath, const std::string& currentDir="") const = 0;

      };  // class ISpecWriterImpl

      // Abstract class.
      class AbstractSpecWriterImpl : public ISpecWriterImpl
      {
      public:
        // Initialize AbstractSpecWriterImpl object.
        AbstractSpecWriterImpl(const APNGAsm* pApngasm);

      protected:
        const APNGAsm* const _pApngasm;

      };  // class AbstractSpecWriterImpl

      // for JSON.
      class JsonSpecWriterImpl : public AbstractSpecWriterImpl
      {
      public:
        // Initialize JsonSpecWriterImpl object.
        JsonSpecWriterImpl(const APNGAsm* pApngasm);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& currentDir="") const;
        
      };  // class JsonSpecWriterImpl

      // for XML.
      class XmlSpecWriterImpl : public AbstractSpecWriterImpl
      {
      public:
        // Initialize XmlSpecWriterImpl object.
        XmlSpecWriterImpl(const APNGAsm* pApngasm);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& currentDir="") const;
        
      };  // class XmlSpecWriterImpl


    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECWRITERIMPL_H_
