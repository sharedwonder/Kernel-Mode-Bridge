// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#include "settings.h"

#include "framework.h"

#define ACCESS_CHECK_MODE 1
#define ALLOW_UNLOAD_DRIVER TRUE
#define ALLOW_OTHER_PROGRAMS_MODIFY_SETTINGS TRUE

static HANDLE keyHandle;

static inline VOID SetDefaultValues(_Out_ DriverSettings *driverSettings) {
    driverSettings->AccessCheckMode = ACCESS_CHECK_MODE;
    driverSettings->AllowUnloadDriver = ALLOW_UNLOAD_DRIVER;
    driverSettings->AllowOtherProgramsModifySettings = ALLOW_OTHER_PROGRAMS_MODIFY_SETTINGS;
}

VOID DriverInitSettings(_Out_ DriverSettings *driverSettings) {
    OBJECT_ATTRIBUTES attrib;
    UNICODE_STRING keyPath;

    SetDefaultValues(driverSettings);

    RtlInitUnicodeString(&keyPath, L"\\Registry\\Machine\\SOFTWARE\\Kernel Mode Bridge\\Kernel Mode Entry\\Settings");

    InitializeObjectAttributes(&attrib, &keyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    ZwOpenKey(&keyHandle, KEY_READ, &attrib);

    DriverLoadSettings(driverSettings);
}

VOID DriverLoadSettings(_Out_ DriverSettings *driverSettings) {
    NTSTATUS status = STATUS_SUCCESS;

    UNICODE_STRING valueName;
    KEY_VALUE_PARTIAL_INFORMATION valueInfo;
    ULONG size;

#define GET_SETTING(name, convertType, cType, keyValueType) \
    if (keyHandle != NULL) { \
        PCWSTR src = L ## #name; \
        RtlInitUnicodeString(&valueName, src); \
        status = ZwQueryValueKey(keyHandle, &valueName, KeyValuePartialInformation, &valueInfo, sizeof(valueInfo), &size); \
        if (NT_SUCCESS(status) && size != 0) { \
            status = ZwQueryValueKey(keyHandle, &valueName, KeyValuePartialInformation, &valueInfo, size, &size); \
            if (NT_SUCCESS(status) && valueInfo.Type == keyValueType && valueInfo.DataLength == sizeof(cType)) { \
                driverSettings->name = (convertType) *((cType *) valueInfo.Data); \
            } \
        } \
    } \
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, \
               "[KernelModeEntry: DriverLoadSettings] Settings:\n\t" #name " (" #convertType "): %d.\n", \
               driverSettings->name))

    GET_SETTING(AccessCheckMode, UCHAR, ULONG, REG_DWORD);
    GET_SETTING(AllowUnloadDriver, BOOLEAN, ULONG, REG_DWORD);
    GET_SETTING(AllowOtherProgramsModifySettings, BOOLEAN, ULONG, REG_DWORD);

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "[KernelModeEntry: DriverLoadSettings] The driver loaded settings.\n"));
}

VOID DriverSaveSettings(_In_ DriverSettings *driverSettings) {
    UNREFERENCED_PARAMETER(driverSettings);
}

VOID DriverResetSettings(_Out_opt_ DriverSettings *driverSettings) {
    if (driverSettings != NULL) {
        SetDefaultValues(driverSettings);
    }
}
