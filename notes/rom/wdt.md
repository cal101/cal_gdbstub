# Analyzing Watchdog Timer methods

The ESP8266 firmware rom contains watch dog timer related functions. 
On this page I show how I tried to understand them.

    grep wdt ld/eagle.rom.addr.v6.ld 
    PROVIDE ( ets_wdt_disable = 0x400030f0 );
    PROVIDE ( ets_wdt_enable = 0x40002fa0 );
    PROVIDE ( ets_wdt_get_mode = 0x40002f34 );
    PROVIDE ( ets_wdt_init = 0x40003170 );
    PROVIDE ( ets_wdt_restore = 0x40003158 );

You can use `objdump` to analyze an annotated ROM image like [this](https://github.com/jcmvbkbc/esp-elf-rom). 
I am doing it differently: Following the traces of [Blinky](https://github.com/cal101/cal_gdbstub/blob/master/BlinkyTale1.md) I am using gdb for disassembling and an annotated ROM image to provide symbols.

The following address ranges are usefull to understand the analysis:

    0x3fffc000 Addresses in data RAM, content is mutable
    0x40000000 Start of firmware ROM, content is immutable

The ABI defines how registers are used for parameters and return values.

Here is a simplified excerpt of some rules:

* Register `A0` is used as general purpose register and for return addresses of sub routine calls.
* Register `A1` is the stack pointer.
* Registers `A2`, `A3`, are used for function parameters and `A2` for return value.

## Analyzing `ets_wdt_disable`

    (gdb) disassemble &ets_wdt_disable

                   ;;; a6 := -2 / 0xFFFFFFFE
    0x400030f0 <+0>:	movi.n	a6, -2
                   ;;; standard prologue that makes space for 16 bytes on stack
    0x400030f2 <+2>:	addi	a1, a1, -16
                   ;;; save register a0 on stack
    0x400030f5 <+5>:	s32i.n	a0, a1, 0
                   ;;; save register a12 on stack
    0x400030f7 <+7>:	s32i.n	a12, a1, 4
                   ;;; a2 := *(0x40002f30) = 0x3fffc708
    0x400030f9 <+9>:	l32r	a2, 0x40002f30 <ets_get_cpu_frequency+36>
                   ;;; a4 = *(0x4000115c) = 0x60000600
    0x400030fc <+12>:	l32r	a4, 0x4000115c <main+368>
                   ;;; a12 := *(a2) = *(0x3fffc708)
    0x400030ff <+15>:	l32i.n	a12, a2, 0
                   ;;; MEMW is intended to implement the volatile attribute of 
                   ;;; languages such as C and C++.
                   ;;; memory sync barrier
    0x40003101 <+17>:	memw
                   ;;; a5 = *(a4 + 0x300) = *(0x60000900)
    0x40003104 <+20>:	l32i	a5, a4, 0x300
                   ;;; clear bit 1
                   ;;; a5 = a5 & a6 = a5 & 0xFFFFFFFE
    0x40003107 <+23>:	and	a5, a5, a6
                   ;;; memory sync barrier
    0x4000310a <+26>:	memw
                   ;;; *(0x60000900) = a5
    0x4000310d <+29>:	s32i	a5, a4, 0x300
                   ;;; a3 := 115
    0x40003110 <+32>:	movi	a3, 115
    0x40003113 <+35>:	memw
                   ;;; *(a4 + 0x314) = *(0x60000914) = a3 = 115
    0x40003116 <+38>:	s32i	a3, a4, 0x314
                   ;;; a0 := 0
    0x40003119 <+41>:	movi.n	a0, 0
                   ;;; *(0x3fffc708) = 0
    0x4000311b <+43>:	s32i.n	a0, a2, 0
                   ;;; if (a12 == 1) goto 0x40003130
    0x4000311d <+45>:	beqi	a12, 1, 0x40003130 <ets_wdt_disable+64>
                   ;;; if (a12 == 2) goto 0x40003144
    0x40003120 <+48>:	beqi	a12, 2, 0x40003144 <ets_wdt_disable+84>
                   ;;; sets the return value
                   ;;; a2 := a12
    0x40003123 <+51>:	mov.n	a2, a12
                   ;;; the epilogue restores register values from stack
                   ;;; restore register a0 from stack
    0x40003125 <+53>:	l32i.n	a0, a1, 0
                   ;;; restore register a12 from stack
    0x40003127 <+55>:	l32i.n	a12, a1, 4
                   ;;; restore stack pointer 
    0x40003129 <+57>:	addi	a1, a1, 16
                   ;; return to calling method, register a0 contains return address
    0x4000312c <+60>:	ret.n

    (gdb) x /1wx 0x40002f30
    0x40002f30 <ets_get_cpu_frequency+36>:	0x3fffc708

    (gdb) x /1wx 0x4000115c
    0x4000115c <main+368>:	0x60000600

Now lets de-compile that assembler snippet into some pseudo C.

At address `0x400030f9` register a2 is set without reading it before. That means either the function has no parameters
or the first parameter is ignored. Before leaving the function at address `0x4000312c` register `a2` is set to some value.

The prologue/epilogue has no representation in C. The rest is translated in simple form first.
I ignore signed/unsigned int issues to keep it short and simple.

    int ets_wdt_disable(void) {
        int a6 = 0xFFFFFFFE;
        int a2 = 0x3fffc708;
        int a4 = 0x60000600
        int a12 = *((int*)a2);
        // consider memory at a2 volatile
        int a5 = *((int*) (a4 + 0x300)) // *(0x60000900)
        a5 = a5 & 0xFFFFFFFE
        // consider memory at a4 + 0x300 volatile
        *((int*) (a4 + 0x300)) = a5;
        int a3 = 115;
        // consider memory at a4 + 0x300 volatile
        *((int*)(a4 + 0x314)) = a3 = 115
        int a0 = 0;
        *((int*)a2) = a0 = 0;
        if (a12 == 1) goto 0x40003130
        if (a12 == 2) goto 0x40003144
        return a12;
    0x40003130:
    0x40003144:
    }