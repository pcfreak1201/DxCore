/*  OBLIGATORY LEGAL BOILERPLATE
 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
 either version 2.1 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this library;
 if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*//*
   Modern AVR Comparator library for tinyAVR 0/1/2, megaAVR0, Dx, and  Ex
   Developed in 2019 by MCUdude    https://github.com/MCUdude/
   Ported to tinyAVR & Dx-series by Spence Konde for megaTinyCore and
   DxCore 2021-2023: https://github.com/SpenceKonde/
*/

#ifndef COMPARATOR_h
#define COMPARATOR_h

#include <Arduino.h>
// *INDENT-OFF* - astyle wants me to indent the #if's in a way that makes it harder to read.
/* First, let's figure out how many pins we support.... */
#if !defined(MEGATINYCORE)
  /* This means we're working with a Dx or megaAVR 0-series */
  #if defined(__AVR_DA__) || defined(__AVR_DB__)
    /* P0, P1, P2, P3, N0, N1, N2 */
    #define ANALOG_COMP_PINS_DA_DB
    #if !defined(PORTE)
      #define AC_NULL_REG _SFR_MEM8(0x04B0)
    #endif
    #if defined(DB_28_PINS) || defined(DB_32_PINS)
      #define ANALOG_COMP_NO_N1
    #endif
  #elif defined(__AVR_DD__)
    /* DD:1 AC:  P0, P3, P4, N0, N2, N3 */
    #define ANALOG_COMP_PINS_DD
    #define AC_NULL_REG _SFR_MEM8(0x04B0)
    #define ANALOG_COMP_NO_N1
  #elif defined(__AVR_EA__)
    /* EA:2 ACs: P0, P1, P2, P3, P4, N0, N1, N2, N3 */
    #define ANALOG_COMP_PINS_EA
    #if !defined(PORTE)
      #define AC_NULL_REG _SFR_MEM8(0x04B0)
    #endif
  #else
    /* mega0:1 AC P0, P1, P2, P3, N0, N1, N2 */
    #define ANALOG_COMP_PINS_MEGA
  #endif
#else
  /* we're on a tinyAVR.... */
  #if   ((MEGATINYCORE_SERIES == 0 && defined(__AVR_ATTinyxy4__)                      ) || \
         (MEGATINYCORE_SERIES == 1 && defined(__AVR_ATTinyxy4__)  && PROGMEM_SIZE <= 8192) || \
                                      defined(__AVR_ATtinyxy2__)                      )
    /* P0, N0. Nada mas - for 8-pin parts, 14-pin 0-series, and 14-pin "lesser" 1-series */
    #define ANALOG_COMP_PINS_TINY_FEW
  #elif  (MEGATINYCORE_SERIES == 0) || \
         (MEGATINYCORE_SERIES == 1  /*20 or 24 pin parts now */   && PROGMEM_SIZE <= 8192)
    /* P0, P1, N0, N1 - for 20/24-pin 0-series, and 20/24-pin "lesser" 1-series */
    #define ANALOG_COMP_PINS_TINY_SOME
  #elif (MEGATINYCORE_SERIES == 1) // if above didn't catch it, we've got a "golden" 1-series
    /* Golden 1-series. None of them are missing a given input from all three comparators */
    #if   defined(__AVR_ATtinyxy4__)
      #define ANALOG_COMP_PINS_TINY_GOLDEN_14
    #else
      #define ANALOG_COMP_PINS_TINY_GOLDEN
    #endif
  #elif defined(__AVR_ATtinyx27__) ||  defined(__AVR_ATtinyx26__) // otherwise it's 2-series...
    /* P0, P1, P2, P3, N0, N1, N2 if it's got 24 pins*/
    /* P0, P1, P2,     N0, N1, N2 if it has 20 pins but we don't bother with changes to constructors*/
    #define ANALOG_COMP_PINS_TINY_TWO
  #else
    /* P0, P2, N0, N2 if it has 14*/
    #define ANALOG_COMP_PINS_TINY_TWO_14
  #endif
#endif

