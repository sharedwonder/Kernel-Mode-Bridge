// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "kmloader.h"

static HMODULE NtDll;
static HMODULE KernelModeBridgeDll;
static LPSTR Name[] = {"NtOpenProcess", "NtOpenProcessTokenEx", "NtTerminateProcess", "NtClose"};
static bool attached = false;

void KernelModeAttach() {
    if (attached) {
        return;
    }

    NtDll = GetModuleHandle(TEXT("ntdll.dll"));
    if (NtDll == NULL) {
        return;
    }
    KernelModeBridgeDll = LoadLibrary(TEXT("KernelModeBridge.dll"));
    if (KernelModeBridgeDll == NULL) {
        return;
    }

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (size_t index = 0; index < sizeof(Name) / sizeof(Name[0]); ++ index) {
        DetourAttach((PVOID *) GetProcAddress(NtDll, Name[index]),
                     (PVOID *) GetProcAddress(KernelModeBridgeDll, Name[index]));
    }

    DetourTransactionCommit();
}

void KernelModeDetach() {
    if (!attached) {
        return;
    }

    FreeLibrary(KernelModeBridgeDll);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (size_t index = 0; index < sizeof(Name) / sizeof(Name[0]); ++ index) {
        DetourDetach((PVOID *) GetProcAddress(NtDll, Name[index]),
                     (PVOID *) GetProcAddress(KernelModeBridgeDll, Name[index]));
    }

    DetourTransactionCommit();
}
