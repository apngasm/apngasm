#ifndef _APNGASMLISTENER_H_
#define _APNGASMLISTENER_H_

#include <string>

namespace apngasm {
  namespace listener {

    class IAPNGAsmListener
    {
    public:
      // Called before save.
      // Return true if can save.
      virtual bool onPreSave(const std::string& filePath) const = 0;

      // Called after save.
      virtual void onPostSave(const std::string& filePath) const = 0;

      // Called when create output path of png file.
      // Return output path.
      virtual const std::string onCreatePngPath(const std::string& outputDir, int index) const = 0;

    };  // class IAPNGAsmListener

    class APNGAsmListener : public IAPNGAsmListener
    {
    public:
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
