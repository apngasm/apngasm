#include "specreaderimpl.h"
#include "../../apngframe.h"  // DEFAULT_FRAME_NUMERATOR, DEFAULT_FRAME_DENOMINATOR
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>

namespace apngasm {
  namespace spec {
    namespace priv {
    
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
      } // unnamed namespace

      // Initialize AbstractSpecReaderImpl object.
      AbstractSpecReaderImpl::AbstractSpecReaderImpl()
        : _name("")
        , _loops(0)
        , _skipFirst(false)
        , _frameInfos()
      {
        // nop
      }

      // Return animation name.
      const std::string& AbstractSpecReaderImpl::getName() const
      {
        return _name;
      }

      // Return loops.
      unsigned int AbstractSpecReaderImpl::getLoops() const
      {
        return _loops;
      }

      // Return flag of skip first frame.
      bool AbstractSpecReaderImpl::isSkipFirst() const
      {
        return _skipFirst;
      }

      // Return frame information vector.
      const std::vector<FrameInfo>& AbstractSpecReaderImpl::getFrameInfos() const
      {
        return _frameInfos;
      }
      
      // Read parameter from spec file.
      // Return true if read succeeded.
      bool JSONSpecReaderImpl::read(const std::string& filePath)
      {
        // Read JSON file.
        boost::property_tree::ptree root;
        boost::property_tree::read_json(filePath, root);

        // Set current directory.
        const boost::filesystem::path oldPath = boost::filesystem::current_path();
        const boost::filesystem::path currentPath = boost::filesystem::path(filePath);
        if(currentPath.has_parent_path())
          boost::filesystem::current_path(currentPath.parent_path());

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
        if( boost::optional<boost::property_tree::ptree&> child = root.get_child_optional("delays") )
        {
          BOOST_FOREACH(const boost::property_tree::ptree::value_type& current, child.get())
          {
            Delay delay;
            if( !str2delay(current.second.data(), &delay) )
              delay = defaultDelay;
            delays.push_back(delay);
          }
        }

        // frames
        if( boost::optional<boost::property_tree::ptree&> child = root.get_child_optional("frames") )
        {
          int delayIndex = 0;
          BOOST_FOREACH(const boost::property_tree::ptree::value_type& current, child.get())
          {
            std::string file;
            Delay delay;
            const boost::property_tree::ptree& frame = current.second;

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
            const FrameInfo frameInfo = { boost::filesystem::absolute(file).string(), delay };
            _frameInfos.push_back(frameInfo);

            ++delayIndex;
          }
        }

        // Reset current directory.
        boost::filesystem::current_path(oldPath);

        return true;
      }
      
      // Read parameter from spec file.
      // Return true if read succeeded.
      bool XMLSpecReaderImpl::read(const std::string& filePath)
      {
        // Read XML file.
        boost::property_tree::ptree root;
        boost::property_tree::read_xml(filePath, root);

        // Set current directory.
        const boost::filesystem::path oldPath = boost::filesystem::current_path();
        const boost::filesystem::path currentPath = boost::filesystem::path(filePath);
        if(currentPath.has_parent_path())
          boost::filesystem::current_path(currentPath.parent_path());

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
        if( boost::optional<boost::property_tree::ptree&> child = root.get_child_optional("animation") )
        {
          BOOST_FOREACH(const boost::property_tree::ptree::value_type& current, child.get())
          {
            std::string file;
            Delay delay;
            const boost::property_tree::ptree& frame = current.second;

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
            const FrameInfo frameInfo = { boost::filesystem::absolute(file).string(), delay };
            _frameInfos.push_back(frameInfo);
          }
        }

        // Reset current directory.
        boost::filesystem::current_path(oldPath);

        return true;
      }

    } // namespace priv
  } // namespace spec
} // namespace apngasm
