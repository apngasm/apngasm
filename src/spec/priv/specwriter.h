#ifndef _SPECWRITER_H_
#define _SPECWRITER_H_

#include <string>

namespace apngasm {

  class APNGAsm;

  namespace spec {
    namespace priv {

      // Interface.
      class ISpecWriter
      {
      public:
        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        virtual bool write(const std::string& filePath, const std::string& currentDir="") const = 0;

      };  // class ISpecWriter

      // Abstract class.
      class AbstractSpecWriter : public ISpecWriter
      {
      public:
        // Initialize AbstractSpecWriter object.
        AbstractSpecWriter(const APNGAsm* pApngasm);

      protected:
        const APNGAsm* const _pApngasm;

      };  // class AbstractSpecWriter

      // for JSON.
      class JsonSpecWriter : public AbstractSpecWriter
      {
      public:
        // Initialize JsonSpecWriter object.
        JsonSpecWriter(const APNGAsm* pApngasm);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& currentDir="") const;
        
      };  // class JsonSpecWriter

      // for XML.
      class XmlSpecWriter : public AbstractSpecWriter
      {
      public:
        // Initialize XmlSpecWriter object.
        XmlSpecWriter(const APNGAsm* pApngasm);

        // Write APNGAsm object to spec file.
        // Return true if write succeeded.
        bool write(const std::string& filePath, const std::string& currentDir="") const;
        
      };  // class XmlSpecWriter


    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECWRITER_H_
