#ifndef _CONF_H_
#define _CONF_H_

#define APNG_WRITE_SUPPORTED
#define APNG_READ_SUPPORTED
#define APNG_SPECS_SUPPORTED

#ifndef APNGASM_DECLSPEC
  #ifdef _WINDOWS
    #ifdef apngasm_EXPORTS
      #define APNGASM_DECLSPEC __declspec(dllexport)
    #else
      #define APNGASM_DECLSPEC __declspec(dllimport)
    #endif
  #else
    #define APNGASM_DECLSPEC
  #endif
#endif

#endif

