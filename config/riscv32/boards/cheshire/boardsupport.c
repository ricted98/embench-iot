// Copyright 2025 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
#include <stdint.h>
#include <support.h>
#include "regs/cheshire.h"
#include "dif/clint.h"
#include "dif/uart.h"
#include "params.h"
#include "util.h"
#include "printf.h"

uint32_t cycles_total, instr_total, cycle_count, instret_count;
uint32_t cycles_initial, cycles_final;
uint32_t instr_initial, instr_final;
uint32_t bp_event_initial, bp_event_final;
uint32_t bp_mispred_initial, bp_mispred_final;

void
initialise_board ()
{
#if defined(PROFILE) && PROFILE == 1
    // uint32_t rtc_freq = CHS_REGS->rtc_freq.f.ref_freq;
    // uin32_t reset_freq = clint_get_core_freq(rtc_freq, 2500);
    uart_init(&__uart_base_addr__, 200000000, __BOOT_BAUDRATE);
#endif
}

void __attribute__ ((noinline)) __attribute__ ((externally_visible))
start_trigger ()
{
#if defined(PROFILE) && PROFILE == 1
    // Configure HPM event selectors: event 23 → counter3, event 26 → counter4
    // All immediates fit in 5 bits (0-31), so use csrwi/csrci directly
    __asm__ volatile("csrwi mhpmevent3, 26");
    __asm__ volatile("csrwi mhpmevent4, 23");

    // Clear inhibit bits for counter3 (bit 3) and counter4 (bit 4): mask = 24 = 0b11000
    __asm__ volatile("csrci mcountinhibit, 24");

    // Snapshot initial counter values
    __asm__ volatile("csrr %0, mhpmcounter3" : "=r"(bp_event_initial));
    __asm__ volatile("csrr %0, mhpmcounter4" : "=r"(bp_mispred_initial));
    __asm__ volatile("csrr %0, mcycle"        : "=r"(cycles_initial));
    __asm__ volatile("csrr %0, minstret"      : "=r"(instr_initial));
#endif
}

void __attribute__ ((noinline)) __attribute__ ((externally_visible))
stop_trigger ()
{
#if defined(PROFILE) && PROFILE == 1
    // Snapshot final counter values
    __asm__ volatile("csrr %0, mcycle"        : "=r"(cycles_final));
    __asm__ volatile("csrr %0, minstret"      : "=r"(instr_final));
    __asm__ volatile("csrr %0, mhpmcounter3"  : "=r"(bp_event_final));
    __asm__ volatile("csrr %0, mhpmcounter4"  : "=r"(bp_mispred_final));

    // Compute differences
    cycles_total = cycles_final      - cycles_initial;
    instr_total  = instr_final       - instr_initial;
    uint32_t bp_events   = bp_event_final   - bp_event_initial;
    uint32_t bp_mispreds = bp_mispred_final - bp_mispred_initial;

    // Print CSV header then values
    printf("tot_cycles,instret,bp_events,bp_mispredictions\r\n");
    printf("%u,%u,%u,%u\r\n",
           cycles_total,
           instr_total,
           bp_events,
           bp_mispreds);
    uart_write_flush(&__uart_base_addr__);
#endif
}
