#include "xttcps.h"
#include "xscugic.h"

#define TIMER_PRESCALER 16 // 0-15 -> 2^(prescaler+1), 16 -> 1

void TimerIntrHandler(void * CallBackRef);

int RegisterTimerInterrupts(XTtcPs *TtcInstancePtr, XScuGic *IntcInstancePtr);

void enable_ttc(XTtcPs *TtcInstancePtr, XScuGic *IntcInstancePtr, int prescaler);

void measure_time_start();

void measure_time_stop();

void get_timer_data(volatile u64 *start_rollover_counter, volatile u64 *end_rollover_counter, volatile u16 *start, volatile u16 *end);

double compute_elapsed_from_timer_data(u64 start_rollover_counter, u64 end_rollover_counter, u32 start, u32 end, int prescaler);

void get_elapsed_time(double *elapsed_time, int prescaler);

void print_time(char *task, int prescaler);
