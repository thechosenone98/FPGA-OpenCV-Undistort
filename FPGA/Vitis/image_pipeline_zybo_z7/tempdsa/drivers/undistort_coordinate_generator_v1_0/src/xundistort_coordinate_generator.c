// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xundistort_coordinate_generator.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XUndistort_coordinate_generator_CfgInitialize(XUndistort_coordinate_generator *InstancePtr, XUndistort_coordinate_generator_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XUndistort_coordinate_generator_Set_fx_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_fx_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_fx_inverse_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_INVERSE_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_fx_inverse_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_INVERSE_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_fy_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_fy_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_fy_inverse_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_INVERSE_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_fy_inverse_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_INVERSE_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_cx_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CX_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_cx_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CX_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_cy_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CY_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_cy_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CY_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K1_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k1_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K1_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K2_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k2_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K2_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k3_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K3_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k3_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K3_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k4_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K4_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k4_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K4_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k5_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K5_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k5_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K5_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_k6_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K6_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_k6_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K6_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_p1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P1_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_p1_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P1_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_p2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P2_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_p2_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P2_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_s1_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S1_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_s1_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S1_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_s2_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S2_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_s2_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S2_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_s3_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S3_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_s3_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S3_IN_DATA);
    return Data;
}

void XUndistort_coordinate_generator_Set_s4_in(XUndistort_coordinate_generator *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUndistort_coordinate_generator_WriteReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S4_IN_DATA, Data);
}

u32 XUndistort_coordinate_generator_Get_s4_in(XUndistort_coordinate_generator *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUndistort_coordinate_generator_ReadReg(InstancePtr->Control_BaseAddress, XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S4_IN_DATA);
    return Data;
}

