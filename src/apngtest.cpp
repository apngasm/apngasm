#include "apngasm.h"
#include <iostream>

#define RES 100

void Fill(unsigned char * p, int r1, int g1, int b1)
{
  for (int y=0; y<RES; y++)
  for (int x=0; x<RES; x++)
  {
    *p++ = r1;
    *p++ = g1;
    *p++ = b1;
  }
}

void Circle(unsigned char * p, int cx, int cy, int r, int r0, int g0, int b0)
{
  for (int y=0; y<RES; y++)
  for (int x=0; x<RES; x++)
  {
    int i = y*2 + 1 - cy;
    int j = x*2 + 1 - cx;

    if (i*i + j*j < r*r)
    {
      *p++ = r0;
      *p++ = g0;
      *p++ = b0;
    }
    else
      p += 3;
  }
}

int main(int argc, char* argv[])
{
  apngasm::APNGAsm assembler;
  std::cout << "Initializing assembler " << assembler.version() << std::endl;

  std::cout << "Test 1 - start" << std::endl;
  assembler.addFrame("gold01.png", 15, 100);
  assembler.addFrame("gold02.png", 15, 100);
  assembler.addFrame("gold03.png", 15, 100);
  assembler.assemble("out/gold_anim.png");
  std::cout << "frames=" << assembler.frameCount() << std::endl;
  std::cout << "Test 2 - finish" << std::endl;

  assembler.reset();

  std::cout << "Test 1 - start" << std::endl;
  assembler.addFrame("clock1.png", 1, 1);
  assembler.addFrame("clock2.png", 1, 1);
  assembler.addFrame("clock3.png", 1, 1);
  assembler.addFrame("clock4.png", 1, 1);
  assembler.assemble("out/clock_anim.png");
  std::cout << "frames=" << assembler.frameCount() << std::endl;
  std::cout << "Test 2 - finish" << std::endl;

  assembler.reset();

  std::cout << "Test 3 - start" << std::endl;
  std::vector<apngasm::APNGFrame> frames = assembler.disassemble("penguins.png");
  std::cout << frames.size() << " Frames" << std::endl;
  assembler.savePNGs("out");
  std::cout << "Test 3 - finish" << std::endl;

  assembler.reset();

  std::cout << "Test 4 - start" << std::endl;
  {
    unsigned char * pData=(unsigned char *)malloc(RES*RES*3);

    apngasm::rgb trans_color = {0, 48, 128};
    Fill(pData, 0, 48, 128);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES, RES-RES/5, RES/5, 255, 0, 0);
    apngasm::APNGFrame frame1 = apngasm::APNGFrame((apngasm::rgb *)pData, RES, RES, &trans_color, 50, 100);
    assembler.addFrame(frame1);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES+RES/5, RES, RES/5, 255, 0, 0);
    apngasm::APNGFrame frame2 = apngasm::APNGFrame((apngasm::rgb *)pData, RES, RES, &trans_color, 50, 100);
    assembler.addFrame(frame2);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES, RES+RES/5, RES/5, 255, 0, 0);
    apngasm::APNGFrame frame3 = apngasm::APNGFrame((apngasm::rgb *)pData, RES, RES, &trans_color, 50, 100);
    assembler.addFrame(frame3);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES-RES/5, RES, RES/5, 255, 0, 0);
    apngasm::APNGFrame frame4 = apngasm::APNGFrame((apngasm::rgb *)pData, RES, RES, &trans_color, 50, 100);
    assembler.addFrame(frame4);

    assembler.assemble("out/circle_anim.png");

    free(pData);
  }
  std::cout << "Test 4 - finish" << std::endl;

  return 0;
}
