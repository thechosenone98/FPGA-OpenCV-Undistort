// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XUNDISTORT_COORDINATE_GENERATOR_H
#define XUNDISTORT_COORDINATE_GENERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xundistort_coordinate_generator_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XUndistort_coordinate_generator_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XUndistort_coordinate_generator;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XUndistort_coordinate_generator_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XUndistort_coordinate_generator_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XUndistort_coordinate_generator_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XUndistort_coordinate_generator_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XUndistort_coordinate_generator_Initialize(XUndistort_coordinate_generator *InstancePtr, u16 DeviceId);
XUndistort_coordinate_generator_Config* XUndistort_coordinate_generator_LookupConfig(u16 DeviceId);
int XUndistort_coordinate_generator_CfgInitialize(XUndistort_coordinate_generator *InstancePtr, XUndistort_coordinate_generator_Config *ConfigPtr);
#else
int XUndistort_coordinate_generator_Initialize(XUndistort_coordinate_generator *InstancePtr, const char* InstanceName);
int XUndistort_coordinate_generator_Release(XUndistort_coordinate_generator *InstancePtr);
#endif


void XUndistort_coordinate_generator_Set_fx_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_fx_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_fx_inverse_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_fx_inverse_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_fy_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_fy_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_fy_inverse_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_fy_inverse_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_cx_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_cx_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_cy_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_cy_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k1_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k2_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k3_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k3_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k4_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k4_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k5_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k5_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_k6_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_k6_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_p1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_p1_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_p2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_p2_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_s1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_s1_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_s2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_s2_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_s3_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_s3_in(XUndistort_coordinate_generator *InstancePtr);
void XUndistort_coordinate_generator_Set_s4_in(XUndistort_coordinate_generator *InstancePtr, u32 Data);
u32 XUndistort_coordinate_generator_Get_s4_in(XUndistort_coordinate_generator *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
