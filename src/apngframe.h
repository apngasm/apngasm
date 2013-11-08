#ifndef _APNGFRAME_H_
#define _APNGFRAME_H_

#include <png.h>
#include <cstdlib>
#include <string>

namespace apngasm {

  const unsigned DEFAULT_FRAME_NUMERATOR = 100;
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
  };  // class APNGFrame

} // namespace apngasm

#endif /* _APNGFRAME_H_ */
