#include "specreaderimpl.h"
#include "specreader.h"
#include "../apngframe.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/range/algorithm.hpp>

namespace apngasm {
namespace specreader {
  namespace {
    // Convert string to unsigned int.
    // Return true if convert succeeded.
    bool s2u(const std::string& str, unsigned int* pOut)
    {
      if(pOut == NULL)
        return false;

      try
      {
        *pOut = boost::lexical_cast<unsigned int>(str);
        return true;
      }
      catch(boost::bad_lexical_cast& e)
      {
        return false;
      }
    }

    // Convert string to delay parameter.
    // Return true if convert succeeded.
    bool str2delay(const std::string& str, Delay* pOut)
    {
      if(pOut == NULL)
        return false;

      static const char delimiter = '/';
      const std::string::size_type index = str.find(delimiter, 0);

      // Numerator only.
      if(index == std::string::npos)
      {
        if( !s2u(str, &pOut->num) )
          return false;
        pOut->den = DEFAULT_FRAME_DENOMINATOR;
      }

      // Numerator / Denominator
      else
      {
        const std::string& num = str.substr(0, index);
        const std::string& den = str.substr(index+1, str.length());

        if( !s2u(num, &pOut->num) || !s2u(den, &pOut->den) )
          return false;
      }

      return true;
    }

    // Get file path vector.
    const std::vector<std::string>& getFiles(const std::string& filepath)
    {
      static std::vector<std::string> files;

      boost::filesystem::path absPath( boost::filesystem::absolute(filepath) );

      // Clear temporary vector.
      files.clear();

      // File is unique.
      if( absPath.string().find('*', 0) == std::string::npos )
      {
        // Add extension
        if( !boost::algorithm::iends_with(absPath.string(), ".png") )
          absPath = absPath.string() + ".png";
        
        if( boost::filesystem::exists(absPath) )
          files.push_back(absPath.string());
      }

      // File path has wildcard.
      else
      {
        // Convert filepath.
        static const boost::regex escape("[\\^\\.\\$\\|\\(\\)\\[\\]\\+\\?\\\\]");
        static const boost::regex wildcard("\\*");

        absPath = boost::regex_replace(absPath.string(), escape, "\\\\$0");
        absPath = boost::regex_replace(absPath.string(), wildcard, ".+");

        // Skip if directory is not found.
        if( !boost::filesystem::exists(absPath.parent_path()) )
          return files;

        // Search files.
        const boost::regex filter(absPath.string());
        const boost::filesystem::directory_iterator itEnd;
        for(boost::filesystem::directory_iterator itCur(absPath.parent_path());  itCur != itEnd;  ++itCur)
        {
          // Skip if not a file.
          if( !boost::filesystem::is_regular_file(itCur->status()) )
            continue;

          // Skip if no match.
          const std::string& curFilePath = itCur->path().string();
          if( !boost::regex_match(curFilePath, filter) )
            continue;

          // Add filepath if extension is png.
          if( boost::algorithm::iends_with(curFilePath, ".png") )
            files.push_back(curFilePath);
        }

        // Sort vector.
        boost::sort(files);
      }

      return files;
    }
  } // unnamed namespace

  // Initialize SpecReader object.
  SpecReader::SpecReader(const std::string& filePath)
  {
    // File is JSON.
    if( boost::algorithm::iends_with(filePath, ".json") )
      _pImpl = new JsonSpecReader(filePath);
    // File is XML.
    else
      _pImpl = new XmlSpecReader(filePath);
  }

  // Finalize SpecReader object.
  SpecReader::~SpecReader()
  {
    if(_pImpl)
    {
      delete _pImpl;
      _pImpl = NULL;
    }
  }

  // Return animation name.
  const std::string& SpecReader::getName() const
  {
    return _pImpl->getName();
  }

  // Return loops.
  unsigned int SpecReader::getLoops() const
  {
    return _pImpl->getLoops();
  }

  // Return flag of skip first.
  bool SpecReader::getSkipFirst() const
  {
    return _pImpl->getSkipFirst();
  }

  // Return frame information vector.
  const std::vector<FrameInfo>& SpecReader::getFrameInfos() const
  {
    return _pImpl->getFrameInfos();
  }

  // Initialize AbstractSpecReader object.
  AbstractSpecReader::AbstractSpecReader()
    : _name("")
    , _loops(0)
    , _skipFirst(false)
    , _frameInfos()
  {
    // nop
  }
  
