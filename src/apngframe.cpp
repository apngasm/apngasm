#include "apngframe.h"
#include <png.h>
#include <cstdlib>

namespace apngasm {

  unsigned char* APNGFrame::pixels(unsigned char* setPixels)
  {
  	if (setPixels != NULL)
  		_pixels  = setPixels;
  	return _pixels;
  }

  unsigned int APNGFrame::width(unsigned int setWidth)
  {
  	if (setWidth != 0)
  		_width = setWidth;
  	return _width;
  }

  unsigned int APNGFrame::height(unsigned int setHeight)
  {
  	if (setHeight != 0)
  		_height = setHeight;
  	return _height;
  }

  unsigned char APNGFrame::colorType(unsigned char setColorType)
  {
  	if (setColorType != 255)
  		_colorType = setColorType;
  	return _colorType;
  }

  APNGFrame::APNGFrame()
  {
  	_width = _height = 0;
  }

  APNGFrame::APNGFrame(const std::string &filePath, unsigned delayNum, unsigned delayDen)
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
          _delayNum = delayNum;
          _delayDen = delayDen;

          png_byte       depth;
          png_uint_32    rowbytes, i;
          png_colorp     palette;
          png_color_16p  trans_color;
          png_bytep      trans_alpha;
          png_bytepp     row_ptr = NULL;

          png_init_io(png_ptr, f);
          png_set_sig_bytes(png_ptr, 8);
          png_read_info(png_ptr, info_ptr);
          _width = png_get_image_width(png_ptr, info_ptr);
          _height = png_get_image_height(png_ptr, info_ptr);
          _colorType = png_get_color_type(png_ptr, info_ptr);
          depth   = png_get_bit_depth(png_ptr, info_ptr);
          if (depth < 8)
          {
            if (_colorType == PNG_COLOR_TYPE_PALETTE)
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
          _colorType = png_get_color_type(png_ptr, info_ptr);
          rowbytes = png_get_rowbytes(png_ptr, info_ptr);
          memset(_palette, 255, sizeof(_palette));
          memset(_transparency, 255, sizeof(_transparency));

          if (png_get_PLTE(png_ptr, info_ptr, &palette, &_palleteSize))
            memcpy(_palette, palette, _palleteSize * 3);
          else
            _palleteSize = 0;

          if (png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &_transparencySize, &trans_color))
          {
            if (_transparencySize > 0)
            {
              if (_colorType == PNG_COLOR_TYPE_GRAY)
              {
                _transparency[0] = 0;
                _transparency[1] = trans_color->gray & 0xFF;
                _transparencySize = 2;
              }
              else
              if (_colorType == PNG_COLOR_TYPE_RGB)
              {
                _transparency[0] = 0;
                _transparency[1] = trans_color->red & 0xFF;
                _transparency[2] = 0;
                _transparency[3] = trans_color->green & 0xFF;
                _transparency[4] = 0;
                _transparency[5] = trans_color->blue & 0xFF;
                _transparencySize = 6;
              }
              else
              if (_colorType == PNG_COLOR_TYPE_PALETTE)
                memcpy(_transparency, trans_alpha, _transparencySize);
              else
                _transparencySize = 0;
            }
          }
          else
            _transparencySize = 0;

          _pixels = (unsigned char *)malloc(_height * rowbytes);
          row_ptr  = (png_bytepp)malloc(_height * sizeof(png_bytep));

          if (_pixels != NULL && row_ptr != NULL)
          {
            for (i=0; i<_height; i++)
              row_ptr[i] = _pixels + i*rowbytes;

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

  APNGFrame::APNGFrame(rgb *pixels, unsigned char tr[], unsigned delayNum, unsigned delayDen)
  {
  	//TODO
  }

  APNGFrame::APNGFrame(rgba *pixels, unsigned delay_num, unsigned delay_den)
  {
  	//TODO
  }

  // Save frame to a PNG image file.
  void APNGFrame::save(const std::string& outPath)
  {
    FILE * f;
    if ((f = fopen(outPath.c_str(), "wb")) != 0)
    {
      png_structp  png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      png_infop    info_ptr = png_create_info_struct(png_ptr);
      if (png_ptr != NULL && info_ptr != NULL && setjmp(png_jmpbuf(png_ptr)) == 0)
      {
        png_init_io(png_ptr, f);
        png_set_compression_level(png_ptr, 9);
        png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, 6, 0, 0, 0);
        png_write_info(png_ptr, info_ptr);
        png_write_image(png_ptr, _rows);
        png_write_end(png_ptr, info_ptr);
      }
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(f);
    }
  }

} // namespace apngasm
