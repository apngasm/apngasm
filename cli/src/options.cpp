#include "options.h"
#include "apngasm.h"
#include "cli.h"
#include <list>

//define FIRST_FILE_IS_OUT

class multiple_untyped_value
	: public boost::program_options::value_semantic_codecvt_helper<char>
{
public:
	bool is_composing() const { return false; }
	bool is_required() const { return false; }
	bool apply_default(boost::any&) const { return false; }
	unsigned min_tokens() const { return 0; }
	unsigned max_tokens() const { return 0; }
	void notify(const boost::any&) const {}

	std::string name(void) const {
		static std::string arg("arg");
		return arg;
	}

	void xparse(boost::any& value_store,
		const std::vector<std::string>& new_tokens) const
	{
		value_store = std::string("");
	}
};

namespace apngasm_cli {
	const int Options::OVERWRITE_INTERACTIVE = 0;
	const int Options::OVERWRITE_NO          = 1;
	const int Options::OVERWRITE_FORCE       = 2;

	static void processOptions(
		std::list< boost::program_options::options_description >
			&pack,
		boost::program_options::options_description
			&visible_options,
		boost::program_options::positional_options_description
			&positional_options )
	{
		using std::string;
		using std::vector;
		using namespace boost::program_options;

		options_description info_opts("Information Options");
		info_opts.add_options()
			("version,v", "Display the version.")
			(   "help,h", "View detailed help." );
		pack.push_back(info_opts);
		visible_options.add(info_opts);

		options_description main_opts("Main Options");
		main_opts.add_options()
			("delay,d",
				value<string>()->default_value("100"),
				"Default frame delay [in miliseconds or fractions of a second], default is 100.")
			("loops,l",
				value<int>()->default_value(0),
				"Number of loops. Use 0 [the default] for infinite loops.")
			("file,f",
				value<string>(),
				"Loads an XML or JSON animation directive file.")
			("skip,s",
				"Skip the first frame. When animation is not supported the first frame is shown.")
			("disassemble,D",
				value<string>(),
				"Specifies Disassemble file (Then, can NOT specify other files)")
			("output,o",
				value<string>(),
				"Specifies Output File (Then, 1st argument is NOT output-file)")
			("json,j",
				value<string>()->implicit_value("animation.json"),
				"Specifies output json file.")
			("xml,x",
				value<string>()->implicit_value("animation.xml"),
				"Specifies output xml file.")
			;
		pack.push_back(main_opts);
		visible_options.add(main_opts);

		options_description overwrite_opts("Overwrite Options");
		overwrite_opts.add_options()
			("nooverwrite,n",
				new multiple_untyped_value(),
				"exit when overwrite.")
			("interactive,i",
				new multiple_untyped_value(),
				"prompt before overwrite. (default)")
			("force,F",
				new multiple_untyped_value(),
				"always overwrites existing file.");
		pack.push_back(overwrite_opts);
		visible_options.add(overwrite_opts);

		options_description hidden_opts("Hidden options");
		hidden_opts.add_options()
			("files",
				value< vector<string> >(),
				"a list files to be turned into frames");
		pack.push_back(hidden_opts);
		positional_options.add("files", -1);
	}

	Options::Options(int argc, char **argv)
		: overwrite_mode(OVERWRITE_INTERACTIVE)
	{
		namespace po = boost::program_options;

		typedef std::list< po::options_description > OPTGROUPS;
		OPTGROUPS opts_packer;
		po::positional_options_description positional_opts;
		processOptions(opts_packer, visible_opts, positional_opts);

		// pack all options & parse it
		po::options_description all_opts;
		{
			OPTGROUPS::const_iterator it = opts_packer.begin();
			for(; it != opts_packer.end(); ++it) {
				all_opts.add(*it);
			}
		}
		po::command_line_parser parser(argc, argv);
		parser.options(all_opts);
		parser.positional(positional_opts);
		po::parsed_options parsed = parser.run();
		option_vec = parsed.options;
		po::store(parsed, vm);
		po::notify(vm);

		setOverwriteMode();
	}

