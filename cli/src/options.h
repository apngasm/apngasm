#ifndef INCLGRD_OPTIONS_H_
#define INCLGRD_OPTIONS_H_

#include <boost/program_options.hpp>
#include <ostream>

namespace apngasm_cli {
	class Options {
	private:
		std::vector<boost::program_options::option> option_vec;
		boost::program_options::variables_map vm;
		boost::program_options::options_description visible_opts;
		int overwrite_mode;

		void setOverwriteMode(void);

	public:
		static const int OVERWRITE_NO;
		static const int OVERWRITE_INTERACTIVE;
		static const int OVERWRITE_FORCE;
		typedef std::vector<std::string> INPUTS;

		Options(int argc, char **argv);
		size_t count(const std::string &key) const;
		const boost::program_options::variable_value &
			operator[](const std::string &key) const;
		void putVersionTo(std::basic_ostream<char> &out) const;
		void putHelpTo(std::basic_ostream<char> &out) const;
		int getLoops(void) const;
		const std::string getDefaultDelay(void) const;
		bool has(const std::string &key) const;
		int getOverwriteMode(void) const;
		bool outputFile(std::string &out) const;
		bool disassembleFile(std::string &out) const;
		bool specFile(std::string &out) const;
		bool outputJSONFile(std::string &out) const;
		bool outputXMLFile(std::string &out) const;
		const INPUTS::const_iterator inputFilesBegin(void) const;
		const INPUTS::const_iterator inputFilesEnd(void) const;
	};
}

#endif  // INCLGRD_OPTIONS_H_
