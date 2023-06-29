#ifndef _WINDOWS_
#include <windows.h>
#endif

#ifdef FLUSHDNS_USE_CMD_WORKAROUND
BOOL DnsFlushResolverCache(void) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // Start the child process. 
    BOOL run_success = CreateProcess(NULL, TEXT("ipconfig /flushdns"),
                                     NULL, NULL, FALSE, 0,
                                     NULL, NULL, &si, &pi);
    if (!run_success) return FALSE;

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}
#else
#include <windns.h>
#endif