#include "cli.h"
#include <iostream>

static void warn(const boost::program_options::error &e)
{
	std::cerr << "!!! " << e.what() << " !!!" << std::endl;
}

int main(int argc, char* argv[])
try {
	apngasm::APNGAsm apngasm;
	apngasm_cli::CLI cli(argc, argv);
	return cli.start();
} catch(const boost::program_options::invalid_command_line_syntax &e) {
	warn(e);
} catch(const boost::program_options::unknown_option              &e) {
	warn(e);
}
