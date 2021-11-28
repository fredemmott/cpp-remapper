// (c) Eric Korff de Gidts
// SPDX-License-Identifier: MIT
// Logging.h
#pragma once

// The define for ProjectDirLength is passed from the project file to the source code via a define
static_assert((sizeof(__FILE__) > ProjectDirLength), "Ensure any source code is located in a subdirectory of the project");

// Macros for tracing
#define TRACE_DETAILED(message)    { }
#define TRACE_PERFORMANCE(message) { }
#define TRACE_ALWAYS(message)      { }

class LogException {
    public:
    static std::string EncodeWIN32(const char* kind, DWORD result) {
        char buf[255];
        snprintf(buf, sizeof(buf), "%s: failed with WIN32 code 0x%x", kind, result);
        return buf;
    }
};

#define ETW(x) #x

// Macros for throwing exceptions
#define THROW_CONFIGRET(result) { throw std::runtime_error(LogException::EncodeCONFIGRET(ETW(Exception), result));            }
#define THROW_HRESULT(result)   { throw std::runtime_error(LogException::EncodeHRESULT  (ETW(Exception), result));            }
#define THROW_NTSTATUS(result)  { throw std::runtime_error(LogException::EncodeNTSTATUS (ETW(Exception), result));            }
#define THROW_WIN32(result)     { throw std::runtime_error(LogException::EncodeWIN32    (ETW(Exception), result));            }
#define THROW_WIN32_LAST_ERROR  { throw std::runtime_error(LogException::EncodeWIN32    (ETW(Exception), ::GetLastError()));  }

// Macros for logging from within an exception handler
#define LOGEXC_AND_CONTINUE              { LogException(LogException::ExceptionMessage()).Log(ETW(Exception));                         }
#define LOGEXC_AND_RETURN                { LogException(LogException::ExceptionMessage()).Log(ETW(Exception)); return;                 }
#define LOGEXC_AND_RETURN_RESULT(result) { LogException(LogException::ExceptionMessage()).Log(ETW(Exception)); return (result);        }
#define LOGEXC_AND_RETURN_CONFIGRET      { return (LogException(LogException::ExceptionMessage()).Log(ETW(Exception)).ToCONFIGRET());  }
#define LOGEXC_AND_RETURN_HRESULT        { return (LogException(LogException::ExceptionMessage()).Log(ETW(Exception)).ToHRESULT());    }
#define LOGEXC_AND_RETURN_NTSTATUS       { return (LogException(LogException::ExceptionMessage()).Log(ETW(Exception)).ToNTSTATUS());   }
#define LOGEXC_AND_RETURN_WIN32          { return (LogException(LogException::ExceptionMessage()).Log(ETW(Exception)).ToWIN32());      }
