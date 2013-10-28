#ifndef INCLGRD_OPTIONS_H_
#define INCLGRD_OPTIONS_H_

#include <boost/program_options.hpp>
#include <ostream>

namespace apngasm_cli {
	class Options {
	private:
		boost::program_options::variables_map vm;
		boost::program_options::options_description visible_opts;

	public:
		Options(int argc, char **argv);
		size_t count(const std::string &key) const;
		const boost::program_options::variable_value &
			operator[](const std::string &key) const;
		void putVersionTo(std::basic_ostream<char> &out) const;
		void putHelpTo(std::basic_ostream<char> &out) const;
	};
}

#endif  // INCLGRD_OPTIONS_H_
