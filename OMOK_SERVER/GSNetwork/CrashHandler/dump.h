#include <dbghelp.h>
#include "StackWalker.h"
#include "CrashHandler.h"

class OutdatedCrashHandler : public CrashHandler
{
public:
    OutdatedCrashHandler() = default;
    virtual ~OutdatedCrashHandler() = default;

    virtual BOOL Install() override;
    static LONG  Handle(LPEXCEPTION_POINTERS exptrs);
};
