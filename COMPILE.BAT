@ECHO OFF
REM       Univerzalni davkovy soubor pro radkouvou kompilaci
REM                 (c) 1998 - 2006  Tomas Hujer
REM                          1.50.060629
REM ------------------------------------------------------------------------
REM        1. Parametr - Cesta k tomuto souboru (napr. "C:\PRGJAZ\MODEM")
REM        2. Parametr - Jmeno souboru bez pripony (napr. "MODEM")
REM        3. Parametr - Pripona souboru (napr. ".A51")
REM        4. Parametr - RUN_ISP_LOADER ... Po kompilaci spusti ISP loader
REM                      INDIVIDUAL ... Spusti se batch v cilovem adresari
REM ------------------------------------------------------------------------
IF "%1"=="" GOTO HELPEXIT
IF "%4"=="INDIVIDUAL" GOTO RUNBATCH
IF EXIST %1COMPILE.BAT GOTO NEXT
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
COPY %0 %1COMPILE.BAT
ECHO Vytvoren individualni davkovy soubor v adresari %1
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

:NEXT
IF NOT "%5"=="NOLINK" %1
CD %1
ECHO %1
COMPILE.BAT %1 %2 %3 INDIVIDUAL %5
Chyba: Nelze spustit individualni davkovy soubor %1COMPILE.BAT
PAUSE > NUL
GOTO QUIT

:RUNBATCH
REM Automaticke urceni diskoveho oddilu
REM IF EXIST F:\PRGJAZ\Utils\compile.bat SET %PRGDRV=F
IF EXIST H:\PRGJAZ\Utils\compile.bat SET %PRGDRV=H
IF EXIST G:\PRGJAZ\Utils\compile.bat SET %PRGDRV=G
IF EXIST F:\PRGJAZ\Utils\compile.bat SET %PRGDRV=F
IF EXIST E:\PRGJAZ\Utils\compile.bat SET %PRGDRV=E
IF EXIST D:\PRGJAZ\Utils\compile.bat SET %PRGDRV=D
IF EXIST C:\PRGJAZ\Utils\compile.bat SET %PRGDRV=C

REM --------------- Rozliseni kompilatoru podle pripony --------------------
IF %3==.PAS GOTO PAS_PREKLAD
IF %3==.pas GOTO PAS_PREKLAD
IF %3==.A51 GOTO A51_6_PREKLAD
IF %3==.a51 GOTO A51_6_PREKLAD
IF %3==.C51 GOTO C51_6_PREKLAD
IF %3==.c51 GOTO C51_6_PREKLAD
IF %3==.C   GOTO CPlus_PREKLAD
IF %3==.c   GOTO CPlus_PREKLAD
IF %3==.cpp GOTO CPlus_PREKLAD
IF %3==.CPP GOTO CPlus_PREKLAD
IF %3==.C32 GOTO C32_PREKLAD
IF %3==.c32 GOTO C32_PREKLAD
IF %3==.BAT GOTO BAT_ECHO
IF %3==.bat GOTO BAT_ECHO
GOTO UNSIGNED_EXTENSION
REM ------------------------------------------------------------------------



REM ------------------------ Preklad pro PAS -------------------------------
:PAS_PREKLAD
SET TPCBIN=:\PRGJAZ\PASCAL\BP7
ECHO Kompilace Turbo Pascal
REM %1%:
CD %1
ECHO %1 %2 %3
IF NOT EXIST %PRGDRV%%TPC%\%2.PAS ECHO Soubor nenalezen "%2.PAS"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
%PRGDRV%%TPCBIN%\TPC.EXE %2.PAS
ECHO.
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE > NUL
%2.EXE
PAUSE > NUL
GOTO QUIT
REM ------------------------------------------------------------------------


