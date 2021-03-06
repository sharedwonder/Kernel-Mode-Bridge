// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "kmloader.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        return KernelModeAttach();

    case DLL_PROCESS_DETACH:
        KernelModeDetach();

    default:
        break;
    }

    return TRUE;
}
