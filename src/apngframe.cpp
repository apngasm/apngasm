#include "apngframe.h"

unsigned char* APNGFrame::pixels(unsigned char* setPixels)
{
	if (setPixels != NULL)
		p = setPixels;
	return p;
}

unsigned int APNGFrame::width(unsigned int setWidth)
{
	if (setWidth != 0)
		w = setWidth;
	return w;
}

unsigned int APNGFrame::height(unsigned int setHeight)
{
	if (setHeight != 0)
		h = setHeight;
	return h;
}

unsigned char APNGFrame::colorType(unsigned char setColorType)
{
	if (setColorType != 255)
		t = setColorType;
	return t;
}

APNGFrame::APNGFrame()
{
	w = h = 0;
}

APNGFrame::APNGFrame(const std::string &filePath, unsigned delay_num, unsigned delay_den)
{
	//TODO save extracted info to self
  FILE * f;
  if ((f = fopen(filePath.c_str(), "rb")) != 0)
  {
    png_structp     png_ptr;
    png_infop       info_ptr;
    unsigned char   sig[8];

    if (fread(sig, 1, 8, f) == 8 && png_sig_cmp(sig, 0, 8) == 0)
    {
      png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      info_ptr = png_create_info_struct(png_ptr);
      if (png_ptr != NULL && info_ptr != NULL && setjmp(png_jmpbuf(png_ptr)) == 0)
      {
        APNGFrame frame;
        frame.delay_num = delay_num;
        frame.delay_den = delay_den;

        png_byte       depth;
        png_uint_32    rowbytes, i;
        png_colorp     palette;
        png_color_16p  trans_color;
        png_bytep      trans_alpha;
        png_bytepp     row_ptr = NULL;

        png_init_io(png_ptr, f);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);
        frame.w = png_get_image_width(png_ptr, info_ptr);
        frame.h = png_get_image_height(png_ptr, info_ptr);
        frame.t = png_get_color_type(png_ptr, info_ptr);
        depth   = png_get_bit_depth(png_ptr, info_ptr);
        if (depth < 8)
        {
          if (frame.t == PNG_COLOR_TYPE_PALETTE)
            png_set_packing(png_ptr);
          else
            png_set_expand(png_ptr);
        }
        else
        if (depth > 8)
        {
          png_set_expand(png_ptr);
          png_set_strip_16(png_ptr);
        }
        (void)png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        frame.t = png_get_color_type(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        memset(frame.pl, 255, sizeof(frame.pl));
        memset(frame.tr, 255, sizeof(frame.tr));

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &frame.ps))
          memcpy(frame.pl, palette, frame.ps * 3);
        else
          frame.ps = 0;

        if (png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &frame.ts, &trans_color))
        {
          if (frame.ts > 0)
          {
            if (frame.t == PNG_COLOR_TYPE_GRAY)
            {
              frame.tr[0] = 0;
              frame.tr[1] = trans_color->gray & 0xFF;
              frame.ts = 2;
            }
            else
            if (frame.t == PNG_COLOR_TYPE_RGB)
            {
              frame.tr[0] = 0;
              frame.tr[1] = trans_color->red & 0xFF;
              frame.tr[2] = 0;
              frame.tr[3] = trans_color->green & 0xFF;
              frame.tr[4] = 0;
              frame.tr[5] = trans_color->blue & 0xFF;
              frame.ts = 6;
            }
            else
            if (frame.t == PNG_COLOR_TYPE_PALETTE)
              memcpy(frame.tr, trans_alpha, frame.ts);
            else
              frame.ts = 0;
          }
        }
        else
          frame.ts = 0;

        frame.p = (unsigned char *)malloc(frame.h * rowbytes);
        row_ptr  = (png_bytepp)malloc(frame.h * sizeof(png_bytep));

        if (frame.p != NULL && row_ptr != NULL)
        {
          for (i=0; i<frame.h; i++)
            row_ptr[i] = frame.p + i*rowbytes;

          png_read_image(png_ptr, row_ptr);
          free(row_ptr);
          png_read_end(png_ptr, NULL);
          png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        }
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    }
    fclose(f);
  }
}

APNGFrame::APNGFrame(rgb *pixels, unsigned char tr[], unsigned delay_num, unsigned delay_den)
{
	//TODO
}

APNGFrame::APNGFrame(rgba *pixels, unsigned delay_num, unsigned delay_den)
{
	//TODO
}
