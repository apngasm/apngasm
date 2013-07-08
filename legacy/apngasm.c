/* APNG Assembler 2.7
 *
 * This program creates APNG animation from PNG/TGA image sequence.
 *
 * http://apngasm.sourceforge.net/
 *
 * Copyright (c) 2009-2012 Max Stepin
 * maxst at users.sourceforge.net
 *
 * zlib license
 * ------------
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "png.h"     /* original (unpatched) libpng is ok */
#include "zlib.h"

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

typedef struct { z_stream zstream; unsigned char * zbuf; int x, y, w, h, valid; } OP;
typedef struct { unsigned int num; unsigned char r, g, b, a; } COLORS;
typedef struct { unsigned char r, g, b; } rgb;
typedef struct { unsigned char * p; unsigned int w, h; int t, ps, ts; rgb pl[256]; unsigned char tr[256]; unsigned short num, den; } image_info;

OP      op[12];
COLORS  col[256];

unsigned int    next_seq_num = 0;
unsigned char * row_buf;
unsigned char * sub_row;
unsigned char * up_row;
unsigned char * avg_row;
unsigned char * paeth_row;
unsigned char   png_sign[8] = {137,  80,  78,  71,  13,  10,  26,  10};
unsigned char   png_Software[27] = { 83, 111, 102, 116, 119, 97, 114, 101, '\0', 
                                     65,  80,  78,  71,  32, 65, 115, 115, 101, 
                                    109,  98, 108, 101, 114, 32,  50,  46,  55};

int cmp_colors( const void *arg1, const void *arg2 )
{
  if ( ((COLORS*)arg1)->a != ((COLORS*)arg2)->a )
    return (int)(((COLORS*)arg1)->a) - (int)(((COLORS*)arg2)->a);

  if ( ((COLORS*)arg1)->num != ((COLORS*)arg2)->num )
    return (int)(((COLORS*)arg2)->num) - (int)(((COLORS*)arg1)->num);

  if ( ((COLORS*)arg1)->r != ((COLORS*)arg2)->r )
    return (int)(((COLORS*)arg1)->r) - (int)(((COLORS*)arg2)->r);

  if ( ((COLORS*)arg1)->g != ((COLORS*)arg2)->g )
    return (int)(((COLORS*)arg1)->g) - (int)(((COLORS*)arg2)->g);

  return (int)(((COLORS*)arg1)->b) - (int)(((COLORS*)arg2)->b);
}

int LoadPNG(char * szImage, image_info * pInfo)
{
  FILE * f;
  int res = 0;

  if ((f = fopen(szImage, "rb")) != 0)
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
        png_byte       depth;
        png_uint_32    rowbytes, i;
        png_colorp     palette;
        png_color_16p  trans_color;
        png_bytep      trans_alpha;
        png_bytepp     row_ptr = NULL;

        png_init_io(png_ptr, f);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);
        pInfo->w = png_get_image_width(png_ptr, info_ptr);
        pInfo->h = png_get_image_height(png_ptr, info_ptr);
        pInfo->t = png_get_color_type(png_ptr, info_ptr);
        depth    = png_get_bit_depth(png_ptr, info_ptr);
        if (depth < 8)
        {
          if (pInfo->t == PNG_COLOR_TYPE_PALETTE)
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
        pInfo->t = png_get_color_type(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &pInfo->ps))
          memcpy(pInfo->pl, palette, pInfo->ps * 3);
        else
          pInfo->ps = 0;

        if (png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &pInfo->ts, &trans_color))
        {
          if (pInfo->ts > 0)
          {
            if (pInfo->t == PNG_COLOR_TYPE_GRAY)
            {
              pInfo->tr[0] = 0;
              pInfo->tr[1] = trans_color->gray & 0xFF;
              pInfo->ts = 2;
            }
            else
            if (pInfo->t == PNG_COLOR_TYPE_RGB)
            {
              pInfo->tr[0] = 0;
              pInfo->tr[1] = trans_color->red & 0xFF;
              pInfo->tr[2] = 0;
              pInfo->tr[3] = trans_color->green & 0xFF;
              pInfo->tr[4] = 0;
              pInfo->tr[5] = trans_color->blue & 0xFF;
              pInfo->ts = 6;
            }
            else
            if (pInfo->t == PNG_COLOR_TYPE_PALETTE)
              memcpy(pInfo->tr, trans_alpha, pInfo->ts);
            else
              pInfo->ts = 0;
          }
        }
        else
          pInfo->ts = 0;

        pInfo->p = (unsigned char *)malloc(pInfo->h * rowbytes);
        row_ptr  = (png_bytepp)malloc(pInfo->h * sizeof(png_bytep));

        if (pInfo->p != NULL && row_ptr != NULL)
        {
          for (i=0; i<pInfo->h; i++)
            row_ptr[i] = pInfo->p + i*rowbytes;

          png_read_image(png_ptr, row_ptr);
          free(row_ptr);
          png_read_end(png_ptr, NULL);
          png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        }
        else
          res = 4;
      }
      else
        res = 3;

      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    }
    else
      res = 2;

    fclose(f);
  }
  else
    res = 1;

  return res;
}