namespace comparator {
  namespace out {
    enum output_t : uint8_t {
      disable         = 0x00,
      disable_invert  = 0x80,
      enable          = 0x40,
      invert          = 0xC0,
      enable_invert   = 0xC0,
    };
    enum pinswap_t : uint8_t {
      no_swap  = 0x00,
  #if defined(DXCORE) && (defined(HAS_48_PINS) || defined(HAS_64_PINS))
      pin_swap = 0x01
  #endif
    };
    enum initval_t : uint8_t {
      init_low  = 0x00,
      init_high = 0x40,
    };
  };

  namespace hyst {
    enum hysteresis_t : uint8_t {
      disable = 0x00, // No hysteresis
      small   = 0x02, // 10 mV
      medium  = 0x04, // 25 mV
      large   = 0x06, // 50 mV
    };
  };

  #if defined(DXCORE)

    namespace in_p {
      enum inputP_t : uint8_t {
      in0    = 0x00,
      pd2    = 0x00,
      #ifndef __AVR_DD__
        in1    = 0x01,
        in2    = 0x02,
      #endif
      in3    = 0x03,
      pd6    = 0x03,
      #if defined(__AVR_DD__) || defined(__AVR_EA__)
        in4    = 0x04,
        pc3    = 0x04,
      #endif
      };
    };

    namespace in_n {
      enum inputN_t : uint8_t {
        in0    = 0x00,
        #if !defined(ANALOG_COMP_NO_N1)
          in1    = 0x01,
          pd0    = 0x01,
        #endif
        in2    = 0x02,
        pd7    = 0x02,
        #if defined(__AVR_DD__) || defined(__AVR_EA__) || defined(__AVR_EB__)
          in3    = 0x03,
          pc3    = 0x03,
          dacref = 0x04
        #else
          dacref = 0x03
        #endif
      };
    };
  #elif defined(MEGATINYCORE)
    namespace in_p {
      enum inputP_t : uint8_t {
        in0    = 0x00,
        #if !(defined(ANALOG_COMP_PINS_TINY_FEW) || defined(ANALOG_COMP_PINS_TINY_TWO_14))
          in1    = 0x01,
        #endif
        #if !(defined(ANALOG_COMP_PINS_TINY_FEW) || defined(ANALOG_COMP_PINS_TINY_SOME))
          in2    = 0x02,
        #endif
        #if defined(ANALOG_COMP_PINS_TINY_TWO) || defined(ANALOG_COMP_PINS_TINY_GOLDEN)
          in3    = 0x03
        #endif
      };
    };

    namespace in_n {
      enum inputN_t : uint8_t {
        in0      = 0x00,
        #if !(defined(ANALOG_COMP_PINS_TINY_GOLDEN_14) || defined(ANALOG_COMP_PINS_TINY_TWO_14) || defined(ANALOG_COMP_PINS_TINY_GOLDEN_14))
          in1    = 0x01,
        #endif
        #if defined(ANALOG_COMP_PINS_TINY_TWO) || defined(ANALOG_COMP_PINS_TINY_TWO_14)
          in2    = 0x02,
        #else
          vref   = 0x02,
        #endif
        #if (MEGATINYCORE_SERIES != 0)
          dacref = 0x03,
        #endif
      };
    };
  #else /* megaAVR 0-series */
    namespace in_p
    {
      enum inputP_t : uint8_t
      {
        in0    = 0x00,
        in1    = 0x01,
        in2    = 0x02,
        in3    = 0x03,
      };
    };

    namespace in_n
    {
      enum inputN_t : uint8_t
      {
        in0    = 0x00,
        in1    = 0x01,
        in2    = 0x02,
        dacref = 0x03,
      };
    };
  #endif

  namespace ref {
    enum reference_t : uint8_t {
    #if defined(MEGATINYCORE) && MEGATINYCORE_SERIES == 2

