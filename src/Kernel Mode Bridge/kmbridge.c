// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "kmbridge.h"

#include "kmentry.h"
#include "drvctrl.h"

NTSTATUS NTAPI NtCreateFile(
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
) {
    INLINE_CALL_KERNEL_API(
        NT_CREATE_FILE, {
            input.FileHandle = FileHandle;
            input.DesiredAccess = DesiredAccess;
            input.ObjectAttributes = ObjectAttributes;
            input.IoStatusBlock = IoStatusBlock;
            input.AllocationSize = AllocationSize;
            input.FileAttributes = FileAttributes;
            input.ShareAccess = ShareAccess;
            input.CreateDisposition = CreateDisposition;
            input.CreateOptions = CreateOptions;
            input.EaBuffer = EaBuffer;
            input.EaLength = EaLength;
        }
    );
}

NTSTATUS NTAPI NtOpenProcess(_Out_ PHANDLE ProcessHandle, _In_ ACCESS_MASK DesiredAccess,
                             _In_  POBJECT_ATTRIBUTES ObjectAttributes, _In_opt_ PCLIENT_ID ClientId) {
    INLINE_CALL_KERNEL_API(
        NT_OPEN_PROCESS, {
            input.ProcessHandle = ProcessHandle;
            input.DesiredAccess = DesiredAccess;
            input.ObjectAttributes = ObjectAttributes;
            input.ClientId = ClientId;
        }
    );
}

NTSTATUS NTAPI NtOpenProcessToken(_In_ HANDLE ProcessHandle, _In_ ACCESS_MASK DesiredAccess,
                                  _Out_ PHANDLE TokenHandle) {
    INLINE_CALL_KERNEL_API(
        NT_OPEN_PROCESS_TOKEN, {
            input.ProcessHandle = ProcessHandle;
            input.DesiredAccess = DesiredAccess;
            input.TokenHandle = TokenHandle;
        }
    )
}

NTSTATUS NTAPI NtOpenProcessTokenEx(_In_ HANDLE ProcessHandle, _In_ ACCESS_MASK DesiredAccess,
                                    _In_ ULONG HandleAttributes, _Out_ PHANDLE TokenHandle) {
    INLINE_CALL_KERNEL_API(
        NT_OPEN_PROCESS_TOKEN_EX, {
            input.ProcessHandle = ProcessHandle;
            input.DesiredAccess = DesiredAccess;
            input.HandleAttributes = HandleAttributes;
            input.TokenHandle = TokenHandle;
        }
    )
}

NTSTATUS NTAPI NtTerminateProcess(_In_opt_ HANDLE ProcessHandle, _In_ UINT ExitStatus) {
    INLINE_CALL_KERNEL_API(
        NT_TERMINATE_PROCESS, {
            input.ProcessHandle = ProcessHandle;
            input.ExitStatus = ExitStatus;
        }
    );
}

NTSTATUS NTAPI NtClose(_In_ HANDLE Handle) {
    INLINE_CALL_KERNEL_API(
        NT_CLOSE, {
            input.Handle = Handle;
        }
    );
}
