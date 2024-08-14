/* UART5.cpp - Hardware serial library
 * This library is free software under LGPL 2.1. See License.md
 * for more information. This file is part of DxCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * unknown others 2013-2020, 2020-2021 Spence Konde
 */

#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART5)
  #if defined(USE_ASM_TXC) && USE_ASM_TXC == 1 //&& defined(USART1) // No benefit to this if it's just one USART
    // Note the difference between this and the other ISRs - here we don't care at all about the serial object, we just have to work with the USART
    ISR(USART5_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"           "\n\t" // push the low byte of Z - we start out 5-6 clocks behind the ball, these three instructions take 4-5 -> 9 or 11 by th time we reach _do_txc
            "ldi r30, 0xA0"       "\n\t" // and put the low bit of this USART there - 0x20 * n
#if PROGMEM_SIZE > 8192
            "jmp _do_txc"         "\n\t"
#else
            "rjmp _do_txc"        "\n\t"
#endif // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
       // The reason this is possible here and not elsewhere is because TXC only needs the USART, while the others need the HardwareSerial instance.
            :::);
    }
#elif defined(USE_ASM_TXC) && USE_ASM_TXC == 2
    ISR(USART5_TXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t"
              "push      r31"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_txc"     "\n\t"
#else
              "rjmp   _do_txc"    "\n\t"
#endif
              ::"z"(&Serial5));
        __builtin_unreachable();
    }

  #else
    ISR(USART5_TXC_vect) {
      // only enabled in half duplex mode - we disable RX interrupt while sending.
      // When we are done sending, we re-enable the RX interrupt and disable this one.
      // Note that we do NOT clear TXC flag, which the flush() method relies on.
      uint8_t ctrla;
      while (USART5.STATUS & USART_RXCIF_bm) {
        ctrla = USART5.RXDATAL;   // We sent these, so dump them, using local var as trashcan.
      }
      ctrla = USART5.CTRLA;       // Get current CTRLA
      ctrla |= USART_RXCIE_bm;    // turn on receive complete
      ctrla &= ~USART_TXCIE_bm;   // turn off transmit complete
      USART5.CTRLA = ctrla;       // Write it back to CTRLA.
    }
  #endif

  #if !(defined(USE_ASM_RXC) && (USE_ASM_RXC == 1 || USE_ASM_RXC == 2) && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) /* && defined(USART1)*/)
    ISR(USART5_RXC_vect) {
      HardwareSerial::_rx_complete_irq(Serial);
    }
  #else
      ISR(USART5_RXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t" //we start out 5-6 clocks behind the ball, then do 2 push + 2 ldi + 2-3 for jump = 11 or 13 clocks to _do_rxc (and dre is the same)
              "push      r31"     "\n\t"
              "cbi   0x1F, 0"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_rxc"     "\n\t"
#else
              "rjmp   _do_rxc"    "\n\t"
#endif
              ::"z"(&Serial5));
        __builtin_unreachable();
    }
  #endif
  #if !(defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
       (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
       (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART5_DRE_vect) {
      HardwareSerial::_tx_data_empty_irq(Serial);
    }
  #else
    ISR(USART5_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"       "\n\t"
                "push  r31"       "\n\t"
                "cbi   0x1F, 0"   "\n\t"
                :::);
      __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
                "jmp _do_dre"     "\n\t"
#else
                "rjmp _do_dre"    "\n\t"
#endif
                ::"z"(&Serial5));
      __builtin_unreachable();
    }
  #endif
  HardwareSerial Serial5(&USART5, (uint8_t*)_usart5_pins, MUXCOUNT_USART5, HWSERIAL5_MUX_DEFAULT);
#endif
