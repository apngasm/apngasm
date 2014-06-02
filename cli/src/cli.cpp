#include "cli.h"
#include "apngasm-cli-version.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "listener/apngasmlistener.h"


namespace {
  const char separator = boost::filesystem::path::preferred_separator;

  bool isNumber(const std::string s)
  {
	  std::string::const_iterator it = s.begin();
	  while (it != s.end() && std::isdigit(*it)) ++it;
	  return !s.empty() && it == s.end();
  }

  class FrameDelay {
  public:
	  int num, den;
	  FrameDelay(void) : num(100), den(1000) {}
	  FrameDelay(int num) : num(num), den(1000) {}
	  FrameDelay(int num, int den) : num(num), den(den) {}
	  FrameDelay(const std::string &src_str)
		  : den(1000)
	  {
		  using namespace std;

		  if(isNumber(src_str)) {
			  num = atoi(src_str.c_str());
		  } else { // Delay is in fractions of a second or invalid
			  using boost::algorithm::split;

			  vector<string> portions;
			  split(portions, src_str, boost::is_any_of(":"));
			  if(portions.size() != 2
			  || !isNumber(portions[0]) || !isNumber(portions[1])) {
				  throw std::runtime_error("parse delay error");
			  }
			  num = atoi(portions[0].c_str());
			  den = atoi(portions[1].c_str());
		  }
	  }
  };

  class CustomAPNGAsmListener : public apngasm::listener::APNGAsmListener
  {
  public:
	  // Initialize CustomAPNGAsmListener object.
	  CustomAPNGAsmListener(const apngasm::APNGAsm* apngasm, int overwriteMode)
		  : _apngasm(apngasm)
      , _overwriteMode(overwriteMode)
	  {
		  // nop
	  }

    // Called after add frame.
    void onPostAddFrame(const std::string& filePath, unsigned int delayNum, unsigned int delayDen) const
    {
  	  std::cout << filePath << " => Delay=(" << delayNum << "/" << delayDen << ") sec" << std::endl;
    }

    // Called after add frame.
    void onPostAddFrame(const apngasm::APNGFrame& frame) const
    {
  	  apngasm::APNGFrame& tmp = const_cast<apngasm::APNGFrame&>(frame);
  	  std::cout << "New Frame => Delay=(" << tmp.delayNum() << "/" << tmp.delayDen() << ") sec" << std::endl;
    }

    // Called before save.
    // Return true if can save.
    bool onPreSave(const std::string& filePath) const
    {
		  using namespace std;
		  using namespace boost;

		  if(_overwriteMode == apngasm_cli::Options::OVERWRITE_FORCE) {
			  createParentDirs(filePath);
			  return true;
		  }
		  if(!filesystem::exists(filesystem::path(filePath))) {
			  createParentDirs(filePath);
			  return true;
		  }
		  cout << "The file '" << filePath << "' already exists.";
		  if(_overwriteMode == apngasm_cli::Options::OVERWRITE_INTERACTIVE) {
			  cout << " Overwrite? [y/N]: ";
			  string reply;
			  getline(cin, reply);
			  static const regex RE("\\Ay(es)?\\z", regex::icase);
			  if(regex_match(reply, RE)) {
				  return true;
			  }
		  } else {
			  cout << "\nuse `--interactive' or `--force' to overwrite." << endl;
		  }
		  return false;
    }

    // Called after save.
    void onPostSave(const std::string& filePath) const
    {
  	  std::cout << filePath << std::endl;
    }

    // Called when create output path of png file.
    // Return output path.
    const std::string onCreatePngPath(const std::string& outputDir, int index) const
    {
      // saving visible frames as #1, #2, #3, ...
      // saving skipped frame  as #0 (if it's exists)
      return apngasm::listener::APNGAsmListener::onCreatePngPath(outputDir, _apngasm->isSkipFirst() ? index : index+1);
    }

  private:
    const apngasm::APNGAsm* const _apngasm;
	  const int _overwriteMode;

	  // Return true if create succeeded.
	  bool createParentDirs(const std::string& filePath) const
	  {
		  boost::filesystem::path path = filePath;
		  boost::filesystem::path parent = path.parent_path();
		  if(parent == "") {
			  return true;
		  }
		  return boost::filesystem::create_directories(parent);
	  }
  };	// class CustomAPNGAsmListener

} // unnamed namespace

namespace apngasm_cli {
	const std::string CLI::VERSION = APNGASM_CLI_VERSION;

