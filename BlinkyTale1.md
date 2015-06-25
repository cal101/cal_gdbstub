# Blinky and the gdb trap

This is a story about the adventorous little [Blinky](https://github.com/esp8266/esp8266-wiki/wiki/Blinky)
and how he survived the gdb trap.

Blinky is very curious and likes to play with new kids and try out new tricks.
One day he got aware of [cal_gdbstub](TODO) and found it strange enough to take a closer look.

He mixed up some magic ingredients

* esp-open-sdk - make sure to get a fresh one, tastes better
* Expressif SDK esp_iot_sdk_v1.1.1_15_06_05
* [cal_gdbstub](TODO)

and started to explore the mysteries of an ancient ESP01.

He took his old friend `printf` and prepared himself to call `gdb_stub_init()`.

After spelling the magic words 
`make clean all flash`
and
`screen /dev/ttyUSB0 74880`
and pushing the magic _reset_ button he was confronted with


    ets Jan  8 2013,rst cause:2, boot mode:(3,6)

    load 0x40100000, len 32580, room 16 
    tail 4
    chksum 0x58
    load 0x3ffe8000, len 1356, room 4 
    tail 8
    chksum 0xd2
    load 0x3ffe8550, len 1364, room 0 
    tail 4
    chksum 0xd3
    csum 0xd3
    R��"aAcjIh      ��
    SDK ver: 1.1.1 compiled @ Jun  5 2015 20:05:03
    phy ver: 356, pp ver: 8.6

    Blinky, SDK version:1.1.1
    mode : sta(18:fe:34:9d:89:85) + softAP(1a:fe:34:9d:89:85)
    add if0
    dhcp server start:(ip:192.168.4.1,mask:255.255.255.0,gw:192.168.4.1)
    add if1
    bcn 100
    1

There is a lot he ignored for now but that may be another tale to tell.

He saw his tag **Blinky, SDK version:1.1.1** and happily took a deap breath and started to blink.

His friend `printf` reports his actions to the almighty console but he accepted that:

   01010101010101010101010101010101010101010101010101010101010101010101010101010101
   01010101010101010101010101010101010101010101010101010101010101010101010101010101
   01010101010101010101010101010101010101010101010101010101010101010101010101010101
   0101010101010101010101010101010101010101010101010101010101010101010

After blinking for some time Blinky got bored.

He decided to take a break from blinking and calling `gdb_breakpoint` looks like an interesting new way to do that.

And so he added  
> gdb_breakpoint(); 
at the end of his `user_init` 

and spelled the magic flash words again.

He was confronted with

    Blinky, SDK version:1.1.1
    $T05#b9

Seeing his tag he wanted to start blinking again but couldn't. Nothing happens.

He got a little bit frightened and but then decided to wait. From past adventures he knew that his little [watch dog](https://de.wikipedia.org/wiki/Watchdog) will find and rescue him. 
If his dog does not get feed regularly he will get angry. To restore his familiar world he will `reset` it.

So Blinky waited and waited and waited ....

Nothing happened, no watch dog arrived.

So he learned:

**Don't expect watchdog timer resets when entering a break point!**

Blinky decided to look up the cryptic response `$T05#b9` in his magic book and found it to be word of the ancient language of [_gdb stubs_](https://sourceware.org/gdb/onlinedocs/gdb/Stop-Reply-Packets.html#Stop-Reply-Packets).

_T05_ told him that he was trapped (got a signal to enter debugger).
The gdb stub took control and asks for assistance.

He felt adventorous and gave a magic reply he found somewhere:

    $g#67

The explanation he found for this was _packet to request register contents_.

To his surprise he got an answer:

    $fa38244010fcff3f0100000016000000201fff3f010000000000200010dcff3f150000000000000000000000d91f0040508efe3f088efe3f928cfe3f00000000807c10400b0000000000000002000000000000003200000000000000000000000000000000000000000000000000000000000000807c10400000000000000000200000000000000000000000fa382440000000004400000000000000000000001104000000001040000000000800000055152201000000000000000000000000000000009919af80#b8

But he didn't understood it. He knew he needed someone to help him. The esp-open-sdk was caled to the rescue and it send `gdb` to help.

`gdb` told Blinky to exit the terminal and let him do his job.

`gdb` spelled his magic words

    xtensa-lx106-elf-gdb build/app.out

and got a response

    GNU gdb (crosstool-NG 1.20.0) 7.5.1
    Copyright (C) 2012 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "--host=x86_64-build_unknown-linux-gnu --target=xtensa-lx106-elf".
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Reading symbols from .../source-code-examples/blinky/build/app.out...done.
    (gdb)

So he knew what code to expect and connects now:

    (gdb) target remote /dev/ttyUSB0
    Remote debugging using /dev/ttyUSB0
    gdb_breakpoint () at arch_esp8266.c:35
    35	arch_esp8266.c: No such file or directory.

Lets ignore the missing source error for now.

We are in the methods `gdb_breakpoint ()`!

Where do we come from? 

Lets print a back trace 

    (gdb) bt
    #0  gdb_breakpoint () at arch_esp8266.c:35
    #1  0x402438fa in user_init () at user/user_main.c:77
    #2  0x4024050f in ?? ()
    #3  0x4024050f in ?? ()
    Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Now explore the caller of gdb_breakpoint:

    (gdb) up
    #1  0x402438fa in user_init () at user/user_main.c:77
    77	    gdb_breakpoint();
    (gdb) list
    72	    //0 for once and 1 for repeating
    73	    os_timer_arm(&some_timer, 1000, 1);
    74	    
    75	    //Start os task
    76	    system_os_task(user_procTask, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
    77	    gdb_breakpoint();
    78	}

Then `gdb` remembered poor little Blinky and released him by issuing a magic `continue`:

    (gdb) c
    Continuing.

And Blinky does what he lived for ...

    1010101010101010101010101
    010101010101010101010

To be continued ...


80	void user_rf_pre_init(void)
81	{









   




















