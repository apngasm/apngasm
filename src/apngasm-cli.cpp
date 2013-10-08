#include "apngasm.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	APNGAsm apngasm;
	cout << "Initializing apngasm " << apngasm.version() << endl;

	apngasm.addFrame("gold01.png", 15, 100);
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

	cout << "OK" << endl;
	return 0;
}
