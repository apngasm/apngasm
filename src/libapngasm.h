#ifndef _LIBAPNGASM_H_
#define _LIBAPNGASM_H_


#include <stdio.h>

#include <vector>
using namespace std;

class APNGAsm
{
public:
	vector<APNGFrame> frames;
	
	//Construct APNGAsm object
	APNGAsm(void);
	
	//Construct APNGAsm object
	APNGAsm(const vector<APNGFrame> &frames);

	//Adds a frame from a file
	//Returns the frame number in the frame vector
	//Uses default delay of 10ms if not specified
	int addFrame(const string &filePath, int delay = 10);

	//Adds an APNGFrame object to the frame vector
	//Returns the frame number in the frame vector
	int addFrame(const APNGFrame &frame);
	
	//Adds an APNGFrame object to the frame vector
	//Returns a frame vector with the added frames
	APNGAsm& operator << (const APNGFrame &frame);

	//Loads an animation spec from JSON or XML
	//Returns a frame vector with the loaded frames
	//Loaded frames are added to the end of the frame vector
	vector<APNGFrame> loadAnimationSpec(const string &filePath);

	//Assembles and outputs an APNG file
	//Returns the assembled file object
	//If no output path is specified only the file object is returned
	FILE* assemble(const string &outputPath);

private:
	//Loads an animation spec from JSON
	//Returns a frame vector with the loaded frames
	const vector<APNGFrame>& loadJSONSpec(const string &filePath);
	
	//Loads an animation spec from XML
	//Returns a frame vector with the loaded frames
	const vector<APNGFrame>& loadXMLSpec(const string &filePath);
};

#endif /* _LIBAPNGASM_H_ */
