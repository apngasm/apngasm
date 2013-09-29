#include "apngasm.h"

#include <iostream>
#include <sstream>
using namespace std;
#include <boost/program_options.hpp>

int main(int argc, char* argv[])
{
	APNGAsm apngasm;
	namespace bpo = boost::program_options;

	stringstream description;
	description << "APNG Assembler v" << apngasm.version() << endl \
		<< "Assemble an APNG:\n" \
		<< "\tapngasm outfile.png frame1.png frame2.png frame3.png [options]\n" \
		<< "\tapngasm outfile.png frame*.png [options]\n" \
		<< "Assemble an APNG with specific frame delays:\n" \
		<< "\tapngasm outfile.png frame1.png 200 frame2.png 100 [options]\n"
		<< "Disassemble an APNG file into frames and JSON/XML directive files:\n" \
		<< "\tapngasm apng_file.png\n" \
		<< "Optimize or re-assemble PNG/APNG file with new options:\n" \
		<< "\tapngasm outfile.png infile.png [options]\n" \
		<< "Add a frame to an existing APNG or concatinate APNG animations:\n"
		<< "\tapngasm outfile.png apng1.png newframe.png apng2.png [options]\n"
		<< "options";
	bpo::options_description opts(description.str());
	opts.add_options()
		("help,h",	"View detailed help.")
		("delay,d",	bpo::value<int>()->default_value(10), "Default frame delay [in miliseconds], default is 100.")
		("loops,l",	bpo::value<int>()->default_value(0), "Number of loops. Use 0 [the default] for infinite loops.")
		("file,f",	bpo::value<string>(), "Loads an XML or JSON animation directive file.")
		("skip,s",	"Skip the first frame. When animation is not supported the first frame is shown.");

	bpo::variables_map vm;
	store(parse_command_line(argc, argv, opts), vm);

	if (vm.count("help"))
		cout << opts << endl;
	else
	{
		cout << "opts were passed" << endl;
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

	cout << "OK" << endl;*/
	return 0;
}
