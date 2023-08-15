#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "timers.h"
#include "xttcps.h"

static XTtcPs* TimerInstancePtr;
volatile u64 RolloverCounter = 0;
volatile u64 start_RolloverCounter = 0;
volatile u64 end_RolloverCounter = 0;
volatile u16 start_time = 0;
volatile u16 end_time = 0;

void TimerIntrHandler(void *CallBackRef){
    XTtcPs *TtcInstance = (XTtcPs *)CallBackRef;
    // Read the interrupt status to check which interrupt is active
    // int reg = XTtcPs_ReadReg(TtcInstance->Config.BaseAddress, XTTCPS_ISR_OFFSET);
    // std::cout << "Register status: " << std::hex << reg << std::dec << std::endl;
    RolloverCounter++;
    // std::cout << "Timer interrupt triggered. RolloverCounter: " << RolloverCounter << std::endl;
    XTtcPs_ClearInterruptStatus(TtcInstance, XTTCPS_IXR_INTERVAL_MASK);
}

int RegisterTimerInterrupts(XTtcPs *TtcInstancePtr, XScuGic *IntcInstancePtr){
    int Status;
    Status = XScuGic_Connect(IntcInstancePtr, XPAR_XTTCPS_0_INTR, (Xil_ExceptionHandler)TimerIntrHandler, (void *)TtcInstancePtr);
        if (Status != XST_SUCCESS) {
        std::cout << "Error: XScuGic_Connect" << std::endl;
        return Status;
    }
    XScuGic_Enable(IntcInstancePtr, XPAR_XTTCPS_0_INTR);
    XTtcPs_EnableInterrupts(TtcInstancePtr, XTTCPS_IXR_INTERVAL_MASK);
    return XST_SUCCESS;
}

void enable_ttc(XTtcPs *TtcInstancePtr, XScuGic *IntcInstancePtr, int prescaler) {
    int Status;
    // Enable TTC
    XTtcPs_Config *TtcConfig;
    TtcConfig = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);
    Status = XTtcPs_CfgInitialize(TtcInstancePtr, TtcConfig, TtcConfig->BaseAddress);
    if (Status != XST_SUCCESS) {
        std::cout << "Error: XTtcPs_CfgInitialize" << std::endl;
    }

    XTtcPs_SetPrescaler(TtcInstancePtr, prescaler);
    Status = XTtcPs_SetOptions(TtcInstancePtr, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE | XPAR_XTTCPS_0_TTC_CLK_CLKSRC);
    if (Status != XST_SUCCESS) {
        std::cout << "Error: XTtcPs_SetOptions" << std::endl;
    }

    // Set interval value to maximum
    XTtcPs_SetInterval(TtcInstancePtr, 0xFFFF);

    // Reset the counter value
    XTtcPs_ResetCounterValue(TtcInstancePtr);

    // Enable TTC interrupt
    RegisterTimerInterrupts(TtcInstancePtr, IntcInstancePtr);
    // XTtcPs_DisableInterrupts(TtcInstancePtr, XTTCPS_IXR_ALL_MASK);
    XTtcPs_EnableInterrupts(TimerInstancePtr, XTTCPS_IXR_INTERVAL_MASK);

    XTtcPs_Start(TtcInstancePtr);

    // Save the pointer for future calls
    TimerInstancePtr = TtcInstancePtr;
}


void measure_time_start(){
    start_time = XTtcPs_GetCounterValue(TimerInstancePtr);
    start_RolloverCounter = RolloverCounter;
}

void measure_time_stop(){
    end_time = XTtcPs_GetCounterValue(TimerInstancePtr);
    end_RolloverCounter = RolloverCounter;
}

void get_timer_data(volatile u64 *start_rollover_counter, volatile u64 *end_rollover_counter, volatile u16 *start, volatile u16 *end){
    *start_rollover_counter = start_RolloverCounter;
    *end_rollover_counter = end_RolloverCounter;
    *start = start_time;
    *end = end_time;
}


//total time = ((number of rollovers * (maximum counter value + 1)) + (end counter value - start counter value + 1)) / (TIMER_CLOCK_FREQ / prescaler)
double compute_elapsed_from_timer_data(u64 start_rollover_counter, u64 end_rollover_counter, u32 start, u32 end, int prescaler){
    int real_prescaler;
    if (prescaler >= 16)
        real_prescaler = 1;
    else
        real_prescaler = 1 << (prescaler+1);
    return (double)(abs((int64_t)end_rollover_counter - (int64_t)start_rollover_counter) * (int64_t)(0xFFFF + 1) + (int64_t)end - (int64_t)start + 1) / ((double)XPAR_XTTCPS_0_TTC_CLK_FREQ_HZ / (double)real_prescaler);
}

void print_time(char *task, int prescaler){
    double time = compute_elapsed_from_timer_data(start_RolloverCounter, end_RolloverCounter, start_time, end_time, prescaler);
    std::cout << "(" << task << ") Time taken: " << time << " seconds" << std::endl;
}