REM ------------------------ Preklad pro C51 v4.01 -------------------------
:C51_4_PREKLAD
SET C51LIB=%PRGDRV%:\PRGJAZ\CPU51\C51_v401\LIB
SET C51INC=%PRGDRV%:\PRGJAZ\CPU51\C51_v401\INC
SET C51PATH=%PRGDRV%:\PRGJAZ\CPU51\C51_v401\BIN
ECHO Kompilace C51 v4.01
REM %PRGDRV%:\PRGJAZ\CPU51\LOADER\ENABLE.EXE 378h
%1%:
CD %1
IF NOT EXIST %2.C51 ECHO Soubor nenalezen "%2.C51"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO LIB=%C51LIB%
ECHO INC=%C51INC%
ECHO BIN=%C51PATH%
ECHO C51: "%2.C51" TO "%2.obj"
%C51PATH%\C51.EXE %2.C51 DEBUG CODE LISTINCLUDE |MORE
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE > NUL
CLS

IF NOT EXIST %2.OBJ ECHO Soubor nenalezen "%2.obj"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO L51: "%2.obj" TO "%2.omf"

IF EXIST %2.CMD GOTO CX4
IF EXIST LINKER.CMD GOTO CLINKER4
ECHO.
ECHO Soubor LINKER.CMD nenalezen. Soubor nebude linkovan.
GOTO QUIT

:CLINKER4
 %C51PATH%\L51.EXE @LINKER.CMD | more
GOTO CNOT4

:CX4
%C51PATH%\L51.EXE @%2.CMD | more

:CNOT4
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

PAUSE >nul
CLS

IF NOT EXIST %2.OMF ECHO Soubor nenalezen "%2.omf"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO OHS51: "%2.omf" TO "%2.hex"
%C51PATH%\OH51.EXE %2.OMF
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

PAUSE >NUL
IF %%4==RUN_ISP_LOADER GOTO RUN_ISP_LOADER
GOTO QUIT

REM ------------------------------------------------------------------------


REM ------------------------ Preklad pro C51 v6.12 -------------------------
:C51_6_PREKLAD
SET C51LIB=%PRGDRV%:\PRGJAZ\CPU51\C51_v612\C51\LIB
SET C51INC=%PRGDRV%:\PRGJAZ\CPU51\C51_v612\C51\INC
SET C51PATH=%PRGDRV%:\PRGJAZ\CPU51\C51_v612\C51\BIN
ECHO Kompilace C51 v6.12
CD %1
IF NOT EXIST %2.C51 ECHO Soubor nenalezen "%2.C51"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO C51: "%2.C51" TO "%2.obj"
%C51PATH%\C51.EXE %2.C51 DEBUG CODE LISTINCLUDE |MORE
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

PAUSE >nul
CLS

IF "%5"=="NOLINK" GOTO QUIT
ECHO Linker v4.13
IF NOT EXIST %2.OBJ ECHO Soubor nenalezen "%2.obj"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO L51: "%2.obj" TO "%2.omf"
IF EXIST %2.CMD GOTO CX6
IF EXIST LINKER.CMD GOTO CLINKER6
ECHO.
ECHO Nelinkovano, %2.CMD neexistuje.
ECHO.
REM PAUSE > NUL
GOTO QUIT

:CLINKER6
%C51PATH%\BL51.EXE @LINKER.CMD | more
GOTO CNOT6

:CX6
%C51PATH%\BL51.EXE @%2.CMD | more

:CNOT6
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >nul
CLS
IF NOT EXIST %2.OMF ECHO Soubor nenalezen "%2.omf"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO OHS51: "%2.omf" TO "%2.hex"
%C51PATH%\OH51.EXE %2.OMF
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

PAUSE >NUL
IF %%4==RUN_ISP_LOADER GOTO RUN_ISP_LOADER
GOTO QUIT
REM ------------------------------------------------------------------------




REM ------------------------ Preklad pro A51 v4.4 -------------------------
:A51_4_PREKLAD
SET A51BIN=:\PRGJAZ\CPU51\A51
CLS
ECHO Kompilace A51 V4.4
CD %1
REM IF NOT EXIST REG51.INC COPY %PRGDRV%:\PRGJAZ\CPU51\A51\INC\REG51.INC >NUL
REM IF NOT EXIST REG52.INC COPY %PRGDRV%:\PRGJAZ\CPU51\A51\INC\REG52.INC >NUL
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO A51: "%2%3" TO "%2.obj"
%PRGDRV%%A51BIN%\A51.EXE %2%3 DEBUG SYMBOLS XREF ERRORPRINT |MORE
IF ERRORLEVEL==1 ECHO ERROR IN "%2%3"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >NUL
IF %%4==RUN_ISP_LOADER GOTO RUN_ISP_LOADER
GOTO QUIT

