@echo off

cd %~dp0



isd_download.exe isd_config.ini -tonorflash -dev br23 -boot 0x12000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -format all
:: -format all
::-reboot 2500

ping /n 2 127.1>null
IF EXIST null del null

