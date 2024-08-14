# Optiboot_dx bootloader technical details

This directory contains the Optiboot small bootloader for AVR Dx-series
microcontrollers as distributed with DxCore.

Optiboot is more fully described here: [http://github.com/Optiboot/optiboot](http://github.com/Optiboot/optiboot)

## Credit where credit is due
Optiboot is the work of **Peter Knight** (aka Cathedrow), building on work of **Jason P** **Kyle**, **Spiff**, and **Ladyada**.

More recent maintenance and modifications are by **Bill Westfield** (aka WestfW).

Adaptation of Optiboot_x to Optiboot_dx by **Spence Konde** (aka DrAzzy, Azduino).

## Reporting issues
This is being maintained as part of DxCore, so issues be reported to:
[https://github.com/SpenceKonde/DxCore](https://github.com/SpenceKonde/DxCore)

## Changes made beyond the largescale adaptation to Dx-series parts
* To enable writes to flash from the app, the entry point is vastly different from Optiboot_x. On the Dx-series, only a single operation is protected and must run from the boot section: The ST or SPM which actually triggers the writing of data to the flash. Thus, everything else can be in the app, and we just need to have a an SPM Z+ instruction followed by a RET, These instructions have opcodes of 95F8 and 9508. Considering endianness, we want them to come out as `F895` `0895`. This is achieved by declaring a `((used))` constant at 0x0001FA (so it's immediately before the optiboot version), with the value `0x950895F8` - endianness reverses the order of the bytesm so it gives the needed F895 0895 sequence needed. The application then stages everything, culminating in a snippet of inline assembly which copies the word to write to r0 and r1, issues `call 0x01FA` (the assembler divides the number we give in half, as this instruction is word addressed; we must not do that division ourselves), and then clears r1 and returns to C. This is a scary approach, to be sure. However, it does work!
* This document has been converted into markdown for easier viewing on github.
* Supposedly the dummy app which made suize reporting suck was required. Turns out it's not. So it's gona and avrsize is back to normal

## Known issues
There are no known issues at this time (other than the fact that there is no EEPROM support That is because it does not fit. It might fit if we didn't need to buffer pages and could write data as it came in, but because we don't know we're getting a program page command until the fire hose of data has been turned on, we can't get rid of that so easily. It was a design decision to not lock in a 1024 byte bootloader section just to get EEPROM write capability; and the consequences are particularly serious on modern AVRs which cannot tolerate )

Available "slack" in the binaries is extremely limited - worst case values are:
* 128k Dx: 2 instruction word (4b)
* 64k Dx:  7 instruction words (14b)
* 64k DD: 10 instruction words (20b)
* 32k Dx: 13 instruction words (26b)
* 32k DD: 13 instruction words (26b)

LED Pins
* DA/DB, all: PA7
* 20+ pin DD: PA7
* 14-pin DD, Serial0: PD6
* 14-pin DD, Serial1: PD4

### Other resources

Arduino-specific issues are tracked as part of the Arduino project
at [http://github.com/arduino/Arduino](http://github.com/arduino/Arduino)


There is additional information in the wiki - however, the wiki does not reflect the changes specific to Optiboot X for megaTinyCore
[https://github.com/Optiboot/optiboot/wiki](https://github.com/Optiboot/optiboot/wiki)


## To set up this environment in Windows
This is how the files megaTinyCore ships with are built. I can provide no guarantee that any other method will still work after what I've done to the makefile.
1. Download and extract the Arduino 1.0.6 .zip package.
* Make sure it is not in a "protected" location. I normally put them in the root of C:.
* Rename the folder so you remember what it's for; I have it in `C:\arduino-1.0.6-7.3.0compiler` (this location will be used in the examples below)
* You may delete everything except the hardware and tools subdirectories
2. Download [the updated toolchain`https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2`](https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2)
3. Extract the toolchain, and locate the top level 'avr' folder inside.
4. Copy the AVR folder to the `C:\arduino-1.0.6-7.3.0compiler\hardware\tools` directory in the 1.0.6 install. Be careful not to put it inside the avr folder that is already there (if you make a mistake, delete the mess and go back to step 1 - trying to unpick the mess is hopeless, because there are like 4 nested folders named avr, 2 folders in different locations named bin, and several 'include' folders. Don't look at me, I didn't choose that directory structure!
5. Copy the `optiboot_dx` folder from `(core install location)/megaavr/bootloaders` to `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders`.
6. You should now be able to open a command window in `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders\optiboot_dx` and run `omake <target>`, or `build_all_dx.bat` to build all the binaries packaged with the core.

This can be done the same way in linux except that you would download the version of the toolchain aoppropriate to your OS, and you can expect make to actually be there, ignore omake, and just use the shell scripts instead of batch files. which If i had time to work on, could be mde a lot smoother, but that's not a priority (I think the main thing we'd want to to is pass the avr-size output through grep or something, but I don't know makefile or shellscripting :-) )


## Previous build instructions
These may or may not work. I am not versed in the ways of makefiles and am not competent to fix any issues that may be encountered. Thus, if you want to use one of these methods, you'll have to fix the makefile yourself (sorry - my brain uses 16-bit addressing. You'll notice I never talk about working with an ATmega2560 - that would require an extra bit, ya see? So figuring out how to wrangle a makefile on a 64-bit desktop computer is well beyond my ability). I will gladly accept pull requests that fix these things - as long as they do not break the method detailed above, as that would leave me unable to build DxCore bootloaders without toolchain wrangling, which as I said, is outside my zone of competence. If you find that these do in fact work as written, please let me know so I can update this file.

### Building optiboot for Arduino

Production builds of optiboot for Arduino are done on a Mac in "unix mode"
using CrossPack-AVR-20100115.  CrossPack tracks WINAVR (for windows), which
is just a package of avr-gcc and related utilities, so similar builds should
work on Windows or Linux systems.

One of the Arduino-specific changes is modifications to the makefile to
allow building optiboot using only the tools installed as part of the
Arduino environment, or the Arduino source development tree.  All three
build procedures should yield identical binaries (.hex files) (although
this may change if compiler versions drift apart between CrossPack and
the Arduino IDE.)


### Building Optiboot in the Arduino IDE Install

Work in the .../hardware/arduino/bootloaders/optiboot/ and use the
`omake <targets>` command, which just generates a command that uses
the arduino-included "make" utility with a command like:
    `make OS=windows ENV=arduino <targets>`
or  `make OS=macosx ENV=arduino <targets>`
On windows, this assumes you're using the windows command shell.  If
you're using a cygwin or mingw shell, or have one of those in your
path, the build will probably break due to slash vs backslash issues.
On a Mac, if you have the developer tools installed, you can use the
Apple-supplied version of make.
The makefile uses relative paths (`../../../tools/` and such) to find
the programs it needs, so you need to work in the existing optiboot
directory (or something created at the same "level") for it to work.



### Building Optiboot in the Arduino Source Development Install

In this case, there is no special shell script, and you're assumed to
have "make" installed somewhere in your path.
Build the Arduino source ("ant build") to unpack the tools into the
expected directory.


## Programming Chips Using the `_isp` Targets
This does not work on AVR Dx-series, and there are no plans to change this.

## Standard Targets
Being designed for DxCore, this provides 325 targets - there are 5 combinations of entry condition and timeout duration. There are two possible sizes for a DD-series part, and 7 combinations of . In the list below, brackets indicate an optional
parameter, while parenthesis indicates a mandatory one - one of the options must be used.
* `128dx_ser(0-5)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `64dx_ser(0-5)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `32dx_ser(0-4)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `64dd_ser(0-1)_[alt(1-4)_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `32dd_ser(0-1)_[alt(1-4)_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `32dd14_ser(0, 0_alt3, 0_alt4 or 1_alt2)_(extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `64dd14_ser(0, 0_alt3, 0_alt4 or 1_alt2)_(extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`

### serN indicates the USART number

* DA/DB: N is 0-5 for 64-pin parts, 0-4 for 48 pin parts, 0-2 for 28/32 pin ones
  * All files are built for the largest pincount available with that memory size; they are binary compatible - but obviously one that tries to use a port or mux option that doesn't exist will not work.
  * There is no UART3, 4, or 5 on parts with less than 48 pins. UART5 is only found on 64-pin parts.
* DD: N is 0 or 1 for all parts, as there are only two serial ports.
  * All files are built for the AVR64DD32 or AVR32DD32. These are binary compatible with lower pincounts, but as above, if you specified a port and mux option that isn't available, it will not work, see below.
  * A separate file is provided for the DD14 (14-pin) parts. This differs in one way only - the pin which the bootloader tries to blink. If USART0 is used, PD6 is assumed to be the builtin LED. otherwise, PD4 is. The reasoning is described in the [DD14 part specific documentation](../../extras/DD14.md).

### The next optional portion of the name indicates portmux option

* DA/DB: Either omit to use default pins, or alt_ to use the alternate pins. Be aware that alternate pin availability depends on the pincount.
  * ser4_alt will not work on 48-pin parts, ser1_alt works only on 48+ pin parts, and ser2_alt doesn't work on 28-pin parts.
  * ser0 (no alt) will not work if a crystal or external clock is connected. In the case of a crystal, it may be damaged. ser2 (no alt) will not work if a watch crystal is connected, and has an even higher risk of damaging the crystal.
* DD: omit to use the default pins, otherwise, ser0 can have alt1, alt2, alt3 or alt4. (PA4/5, PA2/3, PD4/PD5, PC1/PC2). ser1 can have only default pins, or alt2 (PD6, PD7).
  * ser0_alt1 and ser0_alt2 will not work on 14-pin parts. ser1 (no alt) will not work on 14 or 20-pin parts.
  * If a crystal of any sort is used, ser0 (no alt) will not work, and may damage the crystal connected to those pins.
  * There is no ser1_alt1 - that would put the serial on PC4 and PC5. None of the DD-series parts have those pins.

### The final option is the entry condition
* `extr` - Bootloader will run on reset pinreset and software reset, with a 1 second timeout. For uploads with autoreset.
* `extr_8sec` - Bootloader will run on reset pin reset and software reset, with an 8 second timeout. For uploads with manual reset.
* `all_8sec` - Bootloader will run on everything except a WDT reset, 8 second timeout.
* `poronly_8sec` - Bootloader will run only after a power on reset (POR), 8 second timeout. For manual power-cycle uploads when you're using reset as an input.
* `extronly` - Bootloader will run on a reset pin reset - but not a software reset - with a 1 second timeout. For uploads with autoreset.
* `swronly_8sec` - Bootloader will run only on a software reset, with 8-second timeout. Be aware that if your application does not perform a software reset, and does not smash the stack and crash, the bootloader will never run (since smashing the stack triggers a dirty reset, which in turn causes the bootloader to issue a software reset, that will run the bootloader).

Every permutation which corresponds to a valid serial port on some chip is available with all entry conditions

## The bootloader **clears the reset flags** and stashes it's value in GPIOR0 before jumping to the app
This is required in order to ensure that the entry conditions are honored, and to ensure that we don't try to run after a dirty reset without forcing a clean reset. With the reset flags cleared, a dirty reset is detected by a value of 0x00 in the reset flags.  In that case, we have no idea how the peripherals are configured, and since execution may have careened about the flash, whatever configuration the app had could be altered in arbitrary ways as well. Or it could have been caused by an interrupt with no ISR firing, in which case LVL0EX is still set and interrupts can't be enabled. Hence, the only action that can be expected to restore functionality is to issue a software reset.

Note that the same thing is done for non-optiboot configurations, where this step is instead done by the sketch in .init3 (ie, as soon as the stack pointer is initialized and r1 is cleared. The behavior is the same - reset flags cleared and stashed in GPIOR0.

## Making new entry conditions
This is very easy - you need only pass ENTRYCOND_REQUIRE=0xXX. This is compared to the reset flags.
The logic is simple:
1. If the the WDRF (WDT reset flag) is set, we always jump to the app in order to avoid an infinite loop: The bootloader exits by triggering a WDT reset, so a WDT reset means that either a WDT reset got us here, or the app triggered a WDT reset. If you want to start the bootloader from the app, trigger a Software Reset instead.
2. Otherwise, if the bitwise and of ENTRYCOND_REQUIRE and the reset flags is non-zero, we run the bootloader. Otherwise, we jump to the app.

## Origins of the different sizes on different parts
The parts with fully mapped flash write by byte, but have only a couple of words of additional space free versus 64k parts. This is somewhat surprising - you would think it would make much more of a difference than it does.

DD-series 32/16k parts lose 1 instruction word because they need to look up an additional byte of the part signature. AVR signatures have 0x1E as the first byte, and the second byte indicates the flash size, (0x9s for flash size in kb = 2<sup>s</sup> 0x90 is used for sub 1k as well. Note that the rare few parts for which the flash size is not a power of 2 are not always handled the same way; the single ATmega with 40k of flash is 0x95 (32k) while the ATmega4808/9 are 0x96 (64k). The ATmegaxxxRFR/RFR2 parts have 0xAs instead), and the final byte is assigned sequentially, but the 3 high bits are often used to indicate a new series or grouping of parts. 0x8x and 0x6x are used for ATAutomotive parts. Hence, the 64 and 128k parts must look up 1 byte only (the last one) and can simply ldi the other two, as can 32k DA/DB parts). Because the same DD binaries have to support two different flash sizes, the DD 32/16k parts need to look up the last two. That means an LDS instead of LDI, and LDS is a 2-byte instruction.

The majority of the extra flash required for the 128k bootloader is to handle the extended address command.
As of DxCore 1.5.0, we do not use SPM Z+ anymore on 128k parts - it caused verification failures when data was written to the last page in the first half of the flash, but no data was written to the second half of the flash, (despite the write being successful). This only cost 1 instruction word on the 128k parts (and forced us into the ugly hack to make it possible to use lower baud rates on 128k parts). It is still used on 64k parts. Though they have a RAMPZ register, the datasheet indicates that it is vestigal.
```text
RAMPZ is concatenated with the Z-register when reading
the (ELPM) program memory locations above the first 64 KB and writing the (SPM) program memory locations above
the first 128 KB of the program memory

...

These bits hold the MSB of the 24-bit address created by RAMPZ and the 16-bit Z-register. Only the number of bits
required to address the available data and program memory is implemented for each device. Unused bits will always
read as '0'.
```
Since the 64k parts have no program memory locations above the first 64k, RAMPZ should never have a non-zero value, so this should not be a problem.

It is plausible that with more aggressive optimization, it might be possible to make room for EEPROM writes on 64k and smaller flash. It would take a miracle to do that on 128k parts.

### Change 10/17/23
As of 10/17/2023, users building custom versions of optiboot_dx can specify baud rates lower than 62750 now even for 128k parts. This is done by, if (and only if, for readability sake) we are using the ALT serial port (costs 3 words) AND we have 128k of flash (costs 6 words) AND the baud rate is below 62750. It works by replacing the UART initialization code with a little piece of asm, and explicitly prepares a pointer for it. This saves 2 words of flash, and was the easiest way to get these configurations to build. The price is a blob of asm to do a trivial task just to save 4 bytes.

### Wastes of space
There are a few points at which the compiler generates terrible, terrible code.

```asm
After calling getch() toget the high byte of the length in r24
  mov r16, r24       // copy r24 (which is never reused) to r16,
  ldi r17, 0x00 ; 0  // load zero into r17 even though the next thing we do...
  mov r17, r16       // is copy r16 to r17...
  eor r16, r16       // and now we zero out r16....
  rcall .+180        // call getch() again
  or r16, r24        // this time we use an or, just for the hell of it.

This is done twice, and it wastes 3 instruction words, because it is equivalent to:
  mov r17, r24       // copy r24 to the place where we want it
  rcall .+180        // call getch() again
  mov r16, r24       // copy r24 to where we want the second byte.

```
Savings: 6 words, more than doubling the available space on the 128k parts.

```c++
  MYUART_TXPORT.OUT |= MYUART_TXPIN; // and "1" as per datasheet.
```
The datasheet does *not* say that!
```text
Initialization
Full Duplex Mode:
1. Set the baud rate (USARTn.BAUD).
2. Set the frame format and mode of operation (USARTn.CTRLC).
3. Configure the TXD pin as an output.
4. Enable the transmitter and the receiver (USARTn.CTRLB).
```
There is errata, but it is only relevant to half-duplex (ODME) mode.
Savings: 1 word

In real world situations, does it ever run into the WDT syncbusy situation? If not, ditching that would save 4 words.

Were there a way to know we had to erase a page before there was data on the way that we needed to receive, we could then write directly to the flash, making the process much faster and possibly lowering flash size enough to implement EEPROM writing!

But.... AFAIK there isn't a signal given of that.

## Where does the space go?
Here, sorted by instruction
256 instruction words is not very many instructions.
It is worth noting that this is not what typical compiled sketches are full of. You can see how r24 and sometimes r25 are the compiler's go-to registers for storing shortlived values.

```text

   8: 05 c0         rjmp  .+10      ; junk comment elided  <---- 19 rjmp  = 38 bytes. Junk comments were interpretations of dataspace as codespace and vice versa
  16: 03 c0         rjmp  .+6       ; junk comment elided
  24: ed c0         rjmp  .+474     ; 0x200 <app>
  74: ef cf         rjmp  .-34      ; junk comment elided
  84: e4 cf         rjmp  .-56      ; junk comment elided
  8c: dd cf         rjmp  .-70      ; junk comment elided
  96: ec cf         rjmp  .-40      ; junk comment elided
  9e: fa cf         rjmp  .-12      ; junk comment elided
  ae: e0 cf         rjmp  .-64      ; junk comment elided
  c6: d3 cf         rjmp  .-90      ; junk comment elided
  ca: fb cf         rjmp  .-10      ; junk comment elided
  d0: 2b c0         rjmp  .+86      ; 0x128 <head+0x10>
 126: a4 cf         rjmp  .-184     ; junk comment elided
 144: 08 c0         rjmp  .+16      ; 0x156 <head+0x3e>
 154: 8d cf         rjmp  .-230     ; junk comment elided
 166: 84 cf         rjmp  .-248     ; junk comment elided
 17a: 79 cf         rjmp  .-270     ; junk comment elided
 180: 95 cf         rjmp  .-214     ; junk comment elided
 186: 92 cf         rjmp  .-220     ; junk comment elided
 18e: fc cf         rjmp  .-8       ; 0x188 <putch>
 19c: fc cf         rjmp  .-8       ; 0x196 <getch>
 1a8: 01 c0         rjmp  .+2       ; 0x1ac <getch+0x16>
 1b4: fc cf         rjmp  .-8       ; 0x1ae <watchdogConfig>
 1ca: ff cf         rjmp  .-2       ; 0x1ca <verifySpace+0xa>
 1ce: dc cf         rjmp  .-72      ; 0x188 <putch>
 1dc: f1 cf         rjmp  .-30      ; 0x1c0 <verifySpace>
  28: c2 d0         rcall .+388     ; 0x1ae <watchdogConfig> <--- 34 rcalls, most to getch() = 68 bytes
  54: a0 d0         rcall .+320     ; 0x196 <getch>          3/34 watchdogConfig
  5a: 9d d0         rcall .+314     ; 0x196 <getch>          5/34 verifySpace
  5e: b0 d0         rcall .+352     ; 0x1c0 <verifySpace>    16/34 getch
  6e: 8c d0         rcall .+280     ; 0x188 <putch>          2/34 getNch
  72: 8a d0         rcall .+276     ; 0x188 <putch>          2/34 nvm_cmd
  94: 9d d0         rcall .+314     ; 0x1d0 <getNch>         6/34 putch
  a4: 78 d0         rcall .+240     ; 0x196 <getch>
  a8: 76 d0         rcall .+236     ; 0x196 <getch>
  ac: 89 d0         rcall .+274     ; 0x1c0 <verifySpace>
  b4: 70 d0         rcall .+224     ; 0x196 <getch>
  ba: 6d d0         rcall .+218     ; 0x196 <getch>
  bc: 6c d0         rcall .+216     ; 0x196 <getch>
  c2: 86 d0         rcall .+268     ; 0x1d0 <getNch>
  d2: 61 d0         rcall .+194     ; 0x196 <getch>
  dc: 5c d0         rcall .+184     ; 0x196 <getch>
  e6: 57 d0         rcall .+174     ; 0x196 <getch>
  f8: 4e d0         rcall .+156     ; 0x196 <getch>
 106: 5c d0         rcall .+184     ; 0x1c0 <verifySpace>
 10e: 67 d0         rcall .+206     ; 0x1de <nvm_cmd>
 114: 64 d0         rcall .+200     ; 0x1de <nvm_cmd>
 12c: 34 d0         rcall .+104     ; 0x196 <getch>
 136: 2f d0         rcall .+94      ; 0x196 <getch>
 13a: 2d d0         rcall .+90      ; 0x196 <getch>
 13e: 40 d0         rcall .+128     ; 0x1c0 <verifySpace>
 14e: 1c d0         rcall .+56      ; 0x188 <putch>
 160: 13 d0         rcall .+38      ; 0x188 <putch>
 16c: 29 d0         rcall .+82      ; 0x1c0 <verifySpace>
 170: 0b d0         rcall .+22      ; 0x188 <putch>
 174: 09 d0         rcall .+18      ; 0x188 <putch>
 184: 14 d0         rcall .+40      ; 0x1ae <watchdogConfig>
 1c0: ea df         rcall .-44      ; 0x196 <getch>
 1c8: f2 df         rcall .-28      ; 0x1ae <watchdogConfig>
 1d4: e0 df         rcall .-64      ; 0x196 <getch>
  c4: 80 e0         ldi r24, 0x00 ; 0                              <--- 34 LDI = 68 bytes
  d6: d0 e0         ldi r29, 0x00 ; 0
 130: d0 e0         ldi r29, 0x00 ; 0
  c0: 81 e0         ldi r24, 0x01 ; 1
  60: 81 e0         ldi r24, 0x01 ; 1
   c: 21 e0         ldi r18, 0x01 ; 1
 182: 81 e0         ldi r24, 0x01 ; 1
 1c6: 81 e0         ldi r24, 0x01 ; 1
 112: 82 e0         ldi r24, 0x02 ; 2
  c8: 83 e0         ldi r24, 0x03 ; 3
  6c: 83 e0         ldi r24, 0x03 ; 3
  38: 83 e0         ldi r24, 0x03 ; 3
  9c: 85 e0         ldi r24, 0x05 ; 5
  46: 87 e0         ldi r24, 0x07 ; 7
 10c: 88 e0         ldi r24, 0x08 ; 8
  26: 88 e0         ldi r24, 0x08 ; 8
  2c: 88 e0         ldi r24, 0x08 ; 8
  70: 80 e1         ldi r24, 0x10 ; 16
  92: 84 e1         ldi r24, 0x14 ; 20
 1cc: 84 e1         ldi r24, 0x14 ; 20
  66: 8a e1         ldi r24, 0x1A ; 26
 16e: 8e e1         ldi r24, 0x1E ; 30
  78: 2a e2         ldi r18, 0x2A ; 42
  50: 40 e4         ldi r20, 0x40 ; 64
  ec: 30 e4         ldi r19, 0x40 ; 64
 140: f6 e4         ldi r31, 0x46 ; 70
  7a: 38 e6         ldi r19, 0x68 ; 104
  32: 8a e8         ldi r24, 0x8A ; 138
 172: 87 e9         ldi r24, 0x97 ; 151
 1de: 9d e9         ldi r25, 0x9D ; 157 < CCP SPM key
  3e: 80 ec         ldi r24, 0xC0 ; 192
 1b6: 98 ed         ldi r25, 0xD8 ; 216 < CCP IOREG key
   a: 98 ed         ldi r25, 0xD8 ; 216
  f2: 8f ef         ldi r24, 0xFF ; 255
  ea: 81 2c         mov    r8, r1   25 words on moving contents of registers around.
  fe: 45 01         movw   r8, r10  4 of the mov's are part of the atrocious mishandling of the high and low bytes
  ee: 93 2e         mov    r9, r19
  f0: 54 01         movw  r10,  r8
 146: 68 01         movw  r12, r16
  e0: 6e 01         movw  r12, r28
 14c: 6f 01         movw  r12, r30
 13c: d8 2e         mov   r13, r24
  4e: e1 2c         mov   r14,  r1
  52: f4 2e         mov   r15, r20
  a6: 08 2f         mov   r16, r24
  aa: 18 2f         mov   r17, r24
 116: 9c 2d         mov   r25, r12
  18: 98 2f         mov   r25, r24
 10a: d7 01         movw  r26, r14
 12e: c8 2f         mov   r28, r24 < pathological
  5c: c8 2f         mov   r28, r24
  d4: c8 2f         mov   r28, r24 < pathological
 1d2: c8 2f         mov   r28, r24
  d8: dc 2f         mov   r29, r28 < pathological
 132: dc 2f         mov   r29, r28 < pathological
  fa: f4 01         movw  r30, r8  (r8:9, r10:11, r12:13, r14:15 and r16:17 are used to store pointers to be swapped back into X or Z)
 148: f6 01         movw  r30, r12
 158: f8 01         movw  r30, r16
 108: f8 01         movw  r30, r16
  56: 81 34         cpi r24, 0x41 ; 65                        <--- 13 cpi = 26 bytes
  62: c2 38         cpi r28, 0x82 ; 130
  68: c1 38         cpi r28, 0x81 ; 129
  8e: 82 34         cpi r24, 0x42 ; 66
  98: 85 34         cpi r24, 0x45 ; 69
  a0: 85 35         cpi r24, 0x55 ; 85
  b0: 86 35         cpi r24, 0x56 ; 86
  b6: 8d 34         cpi r24, 0x4D ; 77
  cc: 84 36         cpi r24, 0x64 ; 100
 128: 84 37         cpi r24, 0x74 ; 116
 168: 85 37         cpi r24, 0x75 ; 117
 17c: 81 35         cpi r24, 0x51 ; 81
 1c2: 80 32         cpi r24, 0x20 ; 32
  48: 81 50         subi  r24, 0x01 ; 1                       <--- 6 subi = 12 bytes
  86: 21 50         subi  r18, 0x01 ; 1
  e8: d0 5c         subi  r29, 0xC0 ; 192
 156: 1c 5e         subi  r17, 0xEC ; 236
 15c: 0f 5f         subi  r16, 0xFF ; 255
 1d6: c1 50         subi  r28, 0x01 ; 1
 150: 21 97         sbiw  r28, 0x01 ; 1
 162: 21 97         sbiw  r28, 0x01 ; 1
   0: 11 24         eor r1, r1        <----- 4 eor - first one is to prepare zero reg, the one at 0x0124 cleans it after it's been used to write the flash
 124: 11 24         eor r1, r1
  da: cc 27         eor r28, r28      <Pathological
 134: cc 27         eor r28, r28      <Pathological
 194: 08 95         ret               <----- 4 ret
 1ac: 08 95         ret
 1be: 08 95         ret
 1ec: 08 95         ret
  4c: a8 95         wdr               <----- 3 WDR
  7c: a8 95         wdr
 1aa: a8 95         wdr
 118: 0d 90         ld  r0, X+        <----- only three loads...
 11a: 1d 90         ld  r1, X+        <----- Two of these are used to read the page buffer back in
 15a: 80 81         ld  r24, Z        But 8 bulky LDS instructions:
   2: 80 91 40 00   lds r24, 0x0040 ; junk comment elided     040>  < RSTCTRL.RSTFR
 1a2: 80 91 20 08   lds r24, 0x0820 ; junk comment elided     820>  < UARTn.RXDATA
 196: 80 91 24 08   lds r24, 0x0824 ; junk comment elided     824>  < UARTn.STATUS
 176: 80 91 02 11   lds r24, 0x1102 ; junk comment elided     102>  < Sigbyte 2
 1ae: 90 91 01 01   lds r25, 0x0101 ; junk comment elided     101>  < WDT.STATUS
 19e: 90 91 21 08   lds r25, 0x0821 ; junk comment elided     821>  < UARTn.RXDATA
 188: 90 91 24 08   lds r25, 0x0824 ; junk comment elided     824>  < UARTn.STATUS
  7e: 90 91 24 08   lds r25, 0x0824 ; junk comment elided     824>  < UARTn.STATUS
  fc: 80 83         st  Z, r24          <------ Just one ST!
   e: 94 bf         out 0x34, r25 ; 52  <--- 6 OUT = 12 bytes
  22: 8c bb         out 0x1c, r24 ; 28  4/6 to the CCP register!
  be: 8b bf         out 0x3b, r24 ; 59
 1b8: 94 bf         out 0x34, r25 ; 52
 1e0: 94 bf         out 0x34, r25 ; 52
 1e6: 94 bf         out 0x34, r25 ; 52
  10: 20 93 41 00   sts 0x0041, r18 ; junk comment elided     041>   <--- and 11 bulky STS isns.
  1e: 80 93 40 00   sts 0x0040, r24 ; junk comment elided     040> //These first two around 0x0040 are resetctrl - reading the reset cause and pressing the big reset button if we wound up at init without any reset flags being set.
  2e: 80 93 51 04   sts 0x0451, r24 ; junk comment elided     451> //turning on a pullup
  34: 80 93 28 08   sts 0x0828, r24 ; junk comment elided     828> //config USART
  3a: 80 93 27 08   sts 0x0827, r24 ; junk comment elided     827> //config USART
  40: 80 93 26 08   sts 0x0826, r24 ; junk comment elided     826> //config USART
 190: 80 93 22 08   sts 0x0822, r24 ; junk comment elided     822> //putch
 1ba: 80 93 00 01   sts 0x0100, r24 ; junk comment elided     100> //config WDT
 1e2: 10 92 00 10   sts 0x1000, r1  ; junk comment elided     000> clear nvmctrl.ctrla
 1e8: 80 93 00 10   sts 0x1000, r24 ; junk comment elided     000> write a different value to it
  2a: 40 9a         sbi 0x08, 0 ; 8   <set TX direction
  44: 07 9a         sbi 0x00, 7 ; 0   <LED as output
  76: 17 9a         sbi 0x02, 7 ; 2   <Toggle LED
 110: e8 95         spm               <----- 2 SPM
 11c: e8 95         spm
  de: c8 2b         or  r28, r24      <----- the two 'or' instructions are part of that aforementioned inefficienct way to assemble bytes to a word.
 138: c8 2b         or  r28, r24
  e2: d6 94         lsr r13           < a pair of rightshifts
  e4: c7 94         ror r12
  1a: 95 73         andi  r25, 0x35 ; 53 <---- Only a single ANDI, and no ORI's.
 1d0: cf 93         push  r28         <----- Why the fuck is there a push and a pop, we have 9 unused registers!!!!
 1da: cf 91         pop r28           <----- wtf???
                                    ; And now all in one place: The control flow instructions.
  1c: 21 f4         brne  .+8       ; 0x26 junk comment elided     <--16brne's = 32 bytes
  4a: a9 f4         brne  .+42      ; 0x76 junk comment elided
  58: d1 f4         brne  .+52      ; 0x8e junk comment elided
  8a: c1 f7         brne  .-16      ; 0x7c junk comment elided
  90: 19 f4         brne  .+6       ; 0x98 junk comment elided
  9a: 11 f4         brne  .+4       ; 0xa0 junk comment elided
  a2: 31 f4         brne  .+12      ; 0xb0 junk comment elided
  b2: 61 f4         brne  .+24      ; 0xcc junk comment elided
  b8: 39 f4         brne  .+14      ; 0xc8 junk comment elided
 104: a9 f7         brne  .-22      ; 0xf0 junk comment elided
 122: d1 f7         brne  .-12      ; 0x118 <head>
 12a: f1 f4         brne  .+60      ; 0x168 <head+0x50>
 152: d1 f7         brne  .-12      ; 0x148 <head+0x30>
 164: c9 f7         brne  .-14      ; 0x158 <head+0x40>
 16a: 41 f4         brne  .+16      ; 0x17c <head+0x64>
 1d8: e9 f7         brne  .-6       ; 0x1d4 <getNch+0x4>
  64: 21 f0         breq  .+8       ; 0x6e junk comment elided    <--- and 5 breq's
  6a: 09 f0         breq  .+2       ; 0x6e junk comment elided
  ce: 09 f0         breq  .+2       ; 0xd2 junk comment elided
 17e: 09 f0         breq  .+2       ; 0x182 <head+0x6a>
 1c4: 19 f0         breq  .+6       ; 0x1cc <verifySpace+0xc>
  14: 83 fd         sbrc  r24, 3    < test for wdrf      <--- 8 other skipifs - 16 bytes
  82: 97 fd         sbrc  r25, 7
 18c: 95 ff         sbrs  r25, 5
 19a: 87 ff         sbrs  r24, 7
 1a6: 92 fd         sbrc  r25, 2
 1b2: 90 fd         sbrc  r25, 0
   6: 81 11         cpse  r24, r1   < test for empty RSTFR
 142: df 12         cpse  r13, r31
```

So, what can be done?

* Obviously, the places marked "Pathological" can be remodeled, saving 6 words as described above. Same for the WTF's which might be 2 more.
  * Actually wringing these out is much harder than looking at the asm listing and seeing what it's doing stupidly and what better code would look like
* The datasheet *does not specify* that you need to set the TX pin output. Certainly you must not set it output if you use ODME per errata on many parts.
  * ODME is on CTRLB, so if combined with the existing write, could set ODME for free.
  * Thus, even if in normal mode, you don't get any output, since serial adapters almost invariably have a weak internal pullup on their RX, you should be able to make it work by simply setting ODME, since you don't care about the state of the TX pin if you don't have anything connected to it.
  * This saves 1 word only, because it just gets rid of a sbi.
* There are a number of issues in the quality of the compiled code in general:
  * As usual, it stumbles when you're trying to fill a word one byte at a time, as mentioned in the pathological code - that is obviously wrong.
  * It's not clear how it's using r28-r29 - in some cases they appear to be using it as a scratch register.
    * Could that register be used better by statically allocating it? Does it hold anything of value when it is getting pushed and popped? Why that register?
* Registers used also reveals a considerable amount of unused slack that could be sacrificed for flash.

### Registers used and free
Bootloader space optimization might take advantage of the large number of unused registers for an advantage. Here's what the usage list looks like for a 128kb AVR Dx (which is where space problems are most acute):

#### Fixed Lower Working Registers
* r0 - the tempreg is used ONLY while actually writing the flash (you load the value to write into r0 and r1, then execute SPM). Doesn't get touched from anywhere else!
* r1 - Zero except when writing the flash as above.

#### Used Lower Working Registers
* r8 - Used very lightly
* r9 - Used very lightly
* r10 - r10 and r11 somewhat lightly used
* r11
* r12 - r12 and r13 used pretty heavily.
* r13
* r14 - r14 and r15 combined used only 4x, sometimes as a single reg, other times not.
* r15

#### Used Upper Working Registers
* r16   - r16 and r17 used 8 times combined.
* r17   -
* r18   - r18 and r19 used 8 times combined.
* r19   -
* r20   - Only used in a single place. To ferry a single value around. No need to use this reg.
* r24 w - compilers favorite register! 67/174 accesses!
* r25 w - Used 19 times, plus the times it was a register pair with r24.
* r26 X - "huh, only used once?" "psst, it's the X pointer, it's used right and left" "Oh, duh..."
* r27 X - (not explicitly, but accessed by movw is used with r26 and when using the X pointer)
* r28 Y
* r29 Y
* r30 Z
* r31 Z

#### Registers Currently Looking for Work
* r0 - (very nearly - it gets trashed when the flash is actually written)
* r2
* r3
* r4
* r5
* r6
* r7
* r21 - Well, will ya lookit that, unused high registers!
* r22
* r23

That's 4 wholly unused register pairs and a single, and the single and one pair is high. Thus, the only resources we are short on is flash and of course pointer registers - though we are a little bit tight on flash on the 128k bootloader, this situation has considerably more hope that it might have, as the compiler is showing these obvious pathologies, making it's code particularly bad, which is also easier to fix, and there's plenty of register maneuvering room.
