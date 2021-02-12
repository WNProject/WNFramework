// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNUtilities/inc/WNCrashHandler.h"

#include <errno.h>
#include <fcntl.h>

#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

#include <stdio.h>

#if _WN_MSVC_MAJOR >= 19
#pragma warning(push)
#pragma warning(disable : 4091)  // ignores warnings in VS2015 with DbgHelp.
#endif

#include <DbgHelp.h>

#if _WN_MSVC_MAJOR >= 19
#pragma warning(pop)
#endif

namespace wn {
namespace utilities {
namespace {

typedef BOOL(__stdcall* tStackWalk)(_In_ DWORD MachineType,
    _In_ HANDLE hProcess, _In_ HANDLE hThread,
    _Inout_ LPSTACKFRAME64 StackFrame, _Inout_ PVOID ContextRecord,
    _In_opt_ PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
    _In_opt_ PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
    _In_opt_ PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
    _In_opt_ PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);

LPTOP_LEVEL_EXCEPTION_FILTER g_oldFilter = NULL;

LONG WINAPI exceptionFilter(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo) {
  HMODULE h = LoadLibrary("dbghelp.dll");
  tStackWalk mStackWalk = (tStackWalk)GetProcAddress(h, "StackWalk64");
  PFUNCTION_TABLE_ACCESS_ROUTINE64 sfta =
      (PFUNCTION_TABLE_ACCESS_ROUTINE64)GetProcAddress(
          h, "SymFunctionTableAccess64");
  PGET_MODULE_BASE_ROUTINE64 sbgm =
      (PGET_MODULE_BASE_ROUTINE64)GetProcAddress(h, "SymGetModuleBase64");

  if (h == 0 || !mStackWalk || !sfta || !sbgm || !ExceptionInfo) {
    return (g_oldFilter ? (*g_oldFilter)(ExceptionInfo)
                        : EXCEPTION_EXECUTE_HANDLER);
  }
  FILE* f;
  fopen_s(&f, "crash", "w+");
  if (f == NULL) {
    return (g_oldFilter ? (*g_oldFilter)(ExceptionInfo)
                        : EXCEPTION_EXECUTE_HANDLER);
  }

  _EXCEPTION_POINTERS p1 = ExceptionInfo[0];
  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();
  STACKFRAME64 stackFrame;
  ZeroMemory(&stackFrame, sizeof(STACKFRAME64));
#ifdef _WIN64
  stackFrame.AddrPC.Offset = p1.ContextRecord->Rip;
  stackFrame.AddrPC.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = p1.ContextRecord->Rsp;
  stackFrame.AddrStack.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = p1.ContextRecord->Rbp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
#else
  stackFrame.AddrPC.Offset = p1.ContextRecord->Eip;
  stackFrame.AddrPC.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = p1.ContextRecord->Esp;
  stackFrame.AddrStack.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = p1.ContextRecord->Ebp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
#endif
  fprintf(f, "%p\n", GetModuleHandle(NULL));

  while (TRUE == mStackWalk(IMAGE_FILE_MACHINE_I386, process, thread,
                     &stackFrame, p1.ContextRecord, NULL, sfta, sbgm, NULL)) {
    fprintf(f, "%p\n", reinterpret_cast<void*>(stackFrame.AddrPC.Offset));
  }
  fclose(f);

  return (
      g_oldFilter ? (*g_oldFilter)(ExceptionInfo) : EXCEPTION_EXECUTE_HANDLER);
}

}  // anonymous namespace

void initialize_crash_handler() {
  g_oldFilter = ::SetUnhandledExceptionFilter(&exceptionFilter);
}

}  // namespace utilities
}  // namespace wn
