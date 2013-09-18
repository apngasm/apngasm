#ifndef _LIBAPNGASM_H_
#define _LIBAPNGASM_H_


#include <cstdio>
#include <string>
#include <vector>
#include "zlib.h"
#include "apngframe.h"
using namespace std;

#define APNGASM_VERSION "2.0.0"

typedef struct { unsigned char *p; unsigned int size; int x, y, w, h, valid, filters; } OP;
typedef struct { unsigned int num; unsigned char r, g, b, a; } COLORS;

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
	size_t addFrame(const string &filePath, unsigned int delay_num = 10, unsigned int delay_den = 100);

	//Adds an APNGFrame object to the frame vector
	//Returns the frame number in the frame vector
	size_t addFrame(const APNGFrame &frame);
	
	//Adds an APNGFrame object to the frame vector
	APNGAsm& operator << (const APNGFrame &frame);

	//Loads an animation spec from JSON or XML
	//Returns a frame vector with the loaded frames
	//Loaded frames are added to the end of the frame vector
	const vector<APNGFrame>& loadAnimationSpec(const string &filePath);

	//Assembles and outputs an APNG file
	//Returns the assembled file object
	//If no output path is specified only the file object is returned
	bool assemble(const string &outputPath);

	//Disassembles an APNG file
	//Returns the frame vector
	const vector<APNGFrame>& disassemble(const string &filePath);

	//Returns the number of frames
	size_t frameCount();

	//Throw away all frames, start over
	size_t reset();

	string version(void);

private:
	unsigned int FindCommonType(void);
	int UpconvertToCommonType(unsigned int coltype);
	void DirtyTransparencyOptimization(unsigned int coltype);
	unsigned int DownconvertOptimizations(unsigned int coltype, bool keep_palette, bool keep_coltype);

	bool Save(const string &outputPath, unsigned int coltype, unsigned int first, unsigned int loops);

	void process_rect(unsigned char * row, int rowbytes, int bpp, int stride, int h, unsigned char * rows);
	void deflate_rect_fin(unsigned char * zbuf, unsigned int * zsize, int bpp, int stride, unsigned char * rows, int zbuf_size, int n);
	void deflate_rect_op(unsigned char *pdata, int x, int y, int w, int h, int bpp, int stride, int zbuf_size, int n);
	void get_rect(unsigned int w, unsigned int h, unsigned char *pimage1, unsigned char *pimage2, unsigned char *ptemp, unsigned int bpp, unsigned int stride, int zbuf_size, unsigned int has_tcolor, unsigned int tcolor, int n);

	void write_chunk(FILE * f, const char * name, unsigned char * data, unsigned int length);
	void write_IDATs(FILE * f, int frame, unsigned char * data, unsigned int length, unsigned int idat_size);

	//Loads an animation spec from JSON
	//Returns a frame vector with the loaded frames
	const vector<APNGFrame>& loadJSONSpec(const string &filePath);
	
	//Loads an animation spec from XML
	//Returns a frame vector with the loaded frames
	const vector<APNGFrame>& loadXMLSpec(const string &filePath);

    OP              op[6];
    unsigned int    m_width;
    unsigned int    m_height;
    unsigned int    m_size;
    rgb             m_palette[256];
    unsigned char   m_trns[256];
    unsigned int    m_palsize;
    unsigned int    m_trnssize;
    unsigned int    m_next_seq_num;

    z_stream        op_zstream1;
    z_stream        op_zstream2;
    unsigned char * op_zbuf1;
    unsigned char * op_zbuf2;
    z_stream        fin_zstream;
    unsigned char * row_buf;
    unsigned char * sub_row;
    unsigned char * up_row;
    unsigned char * avg_row;
    unsigned char * paeth_row;
};

#endif /* _LIBAPNGASM_H_ */
