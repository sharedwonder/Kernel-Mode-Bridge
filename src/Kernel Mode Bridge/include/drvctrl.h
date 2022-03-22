// Copyright (c) 2022 Liu Baihao. All rights reserved.
// This software is licensed under MIT License.

#pragma once

#include "kmbridge.h"

EXTERN_C HANDLE KME_Device;

#define KME_DEVICE_CONTROL(controlCode, input, statusRef) \
    DeviceIoControl(KME_Device, controlCode, &input, sizeof(input), NULL, 0, (LPDWORD) (statusRef), NULL)

#define INLINE_CALL_KERNEL_API(name, setupInput) \
    ARGS_ ## name input; \
    setupInput \
    NTSTATUS status;\
    KME_DEVICE_CONTROL((name), input, &status); \
    return status; \
