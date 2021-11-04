#include "apngasm.h"
#include <cstdlib>
#include <iostream>

#define RES 100

void Fill(unsigned char *p, int r1, int g1, int b1) {
  for (int y = 0; y < RES; y++)
    for (int x = 0; x < RES; x++) {
      *p++ = r1;
      *p++ = g1;
      *p++ = b1;
    }
}

void Circle(unsigned char *p, int cx, int cy, int r, int r0, int g0, int b0) {
  for (int y = 0; y < RES; y++)
    for (int x = 0; x < RES; x++) {
      int i = y * 2 + 1 - cy;
      int j = x * 2 + 1 - cx;

      if (i * i + j * j < r * r) {
        *p++ = r0;
        *p++ = g0;
        *p++ = b0;
      } else
        p += 3;
    }
}

int main(int argc, char *argv[]) {
  std::string samplePrefix = "./samples";

  if (argc > 1) {
    samplePrefix = argv[1];
    samplePrefix = samplePrefix + "/";
  }

  apngasm::APNGAsm assembler;
  std::cout << "=================================================="
            << std::endl;
  std::cout << "Running apngasm library tests for C++..." << std::endl;
  std::cout << "Initializing assembler: " << assembler.version() << std::endl;
  std::cout << "Sample prefix is: " << samplePrefix << std::endl;

#ifdef APNG_WRITE_SUPPORTED
  std::cout << "Test 1 [gold] - start" << std::endl;
  assembler.addFrame(samplePrefix + "gold01.png", 15, 100);
  assembler.addFrame(samplePrefix + "gold02.png", 15, 100);
  assembler.addFrame(samplePrefix + "gold03.png", 15, 100);
  assembler.assemble("cpp-out/gold_anim.png");
  assembler.assemble("cpp-out/gold_anim2.png");
  std::cout << "frames=" << assembler.frameCount() << std::endl;
  std::cout << "Test 1 - finish" << std::endl;

  assembler.reset();

  std::cout << "Test 2 [clock] - start" << std::endl;
  assembler.addFrame("clock1.png", 1, 1);
  assembler.addFrame("clock2.png", 1, 1);
  assembler.addFrame("clock3.png", 1, 1);
  assembler.addFrame("clock4.png", 1, 1);
  assembler.assemble("cpp-out/clock_anim.png");
  assembler.assemble("cpp-out/clock_anim2.png");
  std::cout << "frames=" << assembler.frameCount() << std::endl;
  std::cout << "Test 2 - finish" << std::endl;
#endif

#ifdef APNG_READ_SUPPORTED
  assembler.reset();

  std::cout << "Test 3 [penguins] - start" << std::endl;
  std::vector<apngasm::APNGFrame> frames =
      assembler.disassemble(samplePrefix + "penguins.png");
  std::cout << frames.size() << " Frames" << std::endl;
  assembler.savePNGs("./cpp-out");
#ifdef APNG_SPECS_SUPPORTED
  assembler.saveJSON("cpp-out/penguins.json", samplePrefix);
  assembler.saveXML("cpp-out/penguins.xml", samplePrefix);
#endif
  std::cout << "Test 3 - finish" << std::endl;
#endif

#ifdef APNG_WRITE_SUPPORTED
  assembler.reset();

  std::cout << "Test 4 [circle] - start" << std::endl;
  {
    unsigned char *pData = (unsigned char *)malloc(RES * RES * 3);

    apngasm::rgb trans_color = {0, 48, 128};
    Fill(pData, 0, 48, 128);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES, RES - RES / 5, RES / 5, 255, 0, 0);
    apngasm::APNGFrame frame1 = apngasm::APNGFrame((apngasm::rgb *)pData, RES,
                                                   RES, &trans_color, 50, 100);
    assembler.addFrame(frame1);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES + RES / 5, RES, RES / 5, 255, 0, 0);
    apngasm::APNGFrame frame2 = apngasm::APNGFrame((apngasm::rgb *)pData, RES,
                                                   RES, &trans_color, 50, 100);
    assembler.addFrame(frame2);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES, RES + RES / 5, RES / 5, 255, 0, 0);
    apngasm::APNGFrame frame3 = apngasm::APNGFrame((apngasm::rgb *)pData, RES,
                                                   RES, &trans_color, 50, 100);
    assembler.addFrame(frame3);

    Circle(pData, RES, RES, RES, 64, 212, 32);
    Circle(pData, RES - RES / 5, RES, RES / 5, 255, 0, 0);
    apngasm::APNGFrame frame4 = apngasm::APNGFrame((apngasm::rgb *)pData, RES,
                                                   RES, &trans_color, 50, 100);
    assembler.addFrame(frame4);

    assembler.assemble("cpp-out/circle_anim.png");
    assembler.assemble("cpp-out/circle_anim2.png");

    free(pData);
  }
  std::cout << "Test 4 - finish" << std::endl;
#endif

  assembler.reset();

  std::cout << "Test 5 [gold vector] - start" << std::endl;
  {
    apngasm::APNGFrame frame1 =
        apngasm::APNGFrame(samplePrefix + "gold01.png", 15, 100);
    apngasm::APNGFrame frame2 =
        apngasm::APNGFrame(samplePrefix + "gold02.png", 15, 100);
    std::vector<apngasm::APNGFrame> frames;
    frames.push_back(frame1);
    frames.push_back(frame2);
    apngasm::APNGAsm test5Assembler(frames);
    std::cout << "frames=" << test5Assembler.frameCount() << std::endl;
    test5Assembler.assemble("cpp-out/test5_anim.png");
  }
  std::cout << "Test 5 - finish" << std::endl;

  std::cout << "Test 6 [clock skip first] - start" << std::endl;
  {
    apngasm::APNGAsm skipFirstTestAssembler;
    skipFirstTestAssembler.setSkipFirst(true);
    skipFirstTestAssembler.addFrame(samplePrefix + "clk-first.png");
    skipFirstTestAssembler.addFrame(samplePrefix + "clock*");
    skipFirstTestAssembler.assemble("cpp-out/skip_first_test.png");

    skipFirstTestAssembler.reset();
    skipFirstTestAssembler.addFrame("cpp-out/skip_first_test.png");

    std::cout << "frames=" << skipFirstTestAssembler.frameCount()
              << ", isSkipFirst=" << skipFirstTestAssembler.isSkipFirst()
              << std::endl;
  }
  std::cout << "Test 6 - finish" << std::endl;

  // std::cout << "Memory leak test - start" << std::endl;
  // {
  //   for(int i = 0;  i < 1000;  ++i)
  //   {
  //     apngasm::APNGAsm memoryLeakTestAssembler;
  //     memoryLeakTestAssembler.addFrame(samplePrefix + "penguins.png");
  //     memoryLeakTestAssembler.assemble("out/memory_leak_test.png");
  //   }
  //   std::cout << "Press enter to continue...";
  //   std::cin.get();
  // }
  // std::cout << "Memory leak test - finish" << std::endl;

  std::cout << "Finished apngasm library C++ tests." << std::endl;
  std::cout
      << "Check the output in \"testing/cpp-out\" of your build directory."
      << std::endl;
  std::cout << "=================================================="
            << std::endl;

  return 0;
}