        vref_1v024 = 0x00, // 1.024V
        vref_1v0   = 0x00, // alias
        vref_2v048 = 0x01, // 2.048V
        vref_2v0   = 0x01, // Alias
        vref_2V1   = 0x01, // alias
        vref_2v500 = 0x02, // 2.5V
        vref_2v5   = 0x02,
        vref_4v096 = 0x03, // 4.096V
        vref_4v1   = 0x03, // Alias
        vref_vdd   = 0x07, // VDD as reference
    #elif defined(MEGATINYCORE) || !defined(DXCORE) // 0/1-series tiny, 0-series mega.
        vref_0v55  = 0x00, // 0.55V
        vref_1v1   = 0x01, // 1.1V
        vref_1v5   = 0x04, // 1.5V
        vref_2v5   = 0x02, // 2.5V
        vref_2v500 = 0x02,
        vref_4v3   = 0x03, // 4.3V
        vref_4v34  = 0x03, // 4.3V Alias
    #else // Dx/Ex
        vref_1v024 = 0x00, // 1.02V
        vref_1v0   = 0x00, // alias
        vref_2v048 = 0x01, // 2.05V
        vref_2v0   = 0x01, // Alias
        vref_2V1   = 0x01, // alias - since the targeted value is right in the middle
        vref_4v096 = 0x02, // 4.1V
        vref_4v1   = 0x02, // Alias
        vref_2v500 = 0x03, // 2.5V
        vref_2v5   = 0x03, // Alias
        vref_vdd   = 0x05, // VDD as reference
        vref_vrefa = 0x06,
    #endif
    disable    = 0x08,
    };
  };
};