	void Options::setOverwriteMode(void)
	{
		using std::vector;
		using boost::program_options::option;
		vector<option>::const_iterator it = option_vec.begin();
		for(; it != option_vec.end(); ++it) {
			if(it->string_key == "nooverwrite") {
				overwrite_mode = OVERWRITE_NO;
			} else if(it->string_key == "force") {
				overwrite_mode = OVERWRITE_FORCE;
			} else if(it->string_key == "interactive") {
				overwrite_mode = OVERWRITE_INTERACTIVE;
			}
		}
	}

	void Options::putVersionTo(std::basic_ostream<char> &out) const
	{
		out << "APNG Assembler v" << apngasm::APNGASM_VERSION
			<< " (frontend v" << CLI::VERSION << ")" << std::endl;
	}

	void Options::putHelpTo(std::basic_ostream<char> &out) const
	{
		putVersionTo(out);
		out << "===============================================================================\n\n";
		out << (
			"ALLOWED OPTIONS\n"
			"-------------------------------------------------------------------------------\n"
		);

		out << visible_opts << std::endl;
		out << (
			"EXAMPLE\n"
			"-------------------------------------------------------------------------------\n"
			"\nAssemble an APNG:\n\n"
			"    apngasm -o outfile.png frame1.png frame2.png frame3.png [options]\n"
			"    apngasm -o outfile.png frame*.png [options]\n"
			"\nAssemble an APNG with specific frame delays in miliseconds:\n\n"
			"    apngasm -o outfile.png frame1.png 200 frame2.png 100 [options]\n"
			"\nAssemble an APNG with specific frame delays in fractions of a second:\n\n"
			"    apngasm -o outfile.png frame1.png 1:2 frame2.png 3:5 [options]\n"
			"\nDisassemble an APNG file into frames and JSON/XML directive files:\n\n"
			"    apngasm -o output_dir -D apng_file.png\n"
		) << std::endl;
	}

	int Options::getOverwriteMode(void) const
	{
		return overwrite_mode;
	}

	const std::string Options::getDefaultDelay(void) const
	{
		return vm["delay"].as<std::string>();
	}

	int Options::getLoops(void) const
	{
		return vm["loops"].as<int>();
	}

	size_t Options::count(const std::string &key) const
	{
		return vm.count(key);
	}

	const boost::program_options::variable_value &
		Options::operator[](const std::string &key) const
	{
		return vm[key];
	}

	bool Options::has(const std::string &key) const
	{
		return !!vm.count(key);
	}

	bool Options::outputFile(std::string &out) const
	{
		using std::string;
		if(vm.count("output")) {
			out = vm["output"].as<string>();
			return true;
		}
#ifdef FIRST_FILE_IS_OUT
		if(vm.count("files")) {
#else
		if(vm.count("disassemble") && vm.count("files")) {
#endif
			if(vm["files"].as< std::vector<string> >().size() == 1) {
				out = vm["files"].as< std::vector<string> >()[0];
				return true;
			}
		}
		return false;
	}

	bool Options::disassembleFile(std::string &out) const
	{
		if(!vm.count("disassemble")) {
			return false;
		}
		out = vm["disassemble"].as<std::string>();
		return true;
	}

	bool Options::specFile(std::string &out) const
	{
		if(!vm.count("file")) {
			return false;
		}
		out = vm["file"].as<std::string>();
		return true;
	}

	bool Options::outputJSONFile(std::string &out) const
	{
		if(!vm.count("json")) {
			return false;
		}
		out = vm["json"].as<std::string>();
		return true;
	}

	bool Options::outputXMLFile(std::string &out) const
	{
		if(!vm.count("xml")) {
			return false;
		}
		out = vm["xml"].as<std::string>();
		return true;
	}

	const std::vector<std::string>::const_iterator
		Options::inputFilesBegin(void) const
	{
		using std::vector;
		using std::string;
		if(!vm.count("files")) {
			return inputFilesEnd();
		}
		vector<string>::const_iterator it
			= vm["files"].as< vector<string> >().begin();
#ifdef FIRST_FILE_IS_OUT
		if(!vm.count("output")) {
			++it;
		}
#endif
		return it;
	}

	const std::vector<std::string>::const_iterator
		Options::inputFilesEnd(void) const
	{
		static std::vector<std::string> empty_vector(0);
		return (vm.count("files")
			? vm["files"].as< std::vector<std::string> >().end()
			: empty_vector.end());
	}
}
