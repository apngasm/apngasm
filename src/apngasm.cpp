#include "libapngasm.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	APNGAsm apngasm;
	cout << "Initializing apngasm " << apngasm.version() << endl;

	return 0;
}