// Legacy definitions
namespace out          {
  using namespace comparator::out;
};
namespace hyst          {
  using namespace comparator::hyst;
};
namespace ref          {
  using namespace comparator::ref;
};
class AnalogComparator {
  public:
    #if defined(ANALOG_COMP_PINS_DA_DB) || defined(ANALOG_COMP_PINS_MEGA)
      /* DA, DB, and megaAVR have 7 inputs per AC: P0, P1, P2, P3, N0, N1, N2 */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p,
                       register8_t& in2_p,
                       register8_t& in3_p,
                       register8_t& in0_n,
                       register8_t& in1_n,
                       register8_t& in2_n);

    #elif defined(ANALOG_COMP_PINS_DD) /*6 inputs - P1, P2, and N1 are gone, but newly added P4 and N3 are available P0, P3, P4, N0, N2, N3 */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in3_p,
                       register8_t& in4_p,
                       register8_t& in0_n,
                       register8_t& in2_n,
                       register8_t& in3_n);
    #elif defined(ANALOG_COMP_PINS_EA) /*9 inputs - with 48 pins EA gets all inputs for at least one comparator. P0, P1, P2, P3, P4, N0, N1, N2, N3 */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p,
                       register8_t& in2_p,
                       register8_t& in3_p,
                       register8_t& in4_p,
                       register8_t& in0_n,
                       register8_t& in1_n,
                       register8_t& in2_n,
                       register8_t& in3_n);
    /* tinyAVR starts here!*/
    #elif defined(ANALOG_COMP_PINS_TINY_FEW)        /* 1 pos 1 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in0_n);
    #elif defined(ANALOG_COMP_PINS_TINY_SOME)       /* 2 pos, 2 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p,
                       register8_t& in0_n,
                       register8_t& in1_n);
    #elif defined(ANALOG_COMP_PINS_TINY_TWO_14)     /* 2 pos, 2 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in2_p,
                       register8_t& in0_n,
                       register8_t& in2_n);
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14)  /* 3 pos, 1 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p, /* only for AC2 */
                       register8_t& in2_p, /* Not for AC2  */
                       register8_t& in0_n);
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)     /* 4 pos, 2 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p,
                       register8_t& in2_p,
                       register8_t& in3_p,
                       register8_t& in0_n,
                       register8_t& in1_n);
    #elif defined(ANALOG_COMP_PINS_TINY_TWO)     /* 4 pos, 3 neg */
      AnalogComparator(const uint8_t comparator_number,
                       AC_t& ac,
                       register8_t& in0_p,
                       register8_t& in1_p,
                       register8_t& in2_p,
                       register8_t& in3_p,
                       register8_t& in0_n,
                       register8_t& in1_n,
                       register8_t& in2_n);
    #else
      #error "Can't happen - unsupported part or part definition macros wrong"
    #endif
    void init();
    void start(bool state = true);
    void stop(bool restorepins = false);
    void attachInterrupt(voidFuncPtr callback, uint8_t mode);
    void detachInterrupt();
    bool read() {
      #if defined(AC_CMPSTATE_bm)
        return !!(AC.STATUS & AC_CMPSTATE_bm);
      #else
        return !!(AC.STATUS & AC_STATE_bm);
      #endif
    }
    AC_t& getPeripheral() {
      return AC;
    }

    comparator::out::output_t      output         = comparator::out::disable;
    #if defined(DXCORE)
      comparator::out::pinswap_t     output_swap    = comparator::out::no_swap;
    #endif
    comparator::out::initval_t     output_initval = comparator::out::init_low;
    comparator::hyst::hysteresis_t hysteresis     = comparator::hyst::disable;
    comparator::in_p::inputP_t     input_p        = comparator::in_p::in0;
    comparator::in_n::inputN_t     input_n        = comparator::in_n::in0;
    comparator::ref::reference_t   reference      = comparator::ref::disable;
    uint8_t dacref = 0xff;

  private:
    const uint8_t comparator_number;
    AC_t &AC;
    #if defined(ANALOG_COMP_PINS_DA_DB) || defined(ANALOG_COMP_PINS_MEGA)
      register8_t &IN0_P;                          /* P0, P1, P2, P3,     N0, N1, N2     */
      register8_t &IN1_P;
      register8_t &IN2_P;
      register8_t &IN3_P;
      register8_t &IN0_N;
      register8_t &IN1_N;
      register8_t &IN2_N;
    #elif defined(ANALOG_COMP_PINS_DD)             /* P0,         P3, P4, N0,     N2, N3 */
      register8_t &IN0_P;
      register8_t &IN3_P;
      register8_t &IN4_P;
      register8_t &IN0_N;
      register8_t &IN2_N;
      register8_t &IN3_N;
    #elif defined(ANALOG_COMP_PINS_EA)             /* P0, P1, P2, P3, P4, N0, N1, N2, N3 */
      register8_t &IN0_P;
      register8_t &IN1_P;
      register8_t &IN2_P;
      register8_t &IN3_P;
      register8_t &IN4_P;
      register8_t &IN0_N;
      register8_t &IN1_N;
      register8_t &IN2_N;
      register8_t &IN3_N;
    #elif defined(ANALOG_COMP_PINS_TINY_FEW)       /* P0,                 N0             */
      register8_t &IN0_P;
      register8_t &IN0_N;
    #elif defined(ANALOG_COMP_PINS_TINY_SOME)      /* P0, P1,             N0, N1         */
      register8_t &IN0_P;
      register8_t &IN1_P;
      register8_t &IN0_N;
      register8_t &IN1_N;
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14) /* P0, P1, P2,         N0            */
      register8_t &IN0_P;
      register8_t &IN1_P;
      register8_t &IN2_P;
      register8_t &IN0_N;
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)    /* P0, P1, P2, P3,     N0, N1        */
      register8_t &IN0_P;
      register8_t &IN1_P;
      register8_t &IN2_P;
      register8_t &IN3_P;
      register8_t &IN0_N;
      register8_t &IN1_N;
    #elif defined(ANALOG_COMP_PINS_TINY_TWO_14)    /* P0,     P2,         N0,     N2    */
      register8_t &IN0_P;
      register8_t &IN2_P;
      register8_t &IN0_N;
      register8_t &IN2_N;
    #elif defined(ANALOG_COMP_PINS_TINY_TWO)       /* P0, P1, P2, P3,     N0, N1, N2    */
      register8_t &IN0_P;
      register8_t &IN1_P;
      register8_t &IN2_P;
      register8_t &IN3_P;
      register8_t &IN0_N;
      register8_t &IN1_N;
      register8_t &IN2_N;
    #endif
    bool enable = false;
};

#if defined(AC0_AC_vect)
  extern AnalogComparator Comparator0;
  #define Comparator Comparator0
#endif

#if defined(AC1_AC_vect)
  extern AnalogComparator Comparator1;
#endif

#if defined(AC2_AC_vect)
  extern AnalogComparator Comparator2;
#endif

#endif
