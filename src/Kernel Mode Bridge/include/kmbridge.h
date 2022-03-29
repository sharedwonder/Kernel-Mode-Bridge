// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#pragma once

/*
 * In the C language, when you include the header file "windows.h",
 * and if the macro MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
 * is not defined as 0, the compiler will display the warning C5105 in compiling.
 * But don't need to do this in C++ language.
 *
 * This problem appears in the Windows SDK 10.0.19041 and later.
 * I tried to reinstall the Windows SDK, but it didn't have any effect.
 */
#ifndef __cplusplus
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif // !__cplusplus

#include <windows.h>

#include "kmtypes.h"

#ifdef KERNEL_MODE_BRIDGE_EXPORTS
#define KERNEL_API __declspec(dllexport)
#else
#define KERNEL_API __declspec(dllimport)
#endif // KERNEL_MODE_BRIDGE_EXPORTS

#define WINNT_API KERNEL_API

EXTERN_C_START

typedef HMODULE KernelModeHostLibrary;

// Native NT kernel functions.

KERNEL_API NTSTATUS NTAPI NtCreateFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER AllocationSize,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
    _In_ ULONG EaLength
);

KERNEL_API NTSTATUS NTAPI NtOpenProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
);

KERNEL_API NTSTATUS NTAPI NtOpenProcessToken(
    _In_ HANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PHANDLE TokenHandle
);

KERNEL_API NTSTATUS NTAPI NtOpenProcessTokenEx(
    _In_ HANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _Out_ PHANDLE TokenHandle
);

KERNEL_API NTSTATUS NTAPI NtTerminateProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ UINT ExitStatus
);

KERNEL_API NTSTATUS NTAPI NtClose(
    _In_ HANDLE Handle
);

EXTERN_C_END