REM ------------------------ Preklad pro A51 v6.12 -------------------------
:A51_6_PREKLAD
SET A51INC=:\PRGJAZ\CPU51\C51_V612\INC
SET A51BIN=:\PRGJAZ\CPU51\C51_V612
CLS
ECHO Kompilace A51
CD %1
IF NOT EXIST REG51.INC COPY %PRGDRV%%A51INC%\REG51.INC >NUL
IF NOT EXIST REG52.INC COPY %PRGDRV%%A51INC%\REG52.INC >NUL
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO A51: "%2%3" TO "%2.obj"
%PRGDRV%%A51BIN%\A51.EXE %2%3 NODEBUG SYMBOLS XREF NOMOD51 ERRORPRINT |MORE
IF ERRORLEVEL==1 ECHO ERROR IN "%2%3"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >NUL
IF %%4==RUN_ISP_LOADER GOTO RUN_ISP_LOADER
GOTO QUIT
REM ------------------------------------------------------------------------


REM ------------------------ ISP Loader ------------------------------------
:RUN_ISP_LOADER
%PRGDRV%:\PRGJAZ\CPU51\FLASHER\rd2f.exe -f %3.hex -c COM1 -b 19200 -m 0 -p 0 -t 2
GOTO QUIT
REM ------------------------------------------------------------------------


REM ------------------------ Preklad pro C++ -------------------------------

REM Borland C++  Version 3.00 Copyright (c) 1991 Borland International
REM Syntax is: BCC [ options ] file[s]     * = default; -x- = turn switch x off
REM   -1      80186/286 Instructions    -2      80286 Protected Mode Inst.
REM   -Ax     Disable extensions        -B      Compile via assembly
REM   -C      Allow nested comments     -Dxxx   Define macro
REM   -Exxx   Alternate Assembler name  -G      Generate for speed
REM   -Hxxx   Use pre-compiled headers  -Ixxx   Include files directory
REM   -K      Default char is unsigned  -Lxxx   Libraries directory
REM   -M      Generate link map         -N      Check stack overflow
REM   -Ox     Optimizations             -P      Force C++ compile
REM   -Qxxx   Memory usage control      -S      Produce assembly output
REM   -Txxx   Set assembler option      -Uxxx   Undefine macro
REM   -Vx     Virtual table control     -Wxxx   Create Windows application
REM   -X      Suppress autodep. output  -Yx     Overlay control
REM   -Z      Suppress register reloads -a      Generate word alignment
REM   -b    * Treat enums as integers   -c      Compile only
REM   -d      Merge duplicate strings   -exxx   Executable file name
REM   -fxx    Floating point options    -gN     Stop after N warnings
REM   -iN     Max. identifier length    -jN     Stop after N errors
REM   -k      Standard stack frame      -lx     Set linker option
REM   -mx     Set Memory Model          -nxxx   Output file directory
REM   -oxxx   Object file name          -p      Pascal calls
REM   -r    * Register variables        -u    * Underscores on externs
REM   -v      Source level debugging    -wxxx   Warning control
REM   -y      Produce line number info  -zxxx   Set segment names

REM Borland C++ Version 3.0  Copyright (c) 1990, 1991 Borland International
REM Syntax:   BC [option [option...]] [srcname|projname [srcname...]]
REM
REM srcname  is any ASCII file (default extension assumed)
REM projname is a project file (must have .PRJ extension)
REM option   is any of the following: (/x- = turn option x off):
REM  /b              batch mode build project then exit
REM  /d              dual monitor mode with BC++ on the other monitor
REM  /e[=n]       *  use n pages of expanded memory for swapping
REM  /h              displays this help
REM  /l              LCD/B&W colors for screen
REM  /m              batch mode make project then exit
REM  /p              restore palette on EGA after swap
REM  /rx             use alternate disk 'x' for swap file
REM  /s           *  allow maximum swapping (thrashing)
REM  /x[=n]          use n KB for the code and data memory pool
REM
REM               *  means enabled by default

