// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "kmloader.h"

#include <tchar.h>
#include <detours.h>
#include <stdbool.h>

static HMODULE NtDll;
static HMODULE KernelModeBridgeDll;
static LPSTR Name[] = {
    "NtCreateFile",
    "NtOpenProcess",
    "NtOpenProcessToken",
    "NtOpenProcessTokenEx",
    "NtTerminateProcess"
    "NtClose"
};
static FARPROC Functions[sizeof(Name) / sizeof(LPSTR)];
static bool Attached = false;

BOOL KernelModeAttach() {
    if (Attached) {
        return TRUE;
    }

    NtDll = GetModuleHandle(TEXT("ntdll.dll"));
    if (NtDll == NULL) {
        return FALSE;
    }

    TCHAR path[MAX_PATH + 1];
    GetModuleFileName(GetModuleHandle(TEXT("KernelModeLoader.dll")), path, MAX_PATH);
    _tcsrchr(path, TEXT('\\'))[1] = '\0';
    _tcscat_s(path, MAX_PATH, TEXT("KernelModeBridge.dll"));

    KernelModeBridgeDll = LoadLibrary(path);
    if (KernelModeBridgeDll == NULL) {
        return FALSE;
    }

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (size_t index = 0; index < sizeof(Name) / sizeof(LPSTR); ++ index) {
        Functions[index] = GetProcAddress(NtDll, Name[index]);
        DetourAttach((PVOID *) Functions + index, (PVOID) GetProcAddress(KernelModeBridgeDll, Name[index]));
    }

    DetourTransactionCommit();

    return TRUE;
}

void KernelModeDetach() {
    if (!Attached) {
        return;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (size_t index = 0; index < sizeof(Name) / sizeof(LPSTR); ++ index) {
        DetourDetach((PVOID *) Functions + index, (PVOID) GetProcAddress(KernelModeBridgeDll, Name[index]));
    }

    DetourTransactionCommit();

    FreeLibrary(KernelModeBridgeDll);
}
