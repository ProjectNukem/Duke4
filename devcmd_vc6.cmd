@echo off
call VS6\VC98\BIN\VCVARS32.BAT
set MASM_BIN=%MSVCDir%\BIN
cd Duke4
call cmd.exe