int LoadTGA(char * szImage, image_info * pInfo)
{
  FILE * f;
  int res = 0;

  if ((f = fopen(szImage, "rb")) != 0)
  {
    unsigned int  i, j, compr;
    unsigned int  k, n, rowbytes;
    unsigned char c;
    unsigned char col[4];
    unsigned char fh[18];
    unsigned char * row_ptr;

    if (fread(&fh, 1, 18, f) != 18) return 2;

    pInfo->w = fh[12] + fh[13]*256;
    pInfo->h = fh[14] + fh[15]*256;
    pInfo->t = -1;
    pInfo->ps = 0;
    pInfo->ts = 0;

    rowbytes = pInfo->w;
    if ((fh[2] & 7) == 3)
      pInfo->t = 0;
    else
    if (((fh[2] & 7) == 2) && (fh[16] == 24))
    {
      pInfo->t = 2;
      rowbytes = pInfo->w * 3;
    }
    else
    if (((fh[2] & 7) == 2) && (fh[16] == 32))
    {
      pInfo->t = 6;
      rowbytes = pInfo->w * 4;
    }
    else
    if (((fh[2] & 7) == 1) && (fh[1] == 1) && (fh[7] == 24))
      pInfo->t = 3;

    compr = fh[2] & 8;

    if (pInfo->t >= 0)
    {
      if ((pInfo->p = (unsigned char *)malloc(pInfo->h * rowbytes)) != NULL)
      {
        if (fh[0] != 0)
          fseek( f, fh[0], SEEK_CUR );

        if (fh[1] == 1)
        {
          unsigned int start = fh[3] + fh[4]*256;
          unsigned int size  = fh[5] + fh[6]*256;

          for (i=start; i<start+size && i<256; i++)
          {
            if (fread(&col, 1, 3, f) != 3) return 5;
            pInfo->pl[i].r = col[2];
            pInfo->pl[i].g = col[1];
            pInfo->pl[i].b = col[0];
          }
          pInfo->ps = i;

          if (start+size > 256)
            fseek(f, (start+size-256)*3, SEEK_CUR);
        }

        if ((fh[17] & 0x20) == 0)
          row_ptr = pInfo->p+(pInfo->h-1)*rowbytes;
        else
          row_ptr = pInfo->p;

        for (j=0; j<pInfo->h; j++)
        {
          if (compr == 0)
          {
            if (pInfo->t == 6)
            {
              for (i=0; i<pInfo->w; i++)
              {
                if (fread(&col, 1, 4, f) != 4) return 5;
                row_ptr[i*4]   = col[2];
                row_ptr[i*4+1] = col[1];
                row_ptr[i*4+2] = col[0];
                row_ptr[i*4+3] = col[3];
              }
            }
            else
            if (pInfo->t == 2)
            {
              for (i=0; i<pInfo->w; i++)
              {
                if (fread(&col, 1, 3, f) != 3) return 5;
                row_ptr[i*3]   = col[2];
                row_ptr[i*3+1] = col[1];
                row_ptr[i*3+2] = col[0];
              }
            }
            else
              if (fread(row_ptr, 1, rowbytes, f) != rowbytes) return 5;
          }
          else
          {
            i = 0;
            while (i<pInfo->w)
            {
              if (fread(&c, 1, 1, f) != 1) return 5;
              n = (c & 0x7F)+1;

              if ((c & 0x80) != 0)
              {
                if (pInfo->t == 6)
                {
                  if (fread(&col, 1, 4, f) != 4) return 5;
                  for (k=0; k<n; k++)
                  {
                    row_ptr[(i+k)*4]   = col[2];
                    row_ptr[(i+k)*4+1] = col[1];
                    row_ptr[(i+k)*4+2] = col[0];
                    row_ptr[(i+k)*4+3] = col[3];
                  }
                }
                else
                if (pInfo->t == 2)
                {
                  if (fread(&col, 1, 3, f) != 3) return 5;
                  for (k=0; k<n; k++)
                  {
                    row_ptr[(i+k)*3]   = col[2];
                    row_ptr[(i+k)*3+1] = col[1];
                    row_ptr[(i+k)*3+2] = col[0];
                  }
                }
                else
                {
                  if (fread(&col, 1, 1, f) != 1) return 5;
                  memset(row_ptr+i, col[0], n);
                }
              }
              else
              {
                if (pInfo->t == 6)
                {
                  for (k=0; k<n; k++)
                  {
                    if (fread(&col, 1, 4, f) != 4) return 5;
                    row_ptr[(i+k)*4]   = col[2];
                    row_ptr[(i+k)*4+1] = col[1];
                    row_ptr[(i+k)*4+2] = col[0];
                    row_ptr[(i+k)*4+3] = col[3];
                  }
                }
                else
                if (pInfo->t == 2)
                {
                  for (k=0; k<n; k++)
                  {
                    if (fread(&col, 1, 3, f) != 3) return 5;
                    row_ptr[(i+k)*3]   = col[2];
                    row_ptr[(i+k)*3+1] = col[1];
                    row_ptr[(i+k)*3+2] = col[0];
                  }
                }
                else
                  if (fread(row_ptr+i, 1, n, f) != n) return 5;
              }
              i+=n;
            }
          }
          if ((fh[17] & 0x20) == 0)
            row_ptr -= rowbytes;
          else
            row_ptr += rowbytes;
        }
      }
      else
        res = 4;
    }
    else
      res = 3;

    fclose(f);
  }
  else
    res = 1;

  return res;
}

void write_chunk(FILE * f, const char * name, unsigned char * data, unsigned int length)
{
  unsigned int crc = crc32(0, Z_NULL, 0);
  unsigned int len = swap32(length);

  fwrite(&len, 1, 4, f);
  fwrite(name, 1, 4, f);
  crc = crc32(crc, (const Bytef *)name, 4);

  if (memcmp(name, "fdAT", 4) == 0)
  {
    unsigned int seq = swap32(next_seq_num++);
    fwrite(&seq, 1, 4, f);
    crc = crc32(crc, (const Bytef *)(&seq), 4);
    length -= 4;
  }

  if (data != NULL && length > 0)
  {
    fwrite(data, 1, length, f);
    crc = crc32(crc, data, length);
  }

  crc = swap32(crc);
  fwrite(&crc, 1, 4, f);
}

void write_IDATs(FILE * f, int frame, unsigned char * data, unsigned int length, unsigned int idat_size)
{
  unsigned int z_cmf = data[0];
  if ((z_cmf & 0x0f) == 8 && (z_cmf & 0xf0) <= 0x70)
  {
    if (length >= 2)
    {
      unsigned int z_cinfo = z_cmf >> 4;
      unsigned int half_z_window_size = 1 << (z_cinfo + 7);
      while (idat_size <= half_z_window_size && half_z_window_size >= 256)
      {
        z_cinfo--;
        half_z_window_size >>= 1;
      }
      z_cmf = (z_cmf & 0x0f) | (z_cinfo << 4);
      if (data[0] != (unsigned char)z_cmf)
      {
        data[0] = (unsigned char)z_cmf;
        data[1] &= 0xe0;
        data[1] += (unsigned char)(0x1f - ((z_cmf << 8) + data[1]) % 0x1f);
      }
    }
  }

  while (length > 0)
  {
    unsigned int ds = length;
    if (ds > 32768)
      ds = 32768;

    if (frame == 0)
      write_chunk(f, "IDAT", data, ds);
    else
      write_chunk(f, "fdAT", data, ds+4);

    data += ds;
    length -= ds;
  }
}