:CPlus_PREKLAD
SET CBIN=:\PrgJaz\C\BC\BIN
SET CLIB=:\PrgJaz\C\BC\LIB
SET CINC=:\PRGJAZ\C\BC\INCLUDE
SET PATH=%PATH%;%PRGDRV%%CBIN%;
CLS
ECHO Kompilace C++
CD %1
ECHO %1%2%3
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
if "%OS%"=="Windows_NT" goto CPLUS_NT

cls
%PRGDRV%%CBIN%\BCC.EXE -I%PRGDRV%%CINC% -L%PRGDRV%%CLIB% -ml %2%3 | more
IF ERRORLEVEL==1 GOTO CPLUSERR
pause
goto CPLUS_OS_NEXT

:CPLUS_NT
cls
%PRGDRV%%CBIN%\BC.EXE /b -I%PRGDRV%%CINC% -L%PRGDRV%%CLIB% %2%3 > OUTPUT.TXT
type OUTPUT.TXT | more
IF ERRORLEVEL==1 GOTO CPLUSERR

:CPLUS_OS_NEXT

ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
IF ERRORLEVEL==1 GOTO CPLUSERR
ECHO Press to run EXE...
PAUSE >NUL
cls
%2.EXE
echo.
echo press a key to exit...
pause > nul
GOTO QUIT

:CPLUSERR
ECHO ERROR IN "%2%3"                     Press to Exit...
PAUSE >NUL
GOTO QUIT
REM ------------------------------------------------------------------------


REM ------------------------ Preklad pro C++ 32 -------------------------------
:C32_PREKLAD
SET C32BIN=:\PrgJaz\C\BC5\BIN
SET C32INC=:\PRGJAZ\C\BC5\Include
CLS
ECHO Kompilace C++ 32
SET PATH=%PATH%; %PRGDRV%%C32BIN%;
ECHO %1%2%3
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
%PRGDRV%%C32BIN%\BCC32.EXE -n%1 -I%PRGDRV%%C32INC% %2%3 | more
IF ERRORLEVEL==1 ECHO ERROR IN "%2%3"
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >NUL
rem %2.EXE
GOTO QUIT
REM ------------------------------------------------------------------------

:BAT_ECHO
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO     Soubory BAT nejsou soucasti netopyra a neni tudiz treba je prekladat!
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >NUL
GOTO QUIT
REM ------------------------------------------------------------------------

REM ------------------- Pro priponu neni definovan preklad -----------------
:UNSIGNED_EXTENSION
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
ECHO       Pro soubor %2%3 neni definovan zadny prekladac !
ECHO ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
PAUSE >NUL
GOTO QUIT
REM ------------------------------------------------------------------------

:HELPEXIT
ECHO              Univerzalni davkovy soubor pro radkouvou kompilaci
ECHO  ------------------------------------------------------------------------
ECHO   Pokud v cilovem adresari tento batch neexistuje, vytvori ta svoji kopii,
ECHO   pokud ano, preda rizeni teto kopii.
ECHO   Batch je tak mozne u kazdeho zdrojoveho kodu individualne prizpusobit.
ECHO  ------------------------------------------------------------------------
ECHO.
ECHO                        Chybi pozadovane parametry !
ECHO.
ECHO  ------------------------------------------------------------------------
ECHO         1. Parametr - Cesta k tomuto souboru (napr. "C:\PRGJAZ\MODEM")
ECHO         2. Parametr - Jmeno souboru bez pripony (napr. "MODEM")
ECHO         3. Parametr - Pripona souboru (napr. ".A51" nebo ".c", ...)
ECHO         4. Parametr - RUN_ISP_LOADER ... Po kompilaci spusti ISP loader
ECHO                       INDIVIDUAL ... Spusti se batch v cilovem adresari
ECHO  ------------------------------------------------------------------------
ECHO                                          (c) 1998 - 2005  Tomas Hujer

PAUSE > NUL



:QUIT



