/* Copyright (C) 2017 Embecosm Limited and University of Bristol

   Contributor Graham Markall <graham.markall@embecosm.com>

   This file is part of Embench and was formerly part of the Bristol/Embecosm
   Embedded Benchmark Suite.

   SPDX-License-Identifier: GPL-3.0-or-later */

   #include <stdint.h>
   #include <support.h>
   #include "regs/cheshire.h"
   #include "dif/clint.h"
   #include "dif/uart.h"
   #include "params.h"
   #include "util.h"
   #include "printf.h"

   void
   initialise_board ()
   {
    uint32_t rtc_freq = *reg32(&__base_regs, CHESHIRE_RTC_FREQ_REG_OFFSET);
    uint64_t reset_freq = clint_get_core_freq(rtc_freq, 2500);
    uart_init(&__base_uart, reset_freq, __BOOT_BAUDRATE);
   }

   void __attribute__ ((noinline)) __attribute__ ((externally_visible))
   start_trigger ()
   {
   }

   void __attribute__ ((noinline)) __attribute__ ((externally_visible))
   stop_trigger ()
   {
   }
