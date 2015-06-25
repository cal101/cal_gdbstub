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
















