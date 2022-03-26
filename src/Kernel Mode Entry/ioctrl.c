// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "ioctrl.h"

#include "framework.h"

NTSTATUS KME_DispatchCreate(_In_ PDEVICE_OBJECT driver, _In_ PIRP irp) {
    UNREFERENCED_PARAMETER(driver);

    NTSTATUS status = STATUS_SUCCESS;
    PRIVILEGE_SET requiredPrivileges;
    PIO_STACK_LOCATION irpStackLocation = IoGetCurrentIrpStackLocation(irp);
    PIO_SECURITY_CONTEXT securityContext = irpStackLocation->Parameters.Create.SecurityContext;

    requiredPrivileges.Control = PRIVILEGE_SET_ALL_NECESSARY;
    requiredPrivileges.PrivilegeCount = 1;
    requiredPrivileges.Privilege[0].Luid.HighPart = 0;
    requiredPrivileges.Privilege[0].Luid.LowPart = SE_DEBUG_PRIVILEGE;
    requiredPrivileges.Privilege[0].Attributes = 0;

    if (!SePrivilegeCheck(&requiredPrivileges,
        &(securityContext->AccessState->SubjectSecurityContext), irp->RequestorMode)) {
        status = STATUS_PRIVILEGE_NOT_HELD;
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                   "[KernelModeEntry: KME_DispatchCreate] The connection is rejected:"
                   "the client have no required privileges (SE_DEBUG_PRIVILEGE).\n"));
        goto End;
    }

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "[KernelModeEntry: KME_DispatchCreate] The client is successfully connected.\n"));

End:
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS KME_DispatchClose(_In_ PDEVICE_OBJECT driver, _In_ PIRP irp) {
    UNREFERENCED_PARAMETER(driver);

    NTSTATUS status = STATUS_SUCCESS;

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "[KernelModeEntry: KME_DispatchClose] The client is successfully disconnected.\n"));

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS KME_DispatchDeviceControl(_In_ PDEVICE_OBJECT driver, _In_ PIRP irp) {
    UNREFERENCED_PARAMETER(driver);

    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStackLocation = IoGetCurrentIrpStackLocation(irp);
    ULONG controlCode = irpStackLocation->Parameters.DeviceIoControl.IoControlCode;
    ULONG inputLength = irpStackLocation->Parameters.DeviceIoControl.InputBufferLength;
    PVOID originalInput = irp->AssociatedIrp.SystemBuffer;

    if (inputLength != 0 && originalInput == NULL) {
        status = STATUS_INVALID_BUFFER_SIZE;
        goto End;
    }

#define CTRL_CASE(name, function) \
    case (name): \
    { \
        PARAM_ ## name *input = (PARAM_ ## name *) originalInput; \
        if (inputLength > 0 && input == NULL) { \
            status = STATUS_INVALID_PARAMETER; \
            goto End; \
        } \
        if (inputLength != sizeof(*input)) { \
            status = STATUS_INFO_LENGTH_MISMATCH; \
            goto End; \
        } \
        status = (function); \
    } \
        break

    switch (controlCode) {
        CTRL_CASE(NT_CREATE_FILE, ZwCreateFile(input->FileHandle, input->DesiredAccess, input->ObjectAttributes,
                                               input->IoStatusBlock, input->AllocationSize, input->FileAttributes,
                                               input->ShareAccess, input->CreateDisposition, input->CreateOptions,
                                               input->EaBuffer, input->EaLength));
        CTRL_CASE(NT_OPEN_PROCESS, ZwOpenProcess(input->ProcessHandle, input->DesiredAccess, input->ObjectAttributes, input->ClientId));
        CTRL_CASE(NT_OPEN_PROCESS_TOKEN,
                  NtOpenProcessToken(input->ProcessHandle, input->DesiredAccess, input->TokenHandle));
        CTRL_CASE(NT_OPEN_PROCESS_TOKEN_EX,
                  ZwOpenProcessTokenEx(input->ProcessHandle, input->DesiredAccess, input->HandleAttributes, input->TokenHandle));
        CTRL_CASE(NT_TERMINATE_PROCESS, ZwTerminateProcess(input->ProcessHandle, input->ExitStatus));
        CTRL_CASE(NT_CLOSE, ZwClose(input->Handle));

    default: // Invalid control code.
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

#undef CTRL_CASE

End:
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = status;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}
