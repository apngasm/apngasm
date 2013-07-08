#ifndef _LIBAPNGASM_H_
#define _LIBAPNGASM_H_


#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

class APNGFrame;

class APNGAsm
{
public:
	vector<APNGFrame> frames;
	APNGAsm(vector<APNGFrame> frames = NULL);

	//Adds a frame from a file
	//Returns the frame number in the frame vector
	//Uses default delay of 10ms if not specified
	int addFrame(string filepath, int delay = 10);

	//Adds an APNGFrame object to the frame vector
	//Returns the frame number in the frame vector
	int addFrame(APNGFrame frame);

	//Loads an animation spec from JSON or XML
	//Returns a frame vector with the loaded frames
	//Loaded frames are added to the end of the frame vector
	vector<APNGFrame> loadAnimationSpec(string filePath);

	//Assembles and outputs an APNG file
	//Returns the assembled file object
	//If no output path is specified only the file object is returned
	FILE* assemble(string outputPath);

private:
	vector<APNGFrame> loadJSONSpec(string filePath);
	vector<APNGFrame> loadXMLSpec(string filePath);
};

//Individual APNG frame
class APNGFrame
{
public:
	//Delay in ms
	int delay;

private:
};

#endif /* _LIBAPNGASM_H_ */
