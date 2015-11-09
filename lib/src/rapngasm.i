%module RAPNGAsm

%{
#include "apngasm.h"
#include "apngframe.h"
%}

namespace apngasm {
    class APNGAsm
    {
		APNGAsm(void);
		~APNGAsm(void);
		const char* version(void) const;
    };

    class APNGFrame
    {
        APNGFrame();
    };
}
