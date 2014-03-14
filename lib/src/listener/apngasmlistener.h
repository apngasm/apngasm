#ifndef _APNGASMLISTENER_H_
#define _APNGASMLISTENER_H_

#include <string>
#include "../apngasm-conf.h"
#include "../apngframe.h"

namespace apngasm {
  namespace listener {

    class APNGASM_DECLSPEC IAPNGAsmListener
    {
    public:
      // Destructor.
      virtual ~IAPNGAsmListener(){ /* nop */ }

      // Called before add frame.
      // Return true if can add.
      virtual bool onPreAddFrame(const std::string& filePath, unsigned int delayNum, unsigned int delayDen) const = 0;
      
      // Called before add frame.
      // Return true if can add.
      virtual bool onPreAddFrame(const APNGFrame& frame) const = 0;

      // Called after add frame.
      virtual void onPostAddFrame(const std::string& filePath, unsigned int delayNum, unsigned int delayDen) const = 0;
      
      // Called after add frame.
      virtual void onPostAddFrame(const APNGFrame& frame) const = 0;

      // Called before save.
      // Return true if can save.
      virtual bool onPreSave(const std::string& filePath) const = 0;

      // Called after save.
      virtual void onPostSave(const std::string& filePath) const = 0;

      // Called when create output path of png file.
      // Return output path.
      virtual const std::string onCreatePngPath(const std::string& outputDir, int index) const = 0;

    };  // class IAPNGAsmListener

    class APNGASM_DECLSPEC APNGAsmListener : public IAPNGAsmListener
    {
    public:
      // Called before add frame.
      // Return true if can add.
      bool onPreAddFrame(const std::string& filePath, unsigned int delayNum, unsigned int delayDen) const;

      // Called before add frame.
      // Return true if can add.
      bool onPreAddFrame(const APNGFrame& frame) const;

      // Called after add frame.
      void onPostAddFrame(const std::string& filePath, unsigned int delayNum, unsigned int delayDen) const;

      // Called after add frame.
      void onPostAddFrame(const APNGFrame& frame) const;

      // Called before save.
      // Return true if can save.
      bool onPreSave(const std::string& filePath) const;

      // Called after save.
      void onPostSave(const std::string& filePath) const;

      // Called when create output path of png file.
      // Return output path.
      const std::string onCreatePngPath(const std::string& outputDir, int index) const;

    };  // class APNGAsmListener

  } // namespace listener
} // namespace apngasm

#endif  // _APNGASMLISTENER_H_
