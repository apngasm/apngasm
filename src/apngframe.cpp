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
        delay_num = delay_num;
        delay_den = delay_den;

        png_byte       depth;
        png_uint_32    rowbytes, i;
        png_colorp     palette;
        png_color_16p  trans_color;
        png_bytep      trans_alpha;
        png_bytepp     row_ptr = NULL;

        png_init_io(png_ptr, f);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);
        w = png_get_image_width(png_ptr, info_ptr);
        h = png_get_image_height(png_ptr, info_ptr);
        t = png_get_color_type(png_ptr, info_ptr);
        depth   = png_get_bit_depth(png_ptr, info_ptr);
        if (depth < 8)
        {
          if (t == PNG_COLOR_TYPE_PALETTE)
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
        t = png_get_color_type(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        memset(pl, 255, sizeof(pl));
        memset(tr, 255, sizeof(tr));

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &ps))
          memcpy(pl, palette, ps * 3);
        else
          ps = 0;

        if (png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &ts, &trans_color))
        {
          if (ts > 0)
          {
            if (t == PNG_COLOR_TYPE_GRAY)
            {
              tr[0] = 0;
              tr[1] = trans_color->gray & 0xFF;
              ts = 2;
            }
            else
            if (t == PNG_COLOR_TYPE_RGB)
            {
              tr[0] = 0;
              tr[1] = trans_color->red & 0xFF;
              tr[2] = 0;
              tr[3] = trans_color->green & 0xFF;
              tr[4] = 0;
              tr[5] = trans_color->blue & 0xFF;
              ts = 6;
            }
            else
            if (t == PNG_COLOR_TYPE_PALETTE)
              memcpy(tr, trans_alpha, ts);
            else
              ts = 0;
          }
        }
        else
          ts = 0;

        p = (unsigned char *)malloc(h * rowbytes);
        row_ptr  = (png_bytepp)malloc(h * sizeof(png_bytep));

        if (p != NULL && row_ptr != NULL)
        {
          for (i=0; i<h; i++)
            row_ptr[i] = p + i*rowbytes;

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
