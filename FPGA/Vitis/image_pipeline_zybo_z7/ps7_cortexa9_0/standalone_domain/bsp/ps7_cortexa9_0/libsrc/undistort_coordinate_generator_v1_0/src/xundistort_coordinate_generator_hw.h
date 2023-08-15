// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of fx_in
//        bit 31~0 - fx_in[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of fx_inverse_in
//        bit 31~0 - fx_inverse_in[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of fy_in
//        bit 31~0 - fy_in[31:0] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of fy_inverse_in
//        bit 31~0 - fy_inverse_in[31:0] (Read/Write)
// 0x2c : reserved
// 0x30 : Data signal of cx_in
//        bit 31~0 - cx_in[31:0] (Read/Write)
// 0x34 : reserved
// 0x38 : Data signal of cy_in
//        bit 31~0 - cy_in[31:0] (Read/Write)
// 0x3c : reserved
// 0x40 : Data signal of k1_in
//        bit 31~0 - k1_in[31:0] (Read/Write)
// 0x44 : reserved
// 0x48 : Data signal of k2_in
//        bit 31~0 - k2_in[31:0] (Read/Write)
// 0x4c : reserved
// 0x50 : Data signal of k3_in
//        bit 31~0 - k3_in[31:0] (Read/Write)
// 0x54 : reserved
// 0x58 : Data signal of k4_in
//        bit 31~0 - k4_in[31:0] (Read/Write)
// 0x5c : reserved
// 0x60 : Data signal of k5_in
//        bit 31~0 - k5_in[31:0] (Read/Write)
// 0x64 : reserved
// 0x68 : Data signal of k6_in
//        bit 31~0 - k6_in[31:0] (Read/Write)
// 0x6c : reserved
// 0x70 : Data signal of p1_in
//        bit 31~0 - p1_in[31:0] (Read/Write)
// 0x74 : reserved
// 0x78 : Data signal of p2_in
//        bit 31~0 - p2_in[31:0] (Read/Write)
// 0x7c : reserved
// 0x80 : Data signal of s1_in
//        bit 31~0 - s1_in[31:0] (Read/Write)
// 0x84 : reserved
// 0x88 : Data signal of s2_in
//        bit 31~0 - s2_in[31:0] (Read/Write)
// 0x8c : reserved
// 0x90 : Data signal of s3_in
//        bit 31~0 - s3_in[31:0] (Read/Write)
// 0x94 : reserved
// 0x98 : Data signal of s4_in
//        bit 31~0 - s4_in[31:0] (Read/Write)
// 0x9c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_IN_DATA         0x10
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_FX_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FX_INVERSE_IN_DATA 0x18
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_FX_INVERSE_IN_DATA 32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_IN_DATA         0x20
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_FY_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_FY_INVERSE_IN_DATA 0x28
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_FY_INVERSE_IN_DATA 32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CX_IN_DATA         0x30
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_CX_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_CY_IN_DATA         0x38
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_CY_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K1_IN_DATA         0x40
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K1_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K2_IN_DATA         0x48
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K2_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K3_IN_DATA         0x50
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K3_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K4_IN_DATA         0x58
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K4_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K5_IN_DATA         0x60
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K5_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_K6_IN_DATA         0x68
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_K6_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P1_IN_DATA         0x70
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_P1_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_P2_IN_DATA         0x78
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_P2_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S1_IN_DATA         0x80
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_S1_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S2_IN_DATA         0x88
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_S2_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S3_IN_DATA         0x90
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_S3_IN_DATA         32
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_ADDR_S4_IN_DATA         0x98
#define XUNDISTORT_COORDINATE_GENERATOR_CONTROL_BITS_S4_IN_DATA         32

