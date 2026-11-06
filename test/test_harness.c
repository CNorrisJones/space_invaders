#include "test_audio.h"
#include "test_cpu_interrupt.h"
#include "test_cpu_iobus.h"
#include "test_cpu_timing.h"
#include "test_cpu.h"
#include "test_display.h"
#include "test_interrupt_scheduler.h"
#include "test_iobus.h"
#include "test_shift_register_hardware.h"
#include "test_video.h"

int main(void) {
    int tests = 0;
    tests += test_audio();
    tests += test_cpu_interrupt();
    tests += test_cpu_iobus();
    tests += test_cpu_timing();
    tests += test_display();
    tests += test_interrupt_scheduler();
    tests += test_iobus();
    tests += test_shift_register_hardware();
    tests += test_video();
    tests += test_cpu();

    return (tests != 0);
}