unsigned int get_rect(unsigned int w, unsigned int h, unsigned char *pimg1, unsigned char *pimg2, unsigned char *ptemp, unsigned int *px, unsigned int *py, unsigned int *pw, unsigned int *ph, unsigned int bpp, unsigned int has_tcolor, unsigned int tcolor)
{
  unsigned int   i, j;
  unsigned int   x_min = w-1;
  unsigned int   y_min = h-1;
  unsigned int   x_max = 0;
  unsigned int   y_max = 0;
  unsigned int   diffnum = 0;
  unsigned int   over_is_possible = 1;

  if (!has_tcolor)
    over_is_possible = 0;

  if (bpp == 1)
  {
    unsigned char *pa = pimg1;
    unsigned char *pb = pimg2;
    unsigned char *pc = ptemp;

    for (j=0; j<h; j++)
    for (i=0; i<w; i++)
    {
      unsigned char c = *pb++;
      if (*pa++ != c)
      {
        diffnum++;
        if ((has_tcolor) && (c == tcolor)) over_is_possible = 0;
        if (i<x_min) x_min = i;
        if (i>x_max) x_max = i;
        if (j<y_min) y_min = j;
        if (j>y_max) y_max = j;
      }
      else
        c = tcolor;

      *pc++ = c;
    }
  }
  else
  if (bpp == 2)
  {
    unsigned short *pa = (unsigned short *)pimg1;
    unsigned short *pb = (unsigned short *)pimg2;
    unsigned short *pc = (unsigned short *)ptemp;

    for (j=0; j<h; j++)
    for (i=0; i<w; i++)
    {
      unsigned int c1 = *pa++;
      unsigned int c2 = *pb++;
      if ((c1 != c2) && ((c1>>8) || (c2>>8)))
      {
        diffnum++;
        if ((c2 >> 8) != 0xFF) over_is_possible = 0;
        if (i<x_min) x_min = i;
        if (i>x_max) x_max = i;
        if (j<y_min) y_min = j;
        if (j>y_max) y_max = j;
      }
      else
        c2 = 0;

      *pc++ = c2;
    }
  }
  else
  if (bpp == 3)
  {
    unsigned char *pa = pimg1;
    unsigned char *pb = pimg2;
    unsigned char *pc = ptemp;

    for (j=0; j<h; j++)
    for (i=0; i<w; i++)
    {
      unsigned int c1 = (((pa[2]<<8)+pa[1])<<8)+pa[0];
      unsigned int c2 = (((pb[2]<<8)+pb[1])<<8)+pb[0];
      if (c1 != c2)
      {
        diffnum++;
        if ((has_tcolor) && (c2 == tcolor)) over_is_possible = 0;
        if (i<x_min) x_min = i;
        if (i>x_max) x_max = i;
        if (j<y_min) y_min = j;
        if (j>y_max) y_max = j;
      }
      else
        c2 = tcolor;

      memcpy(pc, &c2, 3);
      pa += 3;
      pb += 3;
      pc += 3;
    }
  }
  else
  if (bpp == 4)
  {
    unsigned int *pa = (unsigned int *)pimg1;
    unsigned int *pb = (unsigned int *)pimg2;
    unsigned int *pc = (unsigned int *)ptemp;

    for (j=0; j<h; j++)
    for (i=0; i<w; i++)
    {
      unsigned int c1 = *pa++;
      unsigned int c2 = *pb++;
      if ((c1 != c2) && ((c1>>24) || (c2>>24)))
      {
        diffnum++;
        if ((c2 >> 24) != 0xFF) over_is_possible = 0;
        if (i<x_min) x_min = i;
        if (i>x_max) x_max = i;
        if (j<y_min) y_min = j;
        if (j>y_max) y_max = j;
      }
      else
        c2 = 0;

      *pc++ = c2;
    }
  }

  if (diffnum == 0)
  {
    *px = *py = 0;
    *pw = *ph = 1; 
  }
  else
  {
    *px = x_min;
    *py = y_min;
    *pw = x_max-x_min+1;
    *ph = y_max-y_min+1;
  }

  return over_is_possible;
}

void deflate_rect(unsigned char *pdata, int x, int y, int w, int h, int bpp, int stride, int zbuf_size, int n)
{
  int i, j, v;
  int a, b, c, pa, pb, pc, p;
  int rowbytes = w * bpp;
  unsigned char * prev = NULL;
  unsigned char * row  = pdata + y*stride + x*bpp;
  unsigned char * out;

  op[n*2].valid = 1;
  op[n*2].zstream.next_out = op[n*2].zbuf;
  op[n*2].zstream.avail_out = zbuf_size;

  op[n*2+1].valid = 1;
  op[n*2+1].zstream.next_out = op[n*2+1].zbuf;
  op[n*2+1].zstream.avail_out = zbuf_size;

  for (j=0; j<h; j++)
  {
    unsigned int    sum = 0;
    unsigned char * best_row = row_buf;
    unsigned int    mins = ((unsigned int)(-1)) >> 1;

    out = row_buf+1;
    for (i=0; i<rowbytes; i++)
    {
      v = out[i] = row[i];
      sum += (v < 128) ? v : 256 - v;
    }
    mins = sum;

    sum = 0;
    out = sub_row+1;
    for (i=0; i<bpp; i++)
    {
      v = out[i] = row[i];
      sum += (v < 128) ? v : 256 - v;
    }
    for (i=bpp; i<rowbytes; i++)
    {
      v = out[i] = row[i] - row[i-bpp];
      sum += (v < 128) ? v : 256 - v;
      if (sum > mins) break;
    }
    if (sum < mins)
    {
      mins = sum;
      best_row = sub_row;
    }

    if (prev)
    {
      sum = 0;
      out = up_row+1;
      for (i=0; i<rowbytes; i++)
      {
        v = out[i] = row[i] - prev[i];
        sum += (v < 128) ? v : 256 - v;
        if (sum > mins) break;
      }
      if (sum < mins)
      {
        mins = sum;
        best_row = up_row;
      }

      sum = 0;
      out = avg_row+1;
      for (i=0; i<bpp; i++)
      {
        v = out[i] = row[i] - prev[i]/2;
        sum += (v < 128) ? v : 256 - v;
      }
      for (i=bpp; i<rowbytes; i++)
      {
        v = out[i] = row[i] - (prev[i] + row[i-bpp])/2;
        sum += (v < 128) ? v : 256 - v;
        if (sum > mins) break;
      }
      if (sum < mins)
      { 
        mins = sum;
        best_row = avg_row;
      }

      sum = 0;
      out = paeth_row+1;
      for (i=0; i<bpp; i++)
      {
        v = out[i] = row[i] - prev[i];
        sum += (v < 128) ? v : 256 - v;
      }
      for (i=bpp; i<rowbytes; i++)
      {
        a = row[i-bpp];
        b = prev[i];
        c = prev[i-bpp];
        p = b - c;
        pc = a - c;
        pa = abs(p);
        pb = abs(pc);
        pc = abs(p + pc);
        p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;
        v = out[i] = row[i] - p;
        sum += (v < 128) ? v : 256 - v;
        if (sum > mins) break;
      }
      if (sum < mins)
      {
        best_row = paeth_row;
      }
    }

    op[n*2].zstream.next_in = row_buf;
    op[n*2].zstream.avail_in = rowbytes + 1;
    deflate(&op[n*2].zstream, Z_NO_FLUSH);

    op[n*2+1].zstream.next_in = best_row;
    op[n*2+1].zstream.avail_in = rowbytes + 1;
    deflate(&op[n*2+1].zstream, Z_NO_FLUSH);

    prev = row;
    row += stride;
  }

  deflate(&op[n*2].zstream, Z_FINISH);
  deflate(&op[n*2+1].zstream, Z_FINISH);

  op[n*2].x = op[n*2+1].x = x;
  op[n*2].y = op[n*2+1].y = y;
  op[n*2].w = op[n*2+1].w = w;
  op[n*2].h = op[n*2+1].h = h;
}

