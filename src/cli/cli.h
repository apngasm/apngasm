#ifndef INCLGRD_CLI_H_
#define INCLGRD_CLI_H_

#include "options.h"
#include "apngasm.h"

namespace apngasm { namespace listener { class IAPNGAsmListener; } }

namespace apngasm_cli {
	typedef enum ERRCODE_tag {
		ERRCODE_NOERRORS = 0,
		ERRCODE_HELP,
		ERRCODE_VERSION,
		ERRCODE_OUTPUTFILE_NOTSPECIFIED,
		ERRCODE_OUTPUTFILE_ALREADYEXISTS,
		ERRCODE_INVALIDARGUMENT,
		ERRCODE_ASSEMBLEERR
	} ERRCODE;

	class CLI {
	private:
		Options options;
		apngasm::APNGAsm assembler;
		apngasm::listener::IAPNGAsmListener *pListener;

	public:
		static const std::string VERSION;

		// arguments is main's
		CLI(int argc, char **argv);

		// finalize.
		~CLI();

		// 0=succeeded, other=err
		int start(void);

		int assemble(void);
		int disassemble(const std::string &src);
	};
}

#endif  // INCLGRD_CLI_H_
