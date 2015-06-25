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

After spelling the magic words 
`make flash`
and
`screen /dev/ttyUSB0 74880`
and pushing the magic _reset_ button he was confronted with







