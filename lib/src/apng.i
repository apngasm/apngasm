%
    module APNG

    // For java.
    % pragma(java) jniclasscode = % {
  static { NativeLibLoader.load(); }
  %
}

// For ruby.
#ifdef SWIGRUBY

// Replace init method name.
%
    {
#define Init_APNG Init_RAPNGAsm
        % }

    // Convert array.
    % typecheck(SWIG_TYPECHECK_POINTER) apngasm::rgb *pixels,
    apngasm::rgb *trns_color, apngasm::rgba *pixels {
  $1 = ((TYPE($input) == T_ARRAY) && (TYPE(rb_ary_entry($input, 0)) == T_DATA))
           ? 1
           : 0;
}

% typemap(in) apngasm::rgb *pixels, apngasm::rgb *trns_color,
    apngasm::rgba *pixels {
  Check_Type($input, T_ARRAY);
  int size = RARRAY_LEN($input);
  $1 = ($1_type)malloc(size * sizeof($ * 1_type));
  for (int i = 0; i < size; ++i) {
    VALUE inst = rb_ary_entry($input, i);
    Check_Type(inst, T_DATA);
    $1_type element = NULL;
    Data_Get_Struct(inst, $ * 1_type, element);
    $1[i] = *element;
  }
}

% typemap(freearg) apngasm::rgb *pixels, apngasm::rgb *trns_color,
    apngasm::rgba *pixels {
  if ($1)
    free($1);
}

// Rename methods.
% rename(color_type) apngasm::APNGFrame::colorType;
% rename(palette_size) apngasm::APNGFrame::paletteSize;
% rename(transparency_size) apngasm::APNGFrame::transparencySize;
% rename(delay_numerator) apngasm::APNGFrame::delayNum;
% rename(delay_denominator) apngasm::APNGFrame::delayDen;

% rename(add_frame) apngasm::APNGAsm::addFrame(const APNGFrame &frame);
% rename(add_frame_file) apngasm::APNGAsm::addFrame(
      const std::string &filePath, unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
      unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
% rename(add_frame_rgb) apngasm::APNGAsm::addFrame(
      rgb *pixels, unsigned int width, unsigned int height,
      rgb *trns_color = NULL, unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
      unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
% rename(add_frame_rgba) apngasm::APNGAsm::addFrame(
      rgba *pixels, unsigned int width, unsigned int height,
      unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
      unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
% rename(save_pngs) apngasm::APNGAsm::savePNGs;
% rename(load_animation_spec) apngasm::APNGAsm::loadAnimationSpec;
% rename(save_json) apngasm::APNGAsm::saveJSON;
% rename(save_xml) apngasm::APNGAsm::saveXML;
% rename(set_loops) apngasm::APNGAsm::setLoops;
% rename(set_skip_first) apngasm::APNGAsm::setSkipFirst;
% rename(get_frames) apngasm::APNGAsm::getFrames;
% rename(get_loops) apngasm::APNGAsm::getLoops;
% rename(is_skip_first) apngasm::APNGAsm::isSkipFirst;
% rename(frame_count) apngasm::APNGAsm::frameCount;

% rename(Rgb) apngasm::rgb;
% rename(Rgba) apngasm::rgba;

#endif // SWIGRUBY

%
    {
#include "apngasm.h"
#include "apngframe.h"
        % }

    % include "std_string.i" % include "std_vector.i"

    % include "apngasm-conf.h"

    namespace apngasm {
  typedef struct {
    unsigned char r, g, b;
  } rgb;
  typedef struct {
    unsigned char r, g, b, a;
  } rgba;

  class APNGFrame {
  public:
    APNGFrame();
    APNGFrame(const std::string &filePath,
              unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
              unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
    //    APNGFrame(rgb *pixels, unsigned int width, unsigned int height,
    //    unsigned delayNum = DEFAULT_FRAME_NUMERATOR, unsigned delayDen =
    //    DEFAULT_FRAME_DENOMINATOR); APNGFrame(rgb *pixels, unsigned int width,
    //    unsigned int height, rgb *trns_color = NULL, unsigned delayNum =
    //    DEFAULT_FRAME_NUMERATOR, unsigned delayDen =
    //    DEFAULT_FRAME_DENOMINATOR);
    APNGFrame(rgba *pixels, unsigned int width, unsigned int height,
              unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
              unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);

    unsigned char *pixels(unsigned char *setPixels = NULL);
    unsigned int width(unsigned int setWidth = 0);
    unsigned int height(unsigned int setHeight = 0);
    unsigned char colorType(unsigned char setColorType = 255);
    rgb *palette(rgb *setPalette = NULL);
    unsigned char *transparency(unsigned char *setTransparency = NULL);
    int paletteSize(int setPaletteSize = 0);
    int transparencySize(int setTransparencySize = 0);
    unsigned int delayNum(unsigned int setDelayNum = 0);
    unsigned int delayDen(unsigned int setDelayDen = 0);
    unsigned char **rows(unsigned char **setRows = NULL);

    bool save(const std::string &outPath) const;
  };

  class APNGAsm {
  public:
    APNGAsm(void);
    APNGAsm(const std::vector<APNGFrame> &frames);
    ~APNGAsm(void);

    size_t addFrame(const std::string &filePath,
                    unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
                    unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
    size_t addFrame(const APNGFrame &frame);
    size_t addFrame(rgb *pixels, unsigned int width, unsigned int height,
                    rgb *trns_color = NULL,
                    unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
                    unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);
    size_t addFrame(rgba *pixels, unsigned int width, unsigned int height,
                    unsigned delayNum = DEFAULT_FRAME_NUMERATOR,
                    unsigned delayDen = DEFAULT_FRAME_DENOMINATOR);

#ifdef APNG_WRITE_SUPPORTED
    bool assemble(const std::string &outputPath);
#endif

#ifdef APNG_READ_SUPPORTED
    const std::vector<APNGFrame> &disassemble(const std::string &filePath);
    bool savePNGs(const std::string &outputDir) const;
#endif

#ifdef APNG_SPECS_SUPPORTED
    const std::vector<APNGFrame> &
    loadAnimationSpec(const std::string &filePath);
    bool saveJSON(const std::string &outputPath,
                  const std::string &imageDir = "") const;
    bool saveXML(const std::string &outputPath,
                 const std::string &imageDir = "") const;
#endif

    void setLoops(unsigned int loops = 0);
    void setSkipFirst(bool skipFirst);
    const std::vector<APNGFrame> &getFrames() const;
    unsigned int getLoops() const;
    bool isSkipFirst() const;
    size_t frameCount();
    size_t reset();
    const char *version(void) const;
  };
}

// Rename stl vector.
% template(APNGFrameVector) std::vector<apngasm::APNGFrame>;
