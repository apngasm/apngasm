#ifndef _APNGFRAME_H_
#define _APNGFRAME_H_

#define DEFAULT_FRAME_NUMERATOR 100
#define DEFAULT_FRAME_DENOMINATOR 1000

typedef struct { unsigned char r, g, b; } rgb;
typedef struct { unsigned char r, g, b, a; } rgba;

//Individual APNG frame
class APNGFrame
{
public:
  //Pointer to raw pixel data
  unsigned char * p;
  //Width and Height
  unsigned int w, h;
  //PNG color type
  unsigned char t;
  //Palette into
  rgb pl[256];
  //Transparency info
  unsigned char tr[256];
  //Sizes for palette and transparency records
  int ps, ts;
  //Delay is numerator/denominator ratio, in seconds
  unsigned int delay_num, delay_den;

  unsigned char ** rows;

  APNGFrame();
  APNGFrame(const std::string &filePath, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);
  APNGFrame(rgb *pixels, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);
  APNGFrame(rgba *pixels, unsigned delay_num = DEFAULT_FRAME_NUMERATOR, unsigned delay_den = DEFAULT_FRAME_DENOMINATOR);
private:
	int cmp_colors( const void *arg1, const void *arg2 );
};

#endif /* _APNGFRAME_H_ */
