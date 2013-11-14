#ifndef _APNGASM_H_
#define _APNGASM_H_

#include <vector>
#include <string>
#include "apngframe.h"
#include "version.h"

namespace apngasm {

  typedef struct { unsigned char *p; unsigned int size; int x, y, w, h, valid, filters; } OP;

  struct CHUNK { unsigned int size; unsigned char * p; unsigned int flag; };
  struct FramePNG {
    std::vector<CHUNK> chunkSet;
    unsigned w, h, x, y, delay_num, delay_den;
    unsigned char dop, bop;
  };

	class APNGAsm {
	public:
		//Construct APNGAsm object
		APNGAsm(void);
		
		//Construct APNGAsm object
		APNGAsm(const std::vector<APNGFrame> &frames);

		//Adds a frame from a file
		//Returns the frame number in the frame vector
		//Uses default delay of 10ms if not specified
		size_t addFrame(const std::string &filePath, unsigned delayNum = DEFAULT_FRAME_NUMERATOR, unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);

		//Adds an APNGFrame object to the frame vector
		//Returns the frame number in the frame vector
		size_t addFrame(const APNGFrame &frame);
		
		//Adds an APNGFrame object to the frame vector
		APNGAsm& operator << (const APNGFrame &frame);

		//Loads an animation spec from JSON or XML
		//Returns a frame vector with the loaded frames
		//Loaded frames are added to the end of the frame vector
		const std::vector<APNGFrame>& loadAnimationSpec(const std::string &filePath);

		//Assembles and outputs an APNG file
		//Returns the assembled file object
		//If no output path is specified only the file object is returned
		bool assemble(const std::string &outputPath);

		//Disassembles an APNG file
		//Returns the frame vector
		const std::vector<APNGFrame>& disassemble(const std::string &filePath);

		void savePNG(char * szOut, APNGFrame * frame);

		//Returns the number of frames
		size_t frameCount();

		//Throw away all frames, start over
		size_t reset();

    //Returns the version of APNGAsm
		const char* version(void) const;

	private:
    //apng frame vector
    std::vector<APNGFrame> _frames;


		unsigned char findCommonType(void);
		int upconvertToCommonType(unsigned char coltype);
		void dirtyTransparencyOptimization(unsigned char coltype);
    unsigned char downconvertOptimizations(unsigned char coltype, bool keep_palette, bool keep_coltype);

		bool save(const std::string &outputPath, unsigned char coltype, unsigned first, unsigned loops);

		void process_rect(unsigned char * row, int rowbytes, int bpp, int stride, int h, unsigned char * rows);
		void deflate_rect_fin(unsigned char * zbuf, unsigned int * zsize, int bpp, int stride, unsigned char * rows, int zbuf_size, int n);
		void deflate_rect_op(unsigned char *pdata, int x, int y, int w, int h, int bpp, int stride, int zbuf_size, int n);
		void get_rect(unsigned int w, unsigned int h, unsigned char *pimage1, unsigned char *pimage2, unsigned char *ptemp, unsigned int bpp, unsigned int stride, int zbuf_size, unsigned int has_tcolor, unsigned int tcolor, int n);

		void write_chunk(FILE * f, const char * name, unsigned char * data, unsigned int length);
		void write_IDATs(FILE * f, int frame, unsigned char * data, unsigned int length, unsigned int idat_size);

		void decode_frame(APNGFrame * frameOut, FramePNG * frameIn);
		void compose_frame(unsigned char ** rows_dst, unsigned char ** rows_src, unsigned char bop, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		unsigned int read_chunk(FILE * f, CHUNK * pChunk);
		void recalc_crc(CHUNK * pChunk);

		//Loads an animation spec from JSON
		//Returns a frame vector with the loaded frames
		const std::vector<APNGFrame>& loadJSONSpec(const std::string &filePath);
		
		//Loads an animation spec from XML
		//Returns a frame vector with the loaded frames
		const std::vector<APNGFrame>& loadXMLSpec(const std::string &filePath);

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

		std::vector<CHUNK>   all_chunks;
	};	// class APNGAsm
	
}	// namespace apngasm

#endif  // _APNGASM_H_
