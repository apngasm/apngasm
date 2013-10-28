#include "options.h"
#include "apngasm.h"
#include <list>

class CLI {
public:
	static const std::string VERSION;
};

const std::string CLI::VERSION = "0.1.1";

namespace apngasm_cli {
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
		using boost::program_options::options_description;
		using boost::program_options::value;

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
			;
		pack.push_back(main_opts);
		visible_options.add(main_opts);

		options_description hidden_opts("Hidden options");
		hidden_opts.add_options()
			("files",
				value< vector<string> >(),
				"a list files to be turned into frames");
		pack.push_back(hidden_opts);
		positional_options.add("files", -1);
	}

	Options::Options(int argc, char **argv)
	{
		namespace po = boost::program_options;

		typedef std::list< po::options_description > OPTGROUPS;
		OPTGROUPS opts_packer;
		po::positional_options_description positional_opts;
		processOptions(opts_packer, visible_opts, positional_opts);

		// pack all options & parse it
		po::options_description all_opts;
			OPTGROUPS::iterator it = opts_packer.begin();
			for(; it != opts_packer.end(); ++it) {
				all_opts.add(*it);
			}
		po::command_line_parser parser(argc, argv);
		parser.options(all_opts);
		parser.positional(positional_opts);
		po::store(parser.run(), vm);
		po::notify(vm);
	}

	void Options::putVersionTo(std::basic_ostream<char> &out) const
	{
		out << "APNG Assembler v" << APNGASM_VERSION
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
			"    apngasm outfile.png frame1.png frame2.png frame3.png [options]\n"
			"    apngasm outfile.png frame*.png [options]\n"
			"\nAssemble an APNG file using a directive file:\n\n"
			"    apngasm outfile.png -f animation.json\n"
			"\nAssemble an APNG with specific frame delays in miliseconds:\n\n"
			"    apngasm outfile.png frame1.png 200 frame2.png 100 [options]\n"
			"\nAssemble an APNG with specific frame delays in fractions of a second:\n\n"
			"    apngasm outfile.png frame1.png 1:2 frame2.png 3:5 [options]\n"
			"\nDisassemble an APNG file into frames and JSON/XML directive files:\n\n"
			"    apngasm apng_file.png\n"
			"\nOptimize or re-assemble PNG/APNG file with new options:\n\n"
			"    apngasm outfile.png infile.png [options]\n"
//			"\nAdd a frame to an existing APNG or concatinate APNG animations:\n\n"
//			"    apngasm outfile.png apng1.png newframe.png apng2.png [options]\n"
		) << std::endl;
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
}