int main(int argc, char** argv)
{
  char           * szOut;
  char           * szImage;
  char           * szOption;
  char             szFormat[256];
  char             szNext[256];
  unsigned int     i, j, k;
  unsigned int     width, height, len;
  unsigned int     x0, y0, w0, h0;
  unsigned int     x1, y1, w1, h1, try_over;
  unsigned int     bpp, rowbytes, imagesize, coltype;
  unsigned int     idat_size, zbuf_size, zsize;
  unsigned int     has_tcolor, tcolor, colors;
  unsigned int     frames, loops, cur, first;
  unsigned char    dop, bop, r, g, b, a;
  int              c;
  rgb              palette[256];
  unsigned char    trns[256];
  unsigned int     palsize, trnssize;
  unsigned char    cube[4096];
  unsigned char    gray[256];
  unsigned char  * zbuf;
  unsigned char  * sp;
  unsigned char  * dp;
  FILE           * f;
  unsigned char  * img_temp;
  image_info     * img;
  char           * szExt;
  unsigned char  * dst;
  short   delay_num = -1;
  short   delay_den = -1;
  int     input_ext = 0;
  int     keep_palette = 0;
  int     keep_coltype = 0;

  printf("\nAPNG Assembler 2.7\n\n");

  if (argc <= 2)
  {
    printf("Usage   : apngasm output.png frame001.png [options]\n"
           "          apngasm output.png frame*.png   [options]\n\n"
           "Options :\n"
           "1 10    : frame delay is 1/10 sec. (default)\n"
           "/l2     : 2 loops (default is 0, forever)\n"
           "/f      : skip the first frame\n"
           "/kp     : keep palette\n"
           "/kc     : keep color type\n");
    return 1;
  }

  szOut   = argv[1];
  szImage = argv[2];
  cur = 0;
  first = 0;
  loops = 0;
  coltype = 6;

  for (c=3; c<argc; c++)
  {
    szOption = argv[c];

    if (szOption[0] == '/' || szOption[0] == '-')
    {
      if (szOption[1] == 'f' || szOption[1] == 'F')
        first = 1;
      else
      if (szOption[1] == 'l' || szOption[1] == 'L')
        loops = atoi(szOption+2);
      else
      if (szOption[1] == 'k' || szOption[1] == 'K')
      {
        if (szOption[2] == 'p' || szOption[2] == 'P')
          keep_palette = 1;
        else
        if (szOption[2] == 'c' || szOption[2] == 'C')
          keep_coltype = 1;
      }
    }
    else
    {
      short n = atoi(szOption);
      if ((n != 0) || (strcmp(szOption, "0") == 0))
      {
        if (delay_num == -1) delay_num = n;
        else
        if (delay_den == -1) delay_den = n;
      }
    }
  }

  if (delay_num <= 0) delay_num = 1;
  if (delay_den <= 0) delay_den = 10;

  len = strlen(szImage);
  szExt = szImage + len - 4;

  if ((len>4) && (szExt[0]=='.') && (szExt[1]=='p' || szExt[1]=='P') && (szExt[2]=='n' || szExt[2]=='N') && (szExt[3]=='g' || szExt[3]=='G'))
    input_ext = 1;

  if ((len>4) && (szExt[0]=='.') && (szExt[1]=='t' || szExt[1]=='T') && (szExt[2]=='g' || szExt[2]=='G') && (szExt[3]=='a' || szExt[3]=='A'))
    input_ext = 2;

  if (input_ext == 0)
  {
    printf( "Error: '.png' or '.tga' extension expected\n" );
    return 1;
  }

  if (*(szExt-1) == '*')
  {
    f = 0;
    for (i=1; i<6; i++)
    {
      strcpy(szFormat, szImage);
      sprintf(szFormat+len-5, "%%0%dd%%s", i);
      cur = 0;
      sprintf(szNext, szFormat, cur, szExt);
      if ((f = fopen(szNext, "rb")) != 0) break;
      cur = 1;
      sprintf(szNext, szFormat, cur, szExt);
      if ((f = fopen(szNext, "rb")) != 0) break;
    }

    if (f != 0)
      fclose(f);
    else
    {
      printf( "Error: *%s sequence not found\n", szExt );
      return 1;
    }
  }
  else
  {
    for (i=0; i<6; i++)
    {
      if (szImage == szExt-i) break;
      if (*(szExt-i-1) < '0') break;
      if (*(szExt-i-1) > '9') break;
    }

    if (i == 0)
    {
      printf( "Error: *%s sequence not found\n", szExt );
      return 1;
    }
    cur = atoi(szExt-i);
    strcpy(szFormat, szImage);
    sprintf(szFormat+len-4-i, "%%0%dd%%s", i);
    strcpy(szNext, szImage);
  }

  frames = 0;

  if ((f = fopen(szNext, "rb")) == 0)
  {
    printf("Error: can't open the file '%s'", szNext);
    return 1;
  }

  do
  {
    frames++;
    fclose(f);
    sprintf(szNext, szFormat, cur+frames, szExt);
    f = fopen(szNext, "rb");
  } 
  while (f != 0);

  img = (image_info *)malloc(frames*sizeof(image_info));
  if (img == NULL)
  {
    printf( "Error: not enough memory\n" );
    return 1;
  }

  memset(&cube, 0, sizeof(cube));
  memset(&gray, 0, sizeof(gray));

  for (i=0; i<256; i++)
  {
    col[i].num = 0;
    col[i].r = col[i].g = col[i].b = i;
    col[i].a = trns[i] = 255;
  }

  for (i=0; i<frames; i++)
  {
    int res;

    sprintf(szNext, szFormat, cur+i, szExt);
    printf("reading %s (%d of %d)\n", szNext, i-first+1, frames-first);

    if (input_ext == 1)
      res = LoadPNG(szNext, &img[i]);
    else
      res = LoadTGA(szNext, &img[i]);

    if (res)
    {
      printf("Error: Load%s(%s) failed\n", (input_ext == 1) ? "PNG" : "TGA", szNext);
      return res;
    }

    for (c=img[i].ps; c<256; c++)
      img[i].pl[c].r = img[i].pl[c].g = img[i].pl[c].b = c;

    for (c=img[i].ts; c<256; c++)
      img[i].tr[c] = 255;

    img[i].num = delay_num;
    img[i].den = delay_den;

    sprintf(szNext, szFormat, cur+i, ".txt");
    f = fopen(szNext, "rt");
    if (f != 0)
    {
      char  szStr[256];
      int   d1, d2;
      if (fgets(szStr, 256, f) != NULL)
      {
        res = sscanf(szStr, "delay=%d/%d", &d1, &d2);
        if (res == 2)
        {
         if (d1 != 0) img[i].num = d1; else img[i].num = 1;
         if (d2 != 0) img[i].den = d2; else img[i].den = 10;
        } 
      }
      fclose(f);
    }

    if (img[0].w != img[i].w || img[0].h != img[i].h)
    {
      printf("Error at %s: different image size\n", szNext);
      return 1;
    }

    if (i == 0)
      coltype = img[0].t;
    else
    if (img[0].ps != img[i].ps || memcmp(img[0].pl, img[i].pl, img[0].ps*3) != 0)
      coltype = 6;
    else
    if (img[0].ts != img[i].ts || memcmp(img[0].tr, img[i].tr, img[0].ts) != 0)
      coltype = 6;
    else
    if (img[i].t != 3)
    {
      if (coltype != 3)
        coltype |= img[i].t;
      else
        coltype = 6;
    }
    else
      if (coltype != 3)
        coltype = 6;
  }

  width = img[0].w;
  height = img[0].h;

  /* Upconvert to common coltype - start */
  for (i=0; i<frames; i++)
  {
    if (coltype == 6 && img[i].t != 6)
    {
      dst = (unsigned char *)malloc(width*height*4);
      if (dst == NULL)
      {
        printf( "Error: not enough memory\n" );
        return 1;
      }
      if (img[i].t == 0)
      {
        sp = img[i].p;
        dp = dst;
        if (img[i].ts == 0)
        for (j=0; j<width*height; j++)
        {
          *dp++ = *sp;
          *dp++ = *sp;
          *dp++ = *sp++;
          *dp++ = 255;
        }
        else
        for (j=0; j<width*height; j++)
        {
          g = *sp++;
          *dp++ = g;
          *dp++ = g;
          *dp++ = g;
          *dp++ = (img[i].tr[1] == g) ? 0 : 255;
        }
      }
      else
      if (img[i].t == 2)
      {
        sp = img[i].p;
        dp = dst;
        if (img[i].ts == 0)
        for (j=0; j<width*height; j++)
        {
          *dp++ = *sp++;
          *dp++ = *sp++;
          *dp++ = *sp++;
          *dp++ = 255;
        }
        else
        for (j=0; j<width*height; j++)
        {
          r = *sp++;
          g = *sp++;
          b = *sp++;
          *dp++ = r;
          *dp++ = g;
          *dp++ = b;
          *dp++ = (img[i].tr[1] == r && img[i].tr[3] == g && img[i].tr[5] == b) ? 0 : 255;
        }
      }
      else
      if (img[i].t == 3)
      {
        sp = img[i].p;
        dp = dst;
        for (j=0; j<width*height; j++, sp++)
        {
          *dp++ = img[i].pl[*sp].r;
          *dp++ = img[i].pl[*sp].g;
          *dp++ = img[i].pl[*sp].b;
          *dp++ = img[i].tr[*sp];
        }
      }
      else
      if (img[i].t == 4)
      {
        sp = img[i].p;
        dp = dst;
        for (j=0; j<width*height; j++)
        {
          *dp++ = *sp;
          *dp++ = *sp;
          *dp++ = *sp++;
          *dp++ = *sp++;
        }
      }
      free(img[i].p);
      img[i].p = dst;
    }
    else
    if (coltype == 4 && img[i].t == 0)
    {
      dst = (unsigned char *)malloc(width*height*2);
      if (dst == NULL)
      {
        printf( "Error: not enough memory\n" );
        return 1;
      }
      sp = img[i].p;
      dp = dst;
      for (j=0; j<width*height; j++)
      {
        *dp++ = *sp++;
        *dp++ = 255;
      }
      free(img[i].p);
      img[i].p = dst;
    }
    else
    if (coltype == 2 && img[i].t == 0)
    {
      dst = (unsigned char *)malloc(width*height*3);
      if (dst == NULL)
      {
        printf( "Error: not enough memory\n" );
        return 1;
      }
      sp = img[i].p;
      dp = dst;
      for (j=0; j<width*height; j++)
      {
        *dp++ = *sp;
        *dp++ = *sp;
        *dp++ = *sp++;
      }
      free(img[i].p);
      img[i].p = dst;
    }
  }
  /* Upconvert to common coltype - end */

  /* Dirty transparency optimization - start */
  if (coltype == 6)
  {
    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++, sp+=4)
        if (sp[3] == 0)
           sp[0] = sp[1] = sp[2] = 0;
    }
  }
  else
  if (coltype == 4)
  {
    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++, sp+=2)
        if (sp[1] == 0)
          sp[0] = 0;
    }
  }
  /* Dirty transparency optimization - end */

  /* Downconvert optimizations - start */
  has_tcolor = 0;
  palsize = trnssize = 0;
  colors = 0;

  if (coltype == 6 && !keep_coltype)
  {
    int transparent = 255;
    int simple_trans = 1;
    int grayscale = 1;

    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++)
      {
        r = *sp++;
        g = *sp++;
        b = *sp++;
        a = *sp++;
        transparent &= a;

        if (a != 0)
        {
          if (a != 255)
            simple_trans = 0;
          else
            if (((r | g | b) & 15) == 0)
              cube[(r<<4) + g + (b>>4)] = 1;

          if (r != g || g != b)
            grayscale = 0;
          else
            gray[r] = 1;
        }

        if (colors <= 256)
        {
          int found = 0;
          for (k=0; k<colors; k++)
          if (col[k].r == r && col[k].g == g && col[k].b == b && col[k].a == a)
          {
            found = 1;
            col[k].num++;
            break;
          }
          if (found == 0)
          {
            if (colors < 256)
            {
              col[colors].num++;
              col[colors].r = r;
              col[colors].g = g;
              col[colors].b = b;
              col[colors].a = a;
              if (a == 0) has_tcolor = 1;
            }
            colors++;
          }
        }
      }
    }

    if (grayscale && simple_trans && colors<=256) /* 6 -> 0 */
    {
      coltype = 0;

      for (i=0; i<256; i++)
      if (gray[i] == 0)
      {
        trns[0] = 0;
        trns[1] = i;
        trnssize = 2;
        break;
      }

      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          r = *sp++;
          g = *sp++;
          b = *sp++;
          if (*sp++ == 0) 
            *dp++ = trns[1];
          else
            *dp++ = g;
        }
      }
    }
    else
    if (colors<=256)   /* 6 -> 3 */
    {
      coltype = 3;

      if (has_tcolor==0 && colors<256)
        col[colors++].a = 0;
        
      qsort(&col[0], colors, sizeof(COLORS), cmp_colors);

      palsize = colors;
      for (i=0; i<colors; i++)
      {
        palette[i].r = col[i].r;
        palette[i].g = col[i].g;
        palette[i].b = col[i].b;
        trns[i]      = col[i].a;
        if (trns[i] != 255) trnssize = i+1;
      }

      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          r = *sp++;
          g = *sp++;
          b = *sp++;
          a = *sp++;
          for (k=0; k<colors; k++)
            if (col[k].r == r && col[k].g == g && col[k].b == b && col[k].a == a)
              break;
          *dp++ = k;
        }
      }
    }
    else
    if (grayscale)     /* 6 -> 4 */
    {
      coltype = 4;
      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          r = *sp++;
          g = *sp++;
          *dp++ = *sp++;
          *dp++ = *sp++;
        }
      }
    }
    else
    if (simple_trans)  /* 6 -> 2 */
    {
      for (i=0; i<4096; i++)
      if (cube[i] == 0)
      {
        trns[0] = 0;
        trns[1] = (i>>4)&0xF0;
        trns[2] = 0;
        trns[3] = i&0xF0;
        trns[4] = 0;
        trns[5] = (i<<4)&0xF0;
        trnssize = 6;
        break;
      }
      if (transparent == 255)
      {
        coltype = 2;
        for (i=0; i<frames; i++)
        {
          sp = dp = img[i].p;
          for (j=0; j<width*height; j++)
          {
            *dp++ = *sp++;
            *dp++ = *sp++;
            *dp++ = *sp++;
            sp++;
          }
        }
      }
      else
      if (trnssize != 0)
      {
        coltype = 2;
        for (i=0; i<frames; i++)
        {
          sp = dp = img[i].p;
          for (j=0; j<width*height; j++)
          {
            r = *sp++;
            g = *sp++;
            b = *sp++;
            a = *sp++;
            if (a == 0)
            {
              *dp++ = trns[1];
              *dp++ = trns[3];
              *dp++ = trns[5];
            }
            else
            {
              *dp++ = r;
              *dp++ = g;
              *dp++ = b;
            }
          }
        }
      }
    }
  }
  else
  if (coltype == 2)
  {
    int grayscale = 1;

    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++)
      {
        r = *sp++;
        g = *sp++;
        b = *sp++;

        if (img[i].ts == 0)
          if (((r | g | b) & 15) == 0)
            cube[(r<<4) + g + (b>>4)] = 1;

        if (r != g || g != b)
          grayscale = 0;
        else
          gray[r] = 1;

        if (colors <= 256)
        {
          int found = 0;
          for (k=0; k<colors; k++)
          if (col[k].r == r && col[k].g == g && col[k].b == b)
          {
            found = 1;
            col[k].num++;
            break;
          }
          if (found == 0)
          {
            if (colors < 256)
            {
              col[colors].num++;
              col[colors].r = r;
              col[colors].g = g;
              col[colors].b = b;
              if (img[i].ts == 6 && img[i].tr[1] == r && img[i].tr[3] == g && img[i].tr[5] == b)
              {
                col[colors].a = 0;
                has_tcolor = 1;
              }
            }
            colors++;
          }
        }
      }
    }

    if (grayscale && colors<=256 && !keep_coltype) /* 2 -> 0 */
    {
      for (i=0; i<256; i++)
      if (gray[i] == 0)
      {
        trns[0] = 0;
        trns[1] = i;
        trnssize = 2;
        break;
      }
      if (img[0].ts == 0)
      {
        coltype = 0;
        for (i=0; i<frames; i++)
        {
          sp = dp = img[i].p;
          for (j=0; j<width*height; j++, sp+=3)
            *dp++ = *sp;
        }
      }
      else
      if (trnssize != 0)
      {
        coltype = 0;
        for (i=0; i<frames; i++)
        {
          sp = dp = img[i].p;
          for (j=0; j<width*height; j++)
          {
            r = *sp++;
            g = *sp++;
            b = *sp++;
            if (img[i].tr[1] == r && img[i].tr[3] == g && img[i].tr[5] == b)
              *dp++ = trns[1];
            else
              *dp++ = g;
          }
        }
      }
    }
    else
    if (colors<=256 && !keep_coltype)   /* 2 -> 3 */
    {
      coltype = 3;

      if (has_tcolor==0 && colors<256)
        col[colors++].a = 0;
        
      qsort(&col[0], colors, sizeof(COLORS), cmp_colors);

      palsize = colors;
      for (i=0; i<colors; i++)
      {
        palette[i].r = col[i].r;
        palette[i].g = col[i].g;
        palette[i].b = col[i].b;
        trns[i]      = col[i].a;
        if (trns[i] != 255) trnssize = i+1;
      }

      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          r = *sp++;
          g = *sp++;
          b = *sp++;

          for (k=0; k<colors; k++)
            if (col[k].r == r && col[k].g == g && col[k].b == b)
              break;
          *dp++ = k;
        }
      }
    }
    else /* 2 -> 2 */
    {
      if (img[0].ts != 0)
      {
        memcpy(trns, img[0].tr, img[0].ts);
        trnssize = img[0].ts;
      }
      else
      for (i=0; i<4096; i++)
      if (cube[i] == 0)
      {
        trns[0] = 0;
        trns[1] = (i>>4)&0xF0;
        trns[2] = 0;
        trns[3] = i&0xF0;
        trns[4] = 0;
        trns[5] = (i<<4)&0xF0;
        trnssize = 6;
        break;
      }
    }
  }
  else
  if (coltype == 4 && !keep_coltype)
  {
    int simple_trans = 1;

    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++)
      {
        g = *sp++;
        a = *sp++;

        if (a != 0) 
        {
          if (a != 255)
            simple_trans = 0;
          else
            gray[g] = 1;
        }

        if (colors <= 256)
        {
          int found = 0;
          for (k=0; k<colors; k++)
          if (col[k].g == g && col[k].a == a)
          {
            found = 1;
            col[k].num++;
            break;
          }
          if (found == 0)
          {
            if (colors < 256)
            {
              col[colors].num++;
              col[colors].r = g;
              col[colors].g = g;
              col[colors].b = g;
              col[colors].a = a;
              if (a == 0) has_tcolor = 1;
            }
            colors++;
          }
        }
      }
    }

    if (simple_trans && colors<=256)   /* 4 -> 0 */
    {
      coltype = 0;

      for (i=0; i<256; i++)
      if (gray[i] == 0)
      {
        trns[0] = 0;
        trns[1] = i;
        trnssize = 2;
        break;
      }

      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          g = *sp++;
          if (*sp++ == 0) 
            *dp++ = trns[1];
          else
            *dp++ = g;
        }
      }
    }
    else
    if (colors<=256)   /* 4 -> 3 */
    {
      coltype = 3;

      if (has_tcolor==0 && colors<256)
        col[colors++].a = 0;
        
      qsort(&col[0], colors, sizeof(COLORS), cmp_colors);

      palsize = colors;
      for (i=0; i<colors; i++)
      {
        palette[i].r = col[i].r;
        palette[i].g = col[i].g;
        palette[i].b = col[i].b;
        trns[i]      = col[i].a;
        if (trns[i] != 255) trnssize = i+1;
      }

      for (i=0; i<frames; i++)
      {
        sp = dp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          g = *sp++;
          a = *sp++;
          for (k=0; k<colors; k++)
            if (col[k].g == g && col[k].a == a)
              break;
          *dp++ = k;
        }
      }
    }
  }
  else
  if (coltype == 3)
  {
    int simple_trans = 1;
    int grayscale = 1;

    for (c=0; c<img[0].ps; c++)
    {
      col[c].r = img[0].pl[c].r;
      col[c].g = img[0].pl[c].g;
      col[c].b = img[0].pl[c].b;
      col[c].a = img[0].tr[c];
    }

    for (i=0; i<frames; i++)
    {
      sp = img[i].p;
      for (j=0; j<width*height; j++)
        col[*sp++].num++;
    }

    for (i=0; i<256; i++)
    if (col[i].num != 0)
    {
      colors = i+1;
      if (col[i].a != 0)
      {
        if (col[i].a != 255)
          simple_trans = 0;
        else
        if (col[i].r != col[i].g || col[i].g != col[i].b)
          grayscale = 0;
        else
          gray[col[i].g] = 1;
      }
      else
        has_tcolor = 1;
    }
    
    if (grayscale && simple_trans && !keep_coltype) /* 3 -> 0 */
    {
      for (i=0; i<256; i++)
      if (gray[i] == 0)
      {
        trns[0] = 0;
        trns[1] = i;
        trnssize = 2;
        break;
      }
      if (!has_tcolor)
      {
        coltype = 0;
        for (i=0; i<frames; i++)
        {
          sp = img[i].p;
          for (j=0; j<width*height; j++, sp++)
            *sp = img[i].pl[*sp].g;
        }
      }
      else
      if (trnssize != 0)
      {
        coltype = 0;
        for (i=0; i<frames; i++)
        {
          sp = img[i].p;
          for (j=0; j<width*height; j++, sp++)
          {
            if (col[*sp].a == 0)
              *sp = trns[1];
            else
              *sp = img[i].pl[*sp].g;
          }
        }
      }
    }
    else
    if (!keep_palette)                 /* 3 -> 3 */
    {
      for (i=0; i<256; i++)
      if (col[i].num == 0)
      {
        col[i].a = 255;
        if (!has_tcolor)
        {
          col[i].a = 0;
          has_tcolor = 1;
        }
      }

      qsort(&col[0], 256, sizeof(COLORS), cmp_colors);

      for (i=0; i<256; i++)
      {
        palette[i].r = col[i].r;
        palette[i].g = col[i].g;
        palette[i].b = col[i].b;
        trns[i]      = col[i].a;
        if (col[i].num != 0)
          palsize = i+1;
        if (trns[i] != 255) 
          trnssize = i+1;
      }

      for (i=0; i<frames; i++)
      {
        sp = img[i].p;
        for (j=0; j<width*height; j++)
        {
          r = img[i].pl[*sp].r;
          g = img[i].pl[*sp].g;
          b = img[i].pl[*sp].b;
          a = img[i].tr[*sp];

          for (k=0; k<palsize; k++)
            if (col[k].r == r && col[k].g == g && col[k].b == b && col[k].a == a)
              break;
          *sp++ = k;
        }
      }
    }
    else
    {
      palsize = img[0].ps;
      trnssize = img[0].ts;
      for (i=0; i<palsize; i++)
      {
        palette[i].r = col[i].r;
        palette[i].g = col[i].g;
        palette[i].b = col[i].b;
      }
      for (i=0; i<trnssize; i++)
        trns[i] = col[i].a;
    }
  }
  else
  if (coltype == 0)  /* 0 -> 0 */
  {
    if (img[0].ts != 0)
    {
      memcpy(trns, img[0].tr, img[0].ts);
      trnssize = img[0].ts;
    }
    else
    {
      for (i=0; i<frames; i++)
      {
        sp = img[i].p;
        for (j=0; j<width*height; j++)
          gray[*sp++] = 1;
      }
      for (i=0; i<256; i++)
      if (gray[i] == 0)
      {
        trns[0] = 0;
        trns[1] = i;
        trnssize = 2;
        break;
      }
    }
  }
  /* Downconvert optimizations - end */

  bpp = 1;
  if (coltype == 2)
    bpp = 3;
  else
  if (coltype == 4)
    bpp = 2;
  else
  if (coltype == 6)
    bpp = 4;

  has_tcolor = 0;
  if (coltype == 0)
  {
    if (trnssize)
    {
      has_tcolor = 1;
      tcolor = trns[1];
    }
  }
  else
  if (coltype == 2)
  {
    if (trnssize)
    {
      has_tcolor = 1;
      tcolor = (((trns[5]<<8)+trns[3])<<8)+trns[1];
    }
  }
  else
  if (coltype == 3)
  {
    for (i=0; i<trnssize; i++)
    if (trns[i] == 0)
    {
      has_tcolor = 1;
      tcolor = i;
      break;
    }
  }
  else
  {
    has_tcolor = 1;
    tcolor = 0;
  }

  rowbytes  = width * bpp;
  imagesize = rowbytes * height;
  idat_size = (rowbytes + 1) * height;
  zbuf_size = idat_size + ((idat_size + 7) >> 3) + ((idat_size + 63) >> 6) + 11;

  for (i=0; i<12; i++)
  {
    op[i].zstream.data_type = Z_BINARY;
    op[i].zstream.zalloc = Z_NULL;
    op[i].zstream.zfree = Z_NULL;
    op[i].zstream.opaque = Z_NULL;

    if (i & 1)
      deflateInit2(&op[i].zstream, Z_BEST_COMPRESSION, 8, 15, 8, Z_FILTERED);
    else
      deflateInit2(&op[i].zstream, Z_BEST_COMPRESSION, 8, 15, 8, Z_DEFAULT_STRATEGY);

    op[i].zbuf = (unsigned char *)malloc(zbuf_size);
    if (op[i].zbuf == NULL)
    {
      printf( "Error: not enough memory\n" );
      return 1;
    }
  }

  img_temp = (unsigned char *)malloc(imagesize);
  zbuf = (unsigned char *)malloc(zbuf_size);
  row_buf = (unsigned char *)malloc(rowbytes + 1);
  sub_row = (unsigned char *)malloc(rowbytes + 1);
  up_row = (unsigned char *)malloc(rowbytes + 1);
  avg_row = (unsigned char *)malloc(rowbytes + 1);
  paeth_row = (unsigned char *)malloc(rowbytes + 1);

  if (img_temp && zbuf && row_buf && sub_row && up_row && avg_row && paeth_row)
  {
    row_buf[0] = 0;
    sub_row[0] = 1;
    up_row[0] = 2;
    avg_row[0] = 3;
    paeth_row[0] = 4;
  }
  else
  {
    printf( "Error: not enough memory\n" );
    return 1;
  }

  if ((f = fopen(szOut, "wb")) != 0)
  {
    struct IHDR 
    {
      unsigned int    mWidth;
      unsigned int    mHeight;
      unsigned char   mDepth;
      unsigned char   mColorType;
      unsigned char   mCompression;
      unsigned char   mFilterMethod;
      unsigned char   mInterlaceMethod;
    } ihdr = { swap32(width), swap32(height), 8, coltype, 0, 0, 0 };

    struct acTL 
    {
      unsigned int    mFrameCount;
      unsigned int    mLoopCount;
    } actl = { swap32(frames-first), swap32(loops) };

    struct fcTL 
    {
      unsigned int    mSeq;
      unsigned int    mWidth;
      unsigned int    mHeight;
      unsigned int    mXOffset;
      unsigned int    mYOffset;
      unsigned short  mDelayNum;
      unsigned short  mDelayDen;
      unsigned char   mDisposeOp;
      unsigned char   mBlendOp;
    } fctl;

    fwrite(png_sign, 1, 8, f);

    write_chunk(f, "IHDR", (unsigned char *)(&ihdr), 13);

    if (frames > 1)
      write_chunk(f, "acTL", (unsigned char *)(&actl), 8);
    else
      first = 0;

    if (palsize > 0)
      write_chunk(f, "PLTE", (unsigned char *)(&palette), palsize*3);

    if (trnssize > 0)
      write_chunk(f, "tRNS", trns, trnssize);

    x0 = 0;
    y0 = 0;
    w0 = width;
    h0 = height;
    bop = 0;

    printf("saving %s (frame %d of %d)\n", szOut, 1-first, frames-first);
    deflate_rect(img[0].p, x0, y0, w0, h0, bpp, rowbytes, zbuf_size, 0);

    if (op[0].zstream.total_out <= op[1].zstream.total_out)
    {
      zsize = op[0].zstream.total_out;
      memcpy(zbuf, op[0].zbuf, zsize);
    }
    else
    {
      zsize = op[1].zstream.total_out;
      memcpy(zbuf, op[1].zbuf, zsize);
    }

    deflateReset(&op[0].zstream);
    op[0].zstream.data_type = Z_BINARY;
    deflateReset(&op[1].zstream);
    op[1].zstream.data_type = Z_BINARY;

    if (first)
    {
      write_IDATs(f, 0, zbuf, zsize, idat_size);

      printf("saving %s (frame %d of %d)\n", szOut, 1, frames-first);
      deflate_rect(img[1].p, x0, y0, w0, h0, bpp, rowbytes, zbuf_size, 0);

      if (op[0].zstream.total_out <= op[1].zstream.total_out)
      {
        zsize = op[0].zstream.total_out;
        memcpy(zbuf, op[0].zbuf, zsize);
      }
      else
      {
        zsize = op[1].zstream.total_out;
        memcpy(zbuf, op[1].zbuf, zsize);
      }

      deflateReset(&op[0].zstream);
      op[0].zstream.data_type = Z_BINARY;
      deflateReset(&op[1].zstream);
      op[1].zstream.data_type = Z_BINARY;
    }

    for (i=first; i<frames-1; i++)
    {
      unsigned int op_min;
      int          op_best;

      printf("saving %s (frame %d of %d)\n", szOut, i-first+2, frames-first);
      for (j=0; j<12; j++)
        op[j].valid = 0;

      /* dispose = none */
      try_over = get_rect(width, height, img[i].p, img[i+1].p, img_temp, &x1, &y1, &w1, &h1, bpp, has_tcolor, tcolor);
      deflate_rect(img[i+1].p, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 0);
      if (try_over)
        deflate_rect(img_temp, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 1);

      /* dispose = background */
      if (has_tcolor)
      {
        memcpy(img_temp, img[i].p, imagesize);
        if (coltype == 2)
          for (j=0; j<h0; j++)
            for (k=0; k<w0; k++)
              memcpy(img_temp + ((j+y0)*width + (k+x0))*3, &tcolor, 3);
        else
          for (j=0; j<h0; j++)
            memset(img_temp + ((j+y0)*width + x0)*bpp, tcolor, w0*bpp);

        try_over = get_rect(width, height, img_temp, img[i+1].p, img_temp, &x1, &y1, &w1, &h1, bpp, has_tcolor, tcolor);

        deflate_rect(img[i+1].p, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 2);
        if (try_over)
          deflate_rect(img_temp, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 3);
      }

      if (i > first)
      {
        /* dispose = previous */
        try_over = get_rect(width, height, img[i-1].p, img[i+1].p, img_temp, &x1, &y1, &w1, &h1, bpp, has_tcolor, tcolor);
        deflate_rect(img[i+1].p, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 4);
        if (try_over)
          deflate_rect(img_temp, x1, y1, w1, h1, bpp, rowbytes, zbuf_size, 5);
      }

      op_min = op[0].zstream.total_out;
      op_best = 0;
      for (j=1; j<12; j++)
      {
        if (op[j].valid)
        {
          if (op[j].zstream.total_out < op_min)
          {
            op_min = op[j].zstream.total_out;
            op_best = j;
          }
        }
      }

      dop = op_best >> 2;

      fctl.mSeq       = swap32(next_seq_num++);
      fctl.mWidth     = swap32(w0);
      fctl.mHeight    = swap32(h0);
      fctl.mXOffset   = swap32(x0);
      fctl.mYOffset   = swap32(y0);
      fctl.mDelayNum  = swap16(img[i].num);
      fctl.mDelayDen  = swap16(img[i].den);
      fctl.mDisposeOp = dop;
      fctl.mBlendOp   = bop;
      write_chunk(f, "fcTL", (unsigned char *)(&fctl), 26);

      write_IDATs(f, i, zbuf, zsize, idat_size);

      /* process apng dispose - begin */
      if (dop == 1)
      {
        if (coltype == 2)
          for (j=0; j<h0; j++)
            for (k=0; k<w0; k++)
              memcpy(img[i].p + ((j+y0)*width + (k+x0))*3, &tcolor, 3);
        else
          for (j=0; j<h0; j++)
            memset(img[i].p + ((j+y0)*width + x0)*bpp, tcolor, w0*bpp);
      }
      else
      if (dop == 2)
      {
        for (j=0; j<h0; j++)
          memcpy(img[i].p + ((j+y0)*width + x0)*bpp, img[i-1].p + ((j+y0)*width + x0)*bpp, w0*bpp);
      }
      /* process apng dispose - end */

      x0 = op[op_best].x;
      y0 = op[op_best].y;
      w0 = op[op_best].w;
      h0 = op[op_best].h;
      bop = (op_best >> 1) & 1;

      zsize = op[op_best].zstream.total_out;
      memcpy(zbuf, op[op_best].zbuf, zsize);

      for (j=0; j<12; j++)
      {
        deflateReset(&op[j].zstream);
        op[j].zstream.data_type = Z_BINARY;
      }
    }

    if (frames > 1)
    {
      fctl.mSeq       = swap32(next_seq_num++);
      fctl.mWidth     = swap32(w0);
      fctl.mHeight    = swap32(h0);
      fctl.mXOffset   = swap32(x0);
      fctl.mYOffset   = swap32(y0);
      fctl.mDelayNum  = swap16(img[i].num);
      fctl.mDelayDen  = swap16(img[i].den);
      fctl.mDisposeOp = 0;
      fctl.mBlendOp   = bop;
      write_chunk(f, "fcTL", (unsigned char *)(&fctl), 26);
    }

    write_IDATs(f, i, zbuf, zsize, idat_size);

    write_chunk(f, "tEXt", png_Software, 27); 
    write_chunk(f, "IEND", 0, 0);
    fclose(f);
  }
  else
  {
    printf( "Error: couldn't open file for writing\n" );
    return 1;
  }

  for (i=0; i<12; i++)
  {
    deflateEnd(&op[i].zstream);
    if (op[i].zbuf != NULL)
      free(op[i].zbuf);
  }

  free(img_temp);
  free(zbuf);
  free(row_buf);
  free(sub_row);
  free(up_row);
  free(avg_row);
  free(paeth_row);

  for (i=0; i<frames; i++)
  {
    if (img[i].p != NULL)
      free(img[i].p);
  }
  free(img);

  printf("all done\n");

  return 0;
}
