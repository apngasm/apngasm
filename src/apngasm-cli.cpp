#include "apngasm.h"

#include <iostream>
#include <sstream>
using namespace std;
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
//#include <boost/algorithm/string/regex.hpp>

#define MILISECOND 1000
#define DERAYNUMERATOR 100

bool isNumber(const string s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

// bool parseDelay(const string delay, int *numerator, int *denominator)
// {
// 	if (isNumber(delay)) { // The delay is in miliseconds
// 		*numerator = atoi(delay.c_str());
// 		*denominator = MILISECOND;
// 		return true;
// 	} else { // Delay is in fractions of a second or invalid
// 		//TODO parse numerator and denominator
// 		vector<string> portions;
// 		boost::algorithm::split(portions, delay, boost::is_any_of(":"));
// 		*numerator = atoi((portions.front()).c_str()) * 1000;

// 		for (vector<string>::iterator it = portions.erase(portions.begin()); it != portions.end(); ++it) {
// 			*numerator /= (float)atoi(it->c_str());
// 		}
// 		*denominator = MILISECOND;
// 	}
// 	return false;
// }

int parseDelay(const string delay, int *denominator)
{
	int delays = -1;
	if (isNumber(delay)) { // The delay is in miliseconds
		delays = atoi(delay.c_str());
		*denominator = MILISECOND;
	} else { // Delay is in fractions of a second or invalid
		vector<string> portions;
		boost::algorithm::split(portions, delay, boost::is_any_of(":"));
		delays = atoi((portions.front()).c_str()) * 1000;

		for (vector<string>::iterator it = portions.erase(portions.begin()); it != portions.end(); ++it) {
			delays /= (float)atoi(it->c_str());
		}
		*denominator = MILISECOND;
	}
	return delays;
}

int main(int argc, char* argv[])
{
	APNGAsm apngasm;
	namespace bpo = boost::program_options;

	// Defaults
	int delayDenominator = MILISECOND;

	stringstream description;
	description << "APNG Assembler v" << apngasm.version() << endl \
		<< "Assemble an APNG:\n" \
		<< "\tapngasm outfile.png frame1.png frame2.png frame3.png [options]\n" \
		<< "\tapngasm outfile.png frame*.png [options]\n" \
		<< "Assemble an APNG file using a directive file:\n" \
		<< "\tapngasm outfile.png -f animation.json\n" \
		<< "Assemble an APNG with specific frame delays in miliseconds:\n" \
		<< "\tapngasm outfile.png frame1.png 200 frame2.png 100 [options]\n"
		<< "Assemble an APNG with specific frame delays in fractions of a second:\n" \
		<< "\tapngasm outfile.png frame1.png 1:2 frame2.png 3:5 [options]\n"
		<< "Disassemble an APNG file into frames and JSON/XML directive files:\n" \
		<< "\tapngasm apng_file.png\n" \
		<< "Optimize or re-assemble PNG/APNG file with new options:\n" \
		<< "\tapngasm outfile.png infile.png [options]\n" \
		//<< "Add a frame to an existing APNG or concatinate APNG animations:\n"
		//<< "\tapngasm outfile.png apng1.png newframe.png apng2.png [options]\n"
		<< "options";
	bpo::options_description opts(description.str());
	opts.add_options()
		("help,h",	"View detailed help.")
		("delay,d",	bpo::value<string>()->default_value("100"), "Default frame delay [in miliseconds or fractions of a second], default is 100.")
		("loops,l",	bpo::value<int>()->default_value(0), "Number of loops. Use 0 [the default] for infinite loops.")
		("file,f",	bpo::value<string>(), "Loads an XML or JSON animation directive file.")
		("skip,s",	"Skip the first frame. When animation is not supported the first frame is shown.")
		("version,v", "Display the version.");

	bpo::options_description hidden("Hidden options");
	hidden.add_options()
		("files", bpo::value< vector<string> >(), "a list files to be turned into frames");

	bpo::options_description cmdline_options;
	cmdline_options.add(opts).add(hidden);

	bpo::options_description visible("Allowed options");
	visible.add(opts);

	bpo::positional_options_description p;
	p.add("files", -1);

	bpo::variables_map vm;
	store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);

	if (vm.count("help")) {
		cout << visible << endl;
	} else if (vm.count("version")) {
		cout << apngasm.version() << endl;
	} else {
		if (vm.count("delay")) {
			// const string delayOverride = vm["delay"].as<string>();
			// if (parseDelay(delayOverride, &delayNumerator, &delayDenominator)) {
			// 	cout << "Default delay overridden to: " << delayNumerator << "/" << delayDenominator << " seconds" << endl;
			// }
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
					delay.push_back(DERAYNUMERATOR);
				} else {
					if (!regex_match(*fileit, delayNum)) {
						cout << "ERROR:  \"" << *fileit << "\" is invalid" << endl;
						return 0;
					}
					*((delay.end()) - 1) = parseDelay(*fileit, &delayDenominator);
				}
			}
			cout << "frame count" << files.size() << endl;
		}
		cout << "opts were passed" << endl;
		//cout << "count: " << vm.count() << endl;
	}


	/*apngasm.addFrame("gold01.png", 15, 100);
	apngasm.addFrame("gold02.png", 15, 100);
	apngasm.addFrame("gold03.png", 15, 100);
	apngasm.assemble("gold_anim.png");
	cout << "frames=" << apngasm.frameCount() << endl;

	apngasm.reset();

	apngasm.addFrame("clock1.png", 1, 1);
	apngasm.addFrame("clock2.png", 1, 1);
	apngasm.addFrame("clock3.png", 1, 1);
	apngasm.addFrame("clock4.png", 1, 1);
	apngasm.assemble("clock_anim.png");
	cout << "frames=" << apngasm.frameCount() << endl;

	apngasm.disassemble("penguins.png");
	char szOut[256];
	for (unsigned int i=0; i<apngasm.frames.size(); ++i)
	{
		sprintf(szOut, "penguins_frame%02d.png", i);
		apngasm.SavePNG(szOut, &apngasm.frames[i]);
	}

	cout << "OK" << endl;*/
	return 0;
}
