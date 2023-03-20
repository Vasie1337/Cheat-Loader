#include "antidebug.h"

AntidebugError Antidebug::Check()
{
    __try
    {
        RaiseException(DBG_CONTROL_C, 0, 0, NULL);
        return AntidebugError::ANTI_DEBUG_DEBUGGER_FOUND;
    }
    __except (DBG_CONTROL_C == GetExceptionCode()
        ? EXCEPTION_EXECUTE_HANDLER
        : EXCEPTION_CONTINUE_SEARCH)
    {
        return AntidebugError::ANTI_DEBUG_NO_ERROR;
    }
}
