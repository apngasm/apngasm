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
        // Destructor.
        virtual ~ISpecReaderImpl(){ /* nop */ }

        // Read parameter from spec file.
        // Return true if read succeeded.
        virtual bool read(const std::string& filePath) = 0;

        // Return animation name.
        virtual const std::string& getName() const = 0;

        // Return loops.
        virtual unsigned int getLoops() const = 0;

        // Return flag of skip first frame.
        virtual bool isSkipFirst() const = 0;

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

        // Return flag of skip first frame.
        bool isSkipFirst() const;

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
      class JSONSpecReaderImpl : public AbstractSpecReaderImpl
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class JSONSpecReaderImpl


      // for XML.
      class XMLSpecReaderImpl : public AbstractSpecReaderImpl
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class XMLSpecReaderImpl

    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECREADERIMPL_H_