  // Initialize JsonSpecReader object.
  JsonSpecReader::JsonSpecReader(const std::string& filePath)
  {
    // Read JSON file.
    boost::property_tree::ptree root;
    boost::property_tree::read_json(filePath, root);

    // Set current directory.
    const boost::filesystem::path oldPath = boost::filesystem::current_path();
    boost::filesystem::current_path( boost::filesystem::path(filePath).parent_path() );

    // Read fields.
    // name
    if( boost::optional<std::string> value = root.get_optional<std::string>("name") )
    {
      _name = value.get();
    }

    // loops
    if( boost::optional<unsigned int> value = root.get_optional<unsigned int>("loops") )
    {
      _loops = value.get();
    }

    // skip_first
    if( boost::optional<bool> value = root.get_optional<bool>("skip_first") )
    {
      _skipFirst = value.get();
    }

    // delay
    Delay defaultDelay = { DEFAULT_FRAME_NUMERATOR, DEFAULT_FRAME_DENOMINATOR };
    if( boost::optional<std::string> value = root.get_optional<std::string>("default_delay") )
    {
      if( !str2delay(value.get(), &defaultDelay) )
      {
        defaultDelay.num = DEFAULT_FRAME_NUMERATOR;
        defaultDelay.den = DEFAULT_FRAME_DENOMINATOR;
      }
    }

    std::vector<Delay> delays;
    BOOST_FOREACH(const boost::property_tree::ptree::value_type& child, root.get_child("delays"))
    {
      Delay delay;
      if( !str2delay(child.second.data(), &delay) )
        delay = defaultDelay;
      delays.push_back(delay);
    }

    // frames
    int delayIndex = 0;
    BOOST_FOREACH(const boost::property_tree::ptree::value_type& child, root.get_child("frames"))
    {
      std::string file;
      Delay delay;
      const boost::property_tree::ptree& frame = child.second;

      // filepath only.
      if(frame.empty())
      {
        file = frame.data();
        if(delayIndex < delays.size())
          delay = delays[delayIndex];
        else
          delay = defaultDelay;
      }

      // filepath and delay
      else
      {
        const boost::property_tree::ptree::value_type& value = frame.front();
        file = value.first;
        if( !str2delay(value.second.data(), &delay) )
          delay = defaultDelay;
      }

      // Add frame informations.
      const std::vector<std::string>& files = getFiles(file);
      const int count = files.size();
      for(int i = 0;  i < count;  ++i)
      {
        const FrameInfo frameInfo = { files[i], delay };
        _frameInfos.push_back(frameInfo);
      }

      ++delayIndex;
    }

    // Reset current directory.
    boost::filesystem::current_path(oldPath);
  }
  
  // Initialize XmlSpecReader object.
  XmlSpecReader::XmlSpecReader(const std::string& filePath)
  {
    // Read XML file.
    boost::property_tree::ptree root;
    boost::property_tree::read_xml(filePath, root);

    // Set current directory.
    const boost::filesystem::path oldPath = boost::filesystem::current_path();
    boost::filesystem::current_path( boost::filesystem::path(filePath).parent_path() );

    // Read fields.
    // name
    if( boost::optional<std::string> value = root.get_optional<std::string>("animation.<xmlattr>.name") )
    {
      _name = value.get();
    }

    // loops
    if( boost::optional<unsigned int> value = root.get_optional<unsigned int>("animation.<xmlattr>.loops") )
    {
      _loops = value.get();
    }

    // skip_first
    if( boost::optional<bool> value = root.get_optional<bool>("animation.<xmlattr>.skip_first") )
    {
      _skipFirst = value.get();
    }

    // delay
    Delay defaultDelay = { DEFAULT_FRAME_NUMERATOR, DEFAULT_FRAME_DENOMINATOR };
    if( boost::optional<std::string> value = root.get_optional<std::string>("animation.<xmlattr>.default_delay") )
    {
      if( !str2delay(value.get(), &defaultDelay) )
      {
        defaultDelay.num = DEFAULT_FRAME_NUMERATOR;
        defaultDelay.den = DEFAULT_FRAME_DENOMINATOR;
      }
    }

    // frames
    BOOST_FOREACH(const boost::property_tree::ptree::value_type& child, root.get_child("animation"))
    {
      std::string file;
      Delay delay;
      const boost::property_tree::ptree& frame = child.second;

      // filepath
      if( boost::optional<std::string> value = frame.get_optional<std::string>("<xmlattr>.src") )
      {
        file = value.get();
      }
      if(file.empty())
        continue;

      // delay
      if( boost::optional<std::string> value = frame.get_optional<std::string>("<xmlattr>.delay") )
      {
        if( !str2delay(value.get(), &delay) )
          delay = defaultDelay;
      }
      else
      {
        delay = defaultDelay;
      }

      // Add frame informations.
      const std::vector<std::string>& files = getFiles(file);
      const int count = files.size();
      for(int i = 0;  i < count;  ++i)
      {
        const FrameInfo frameInfo = { files[i], delay };
        _frameInfos.push_back(frameInfo);
      }
    }

    // Reset current directory.
    boost::filesystem::current_path(oldPath);
  }

} // namespace specreader
} // namespace apngasm