	CLI::CLI(int argc, char **argv)
		: options(argc, argv), assembler()
	{
		pListener = new CustomAPNGAsmListener(&assembler, options.getOverwriteMode());
		assembler.setAPNGAsmListener(pListener);
	}

	CLI::~CLI()
	{
		delete pListener;
	}

	int CLI::start(void)
	{
		using namespace std;
		if(options.has("help")) {
			options.putHelpTo(cout);
			return ERRCODE_HELP;
		}
		if(options.has("version")) {
			options.putVersionTo(cout);
			return ERRCODE_VERSION;
		}
		string srcFile;
		if(options.disassembleFile(srcFile)) {
			return disassemble(srcFile);
		}
		if(options.specFile(srcFile))
		{
			assembler.loadAnimationSpec(srcFile);
			return assemble();
		}

    assembler.setLoops(options.getLoops());
    assembler.setSkipFirst( options.has("skip") );
		return assemble();
	}

	int CLI::assemble(void)
	{
		using std::cout;
		std::string outfile;
		options.outputFile(outfile);

		// if(!checkOverwrite(outfile)) {
		// 	return ERRCODE_OUTPUTFILE_ALREADYEXISTS;
		// }

		static const boost::regex PNG_RE(".+\\.png\\z");
		static const boost::regex DELAY_RE("[0-9]+[:[0-9]+]?");
		const FrameDelay DEFAULT_DELAY = options.getDefaultDelay();

		std::string lastFile;
    std::string errorFile;
		Options::INPUTS::const_iterator arg = options.inputFilesBegin();
		for(; arg != options.inputFilesEnd(); ++arg) {
			if(regex_match(*arg, DELAY_RE)) {
				if(!lastFile.empty())
				{
					const FrameDelay delay = *arg;
					assembler.addFrame(lastFile, delay.num, delay.den);
					lastFile.clear();
					continue;
				}
			}
			else if(regex_match(*arg, PNG_RE) || arg->find("*", 0)) {
				if(!lastFile.empty())
				{
					const FrameDelay delay = DEFAULT_DELAY;
          const int frameCount = assembler.frameCount();
          if( frameCount < assembler.addFrame(lastFile, delay.num, delay.den) )
          {
            lastFile = *arg;
            continue;
          }
          errorFile = lastFile;
				}
        else
        {
          lastFile = *arg;
          continue;
        }
			}
      else
      {
        errorFile = *arg;
      }
		}
		if(!lastFile.empty())
		{
			const FrameDelay delay = DEFAULT_DELAY;
      const int frameCount = assembler.frameCount();
			if( frameCount >= assembler.addFrame(lastFile, delay.num, delay.den) )
        errorFile = lastFile;
		}

    if( !errorFile.empty() )
    {
			// Error.
			cout << "argument `" << errorFile
				<< "' is invalid." << std::endl;
			return ERRCODE_INVALIDARGUMENT;
    }
		
		if (assembler.frameCount() == 0)
			cout << "apngasm " << assembler.version() << "\nNo source frames were specified. Use --help for usage information." << std::endl;
		else
			cout << assembler.frameCount() << " Frames" << std::endl;

		if (outfile == "") {
			outfile = "out.png";
		}
		if (assembler.assemble(outfile)) {
			cout << "APNG assembled successfully => " << outfile << std::endl;
			return 0;
		} else {
			return ERRCODE_ASSEMBLEERR;
		}
	}

	int CLI::disassemble(const std::string &src)
	{
		// Dissassemble apng file.
		std::vector<apngasm::APNGFrame> frames = assembler.disassemble(src);
		std::cout << frames.size() << " Frames" << std::endl;

		// Output png image files.
		std::string outdir;
		if(!options.outputFile(outdir)) {
			boost::filesystem::path path = src;
			outdir = path.replace_extension("").string();
		}
		if( !assembler.savePNGs(outdir) )
			return 1;

		// Output json spec files.
		std::string outSpecFile;
		if( options.outputJSONFile(outSpecFile) )
		{
			boost::filesystem::path path = outSpecFile;
			if(path.is_relative())
				outSpecFile = outdir + separator + outSpecFile;

			assembler.saveJSON(outSpecFile, outdir);
		}

		// Output XML spec files.
		if( options.outputXMLFile(outSpecFile) )
		{
			boost::filesystem::path path = outSpecFile;
			if(path.is_relative())
				outSpecFile = outdir + separator + outSpecFile;

			assembler.saveXML(outSpecFile, outdir);
		}

		return 0;
	}
}
