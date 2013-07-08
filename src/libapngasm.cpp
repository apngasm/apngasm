#inlucde "libapngasm.h"
#include <string>
namespace std;

//Construct APNGAsm object
APNGAsm::APNGAsm(void){}

//Construct APNGAsm object
APNGAsm::APNGAsm(const vector<APNGFrame> &frames)
{
	this->frames.insert(this->frames.end(), frames.begin(), frames.end());
}

//Adds a frame from a file
//Returns the frame number in the frame vector
//Uses default delay of 10ms if not specified
int APNGAsm::addFrame(const string &filePath, int delay)
{
	return frames.size();
}

//Adds an APNGFrame object to the frame vector
//Returns the frame number in the frame vector
int APNGAsm::addFrame(const APNGFrame &frame)
{
	frames.push_back(frame);
	return frames.size();
}

//Adds an APNGFrame object to the frame vector
//Returns a frame vector with the added frames
APNGAsm& APNGAsm::operator << (const APNGFrame &frame)
{
	addFrame(frame);
	return *this;
}

//Loads an animation spec from JSON or XML
//Returns a frame vector with the loaded frames
//Loaded frames are added to the end of the frame vector
const vector<APNGFrame>& loadAnimationSpec(const string &filePath)
{
	const bool isJSON = true;
	const vector<APNGFrame> &newFrames = isJSON ? loadJSONSpec(filePath) : loadXMLSpec(filePath));
	frames.insert(frames.end(), newFrames.begin(), newFrames.end());
	return frames;
}

//Assembles and outputs an APNG file
//Returns the assembled file object
//If no output path is specified only the file object is returned
FILE* APNGAsm::assemble(const string &outputPath)
{
	return NULL;
}

// private static object
{
	vector<APNGFrame> tmpFrames;
}

//Loads an animation spec from JSON
//Returns a frame vector with the loaded frames
const vector<APNGFrame>& loadJSONSpec(const string &filePath)
{
	return tmpFrames;
}

//Loads an animation spec from XML
//Returns a frame vector with the loaded frames
const vector<APNGFrame>& loadXMLSpec(const string &filePath)
{
	return tmpFrames;
}
