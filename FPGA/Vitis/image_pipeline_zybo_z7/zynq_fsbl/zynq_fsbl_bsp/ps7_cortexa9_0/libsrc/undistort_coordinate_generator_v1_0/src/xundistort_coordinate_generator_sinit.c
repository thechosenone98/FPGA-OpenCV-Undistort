// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xundistort_coordinate_generator.h"

extern XUndistort_coordinate_generator_Config XUndistort_coordinate_generator_ConfigTable[];

XUndistort_coordinate_generator_Config *XUndistort_coordinate_generator_LookupConfig(u16 DeviceId) {
	XUndistort_coordinate_generator_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XUNDISTORT_COORDINATE_GENERATOR_NUM_INSTANCES; Index++) {
		if (XUndistort_coordinate_generator_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XUndistort_coordinate_generator_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XUndistort_coordinate_generator_Initialize(XUndistort_coordinate_generator *InstancePtr, u16 DeviceId) {
	XUndistort_coordinate_generator_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XUndistort_coordinate_generator_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XUndistort_coordinate_generator_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

