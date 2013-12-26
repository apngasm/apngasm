#include "cli.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "listener/apngasmlistener.h"

static bool isNumber(const std::string s)
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
	CustomAPNGAsmListener(int overwriteMode)
		: _overwriteMode(overwriteMode)
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
		cout << "file `" << filePath << "' is already exists.";
		if(_overwriteMode == apngasm_cli::Options::OVERWRITE_INTERACTIVE) {
			cout << " overwrite?[y/N]: ";
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

private:
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

namespace apngasm_cli {
	const std::string CLI::VERSION = "0.1.1";

	CLI::CLI(int argc, char **argv)
		: options(argc, argv), assembler()
	{
		pListener = new CustomAPNGAsmListener(options.getOverwriteMode());
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

		FrameDelay delay;
		Options::INPUTS::const_iterator arg = options.inputFilesBegin();
		for(; arg != options.inputFilesEnd(); ++arg) {
			if(regex_match(*arg, DELAY_RE)) {
				delay = FrameDelay(*arg);
				continue;
			}
			if(!regex_match(*arg, PNG_RE)) {
				cout << "argument `" << (*arg)
					<< "' is invalid." << std::endl;
				return ERRCODE_INVALIDARGUMENT;
			}
			assembler.addFrame(*arg, delay.num, delay.den);
			delay = FrameDelay();
		}
		
		if (assembler.frameCount() == 0)
			cout << "apngasm " << assembler.version() << "\nNo arguments passed. Use --help for help" << std::endl;
		else
			cout << assembler.frameCount() << " Frames" << std::endl;

//		cout << "loops=" << options.getLoops() << std::endl;

		if (outfile == "") {
			outfile = "out.png";
		}
		if (assembler.assemble(outfile)) {
			cout << "assemble succeeded => " << outfile << std::endl;
			return 0;
		} else {
			return ERRCODE_ASSEMBLEERR;
		}
	}

	int CLI::disassemble(const std::string &src)
	{
		// Dissassemble apng file.
		std::vector<apngasm::APNGFrame> frames = assembler.disassemble(src);
		std::cout << "FrameCount=" << frames.size() << std::endl;

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
		if( options.outputJsonFile(outSpecFile) )
		{
			boost::filesystem::path path = outSpecFile;
			if(path.is_relative())
				outSpecFile = outdir + "/" + outSpecFile;

			assembler.saveJson(outSpecFile, outdir);
		}

		// Output xml spec files.
		if( options.outputXmlFile(outSpecFile) )
		{
			boost::filesystem::path path = outSpecFile;
			if(path.is_relative())
				outSpecFile = outdir + "/" + outSpecFile;

			assembler.saveXml(outSpecFile, outdir);
		}

		return 0;
	}
}
