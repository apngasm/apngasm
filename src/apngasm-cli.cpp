#include "apngasm.h"
#include "options.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <sstream>

namespace apngasm_cli {
	class Fraction {
	private:
		int n, d;

	public:
		Fraction(int numerator, int denominator);
		operator double(void) const;
	};
}

namespace apngasm_cli {
	Fraction::Fraction(int numerator, int denominator)
		: n(numerator), d(denominator)
	{
		// nop
	}

	Fraction::operator double(void) const
	{
		return n * 1.0 / d;
	}
}

static const int MILLI_SECONDS = 1000;
static const int DEFAULT_FRAME_LENGTH = 100;  // default frame length

// string is number?
static bool isNumber(const std::string s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

static apngasm_cli::Fraction parseFrameLength(
	const std::string &src_str)
{
	using namespace std;
	if(isNumber(src_str)) {
		return apngasm_cli::Fraction(
				atoi(src_str.c_str()),
				1000);
	} else { // Delay is in fractions of a second or invalid
		using boost::algorithm::split;

		vector<string> portions;
		split(portions, src_str, boost::is_any_of(":"));
		if(portions.size() != 2
		|| !isNumber(portions[0]) || !isNumber(portions[1])) {
			throw std::runtime_error("parse delay error");
		}
		return apngasm_cli::Fraction(
			atoi(portions[0].c_str()),
			atoi(portions[1].c_str()));
	}
}

int main(int argc, char* argv[])
{
	using namespace std;

	APNGAsm apngasm;
	apngasm_cli::Options vm(argc, argv);

	if (vm.count("help")) {
		vm.putHelpTo(std::cout);
	} else if (vm.count("version")) {
		cout << apngasm.version() << endl;
	} else {
		if (vm.count("delay")) {
			// TODO
		}
		if (vm.count("files")) {
			vector<string> files;
			vector<int> delay;
			const boost::regex period(".*\\..*");
			const boost::regex png(".*\\.png\\z");
			const boost::regex delayNum("[0-9]+[:[0-9]+]*");

			vector<string> fileParamsRaw( vm["files"].as< vector<string> >() );
			string outputFile = fileParamsRaw.front();
			vector<string>::iterator fileit = fileParamsRaw.erase(fileParamsRaw.begin());

			if (!regex_match(*fileit, period)) {
				cout << "there is not firstframe file" << endl;
				return 0;
			}

			//extract individual file names
			for (; fileit != fileParamsRaw.end(); ++fileit) {
				if (regex_match(*fileit, period)) {
					if (!regex_match(*fileit, png)) {
						cout << "ERROR:  \"" << *fileit << "\" is invalid extension" << endl;
						return 0;
					}
					files.push_back(*fileit);
					delay.push_back(DEFAULT_FRAME_LENGTH);
				} else {
					if (!regex_match(*fileit, delayNum)) {
						cout << "ERROR:  \"" << *fileit << "\" is invalid" << endl;
						return 0;
					}
					double sec = parseFrameLength(*fileit);
					delay.push_back(static_cast<int>(sec * 1000));
				}
			}
			cout << "frame count" << files.size() << endl;
		}
		cout << "opts were passed" << endl;
	}
	return 0;
}
