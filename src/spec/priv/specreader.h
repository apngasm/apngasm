#ifndef _SPECREADER_H_
#define _SPECREADER_H_

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
      class ISpecReader
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

      };  // interface ISpecReader


      // Abstract class.
      class AbstractSpecReader : public ISpecReader
      {
      public:
        // Initialize AbstractSpecReader object.
        AbstractSpecReader();

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

      };  // class AbstractSpecReader


      // for JSON.
      class JsonSpecReader : public AbstractSpecReader
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class JsonSpecReader


      // for XML.
      class XmlSpecReader : public AbstractSpecReader
      {
      public:
        // Read parameter from spec file.
        // Return true if read succeeded.
        bool read(const std::string& filePath);

      };  // class XmlSpecReader

    } // namespace priv
  } // namespace spec
} // namespace apngasm

#endif  // _SPECREADER_H_