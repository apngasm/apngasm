#ifndef _APNGFRAME_H_
#define _APNGFRAME_H_

typedef struct { unsigned char r, g, b; } rgb;

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
private:
};

#endif /* _APNGFRAME_H_ */
