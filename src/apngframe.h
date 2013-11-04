#ifndef _APNGFRAME_H_
#define _APNGFRAME_H_

#include <png.h>
#include <zlib.h>
#include <cstdlib>
#include <vector>
#include <string>
using namespace std;

#if defined(_MSC_VER) && _MSC_VER >= 1300
#define swap16(data) _byteswap_ushort(data)
#define swap32(data) _byteswap_ulong(data)
#elif defined(__linux__)
#include <byteswap.h>
#define swap16(data) bswap_16(data)
#define swap32(data) bswap_32(data)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define swap16(data) bswap16(data)
#define swap32(data) bswap32(data)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define swap16(data) OSSwapInt16(data)
#define swap32(data) OSSwapInt32(data)
#else
unsigned short swap16(unsigned short data) {return((data & 0xFF) << 8) | ((data >> 8) & 0xFF);}
unsigned int swap32(unsigned int data) {return((data & 0xFF) << 24) | ((data & 0xFF00) << 8) | ((data >> 8) & 0xFF00) | ((data >> 24) & 0xFF);}
#endif

#define notabc(c) ((c) < 65 || (c) > 122 || ((c) > 90 && (c) < 97))

#define id_IHDR 0x52444849
#define id_acTL 0x4C546361
#define id_fcTL 0x4C546366
#define id_IDAT 0x54414449
#define id_fdAT 0x54416466
#define id_IEND 0x444E4549

typedef struct { unsigned char *p; unsigned int size; int x, y, w, h, valid, filters; } OP;
typedef struct { unsigned int num; unsigned char r, g, b, a; } COLORS;

struct CHUNK { unsigned int size; unsigned char * p; unsigned int flag; };
struct FramePNG {
	std::vector<CHUNK> chunkSet;
	unsigned w, h, x, y, delay_num, delay_den;
	unsigned char dop, bop;
};

#define DEFAULT_FRAME_NUMERATOR 100
#define DEFAULT_FRAME_DENOMINATOR 1000

typedef struct { unsigned char r, g, b; } rgb;
typedef struct { unsigned char r, g, b, a; } rgba;

//Individual APNG frame
class APNGFrame
{
public:
  // Raw pixel data
  unsigned char* pixels(unsigned char* setPixels = NULL);
  unsigned char* p;
  
  // Width and Height
  unsigned int width(unsigned int setWidth = 0);
  unsigned int height(unsigned int setHeight = 0);
  unsigned int w;
  unsigned int h;

  // PNG color type
  unsigned char colorType(unsigned char setColorType = 255);
  unsigned char t;
  
  // Palette into
  rgb pl[256];
  
  //Transparency info
  unsigned char tr[256];
  
  //Sizes for palette and transparency records
  int ps, ts;
  
  //Delay is numerator/denominator ratio, in seconds
  unsigned int delay_num, delay_den;

  unsigned char ** rows;

  // Init empty frame for filling in manually
  APNGFrame();
  // Init frame from a PNG image file
  APNGFrame(const std::string &filePath, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);
  // Init frame 
  APNGFrame(rgb *pixels, unsigned char tr[] = NULL, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);
  APNGFrame(rgba *pixels, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);

private:
};

#endif /* _APNGFRAME_H_ */
