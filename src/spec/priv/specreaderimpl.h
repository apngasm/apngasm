#ifndef _SPECREADERIMPL_H_
#define _SPECREADERIMPL_H_

#include <string>
#include <vector>

namespace apngasm {
  namespace spec {
    namespace priv {

      typedef struct {
        unsigned int num;
        unsigned int den;
      } Delay;

      // Frame information.
      typedef struct {
        std::string filePath;
        Delay delay;
      } FrameInfo;

      // Interface.
      class ISpecReaderImpl
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        virtual bool read(const std::string& filePath) = 0;

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
      class AbstractSpecReaderImpl : public ISpecReaderImpl
      {
      public:
        // Initialize AbstractSpecReaderImpl object.
        AbstractSpecReaderImpl();

        // Return animation name.
        const std::string& getName() const;

        // Return loops.
        unsigned int getLoops() const;

        // Return flag of skip first.
        bool getSkipFirst() const;

        // Return frame information vector.
        const std::vector<FrameInfo>& getFrameInfos() const;

      protected:
        // Fields.
        std::string _name;
        unsigned int _loops;
        bool _skipFirst;
        std::vector<FrameInfo> _frameInfos;

      };  // class AbstractSpecReaderImpl


      // for JSON.
      class JsonSpecReaderImpl : public AbstractSpecReaderImpl
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class JsonSpecReaderImpl


      // for XML.
      class XmlSpecReaderImpl : public AbstractSpecReaderImpl
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class XmlSpecReaderImpl

    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECREADERIMPL_H_
