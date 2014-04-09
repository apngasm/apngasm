#include "specwriter.h"
#include "priv/specwriterimpl.h"
#include "../apngasm.h"
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace apngasm {
  namespace spec {

    namespace {
      const char separator = boost::filesystem::path::preferred_separator;

      const boost::filesystem::path createAbsolutePath(const std::string& path)
      {
        const boost::filesystem::path oldPath = boost::filesystem::current_path();
        boost::filesystem::path result = path;
        boost::filesystem::current_path(result.parent_path());
        result = boost::filesystem::current_path();
        boost::filesystem::current_path(oldPath);
        return result;
      }
      const std::string createRelativeDir(const std::string& from, const std::string& to)
      {
        boost::filesystem::path fromPath = createAbsolutePath(from);
        boost::filesystem::path toPath = createAbsolutePath(to);
        const std::string separatorStr = std::string(1, separator);

        // Convert path to native.
        fromPath.make_preferred();
        toPath.make_preferred();
        if( !boost::algorithm::iends_with(fromPath.string(), separatorStr) )
          fromPath /= separatorStr;
        if( !boost::algorithm::iends_with(toPath.string(), separatorStr) )
          toPath /= separatorStr;

        // Other drive.
        if(fromPath.root_name() != toPath.root_name())
          return fromPath.string();

        // Same drive.
        std::string fromDir = fromPath.string();
        std::string toDir = toPath.string();

        {
          const int count = std::min(fromDir.length(), toDir.length());
          int find = -1;
          for(int i = 0;  i < count;  ++i)
          {
            const char fromChar = fromDir.at(i);
            const char toChar = toDir.at(i);

            if(fromChar == toChar)
            {
              if( fromChar == separator )
                find = i;
            }
            else
              break;
          }
          if(find != -1)
          {
            fromDir = fromDir.substr(find + 1);
            toDir = toDir.substr(find + 1);
          }
        }

        std::string result = "";
        if(!fromDir.empty())
        {
          const int count = fromDir.length();
          const std::string parentDir = ".." + separatorStr;
          bool beforeIsSeparator = true;
          for(int i = 0;  i < count;  ++i)
          {
            const char currentChar = fromDir.at(i);
            if( currentChar == separator )
            {
              if( !beforeIsSeparator )
              {
                result += parentDir;
                beforeIsSeparator = true;
              }
            }
            else
            {
              beforeIsSeparator = false;
            }
          }
        }
        result += toDir;

        return result;
      }
    } // unnnamed namespace

    // Initialize SpecWriter object.
    SpecWriter::SpecWriter(const APNGAsm *pApngasm, const listener::IAPNGAsmListener* pListener)
      : _pApngasm(pApngasm)
      , _pListener(pListener)
    {
      // nop
    }

    // Write APNGAsm object to json file.
    // Return true if write succeeded.
    bool SpecWriter::writeJson(const std::string& filePath, const std::string& imageDir) const
    {
      if( !_pApngasm )
        return false;

      priv::JsonSpecWriterImpl impl(_pApngasm, _pListener);
      return impl.write(filePath, createRelativeDir(filePath, imageDir + separator));
    }

    // Write APNGAsm object to xml file.
    // Return true if write succeeded.
    bool SpecWriter::writeXml(const std::string& filePath, const std::string& imageDir) const
    {
      if( !_pApngasm )
        return false;

      priv::XmlSpecWriterImpl impl(_pApngasm, _pListener);
      return impl.write(filePath, createRelativeDir(filePath, imageDir + separator));
    }

  } // namespace spec
} // namespace apngasm
