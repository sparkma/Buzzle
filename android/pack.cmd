@echo off
if @%1 == @ goto help
if @%2 == @ goto help

if @%3 == @ goto default
set REMOVEALL=1
goto begin
:default
set REMOVEALL=0
:begin

setlocal enabledelayedexpansion

rem build native first
rem call build_native.cmd

rem pack every channel 

set APPNAME=com.crazyamber.ttlus
set PUBLISH_PATH=..\publish\%APPNAME%
mkdir %PUBLISH_PATH% > nul

echo APPNAME         : %APPNAME%
echo PUBLISH_PATH    : %PUBLISH_PATH%

rem echo MSG: Patching texture files
rem copy ..\Material\ui\ui_game7.png assets\ui\ui.png /Y > nul
rem if %ERRORLEVEL% NEQ 0 ( echo ERR: texture patch FAILED! [png] & goto exit )
rem copy ..\Material\ui\ui_game7.plist assets\ui\ui.plist /Y > nul
rem if %ERRORLEVEL% NEQ 0 ( echo ERR: texture patch FAILED! [plist] & goto exit )
rem echo MGS: Texture files PATCHED!

for /D %%I in (%PUBLISH_PATH%\*) do (
	echo @
	echo CHANNEL         : %%~nI 
	
	set PUBLISH_NAME=!PUBLISH_PATH!\%%~nI\!APPNAME!_%%~nI_%1.apk
	echo PUBLISH_NAME    : !PUBLISH_NAME!
	echo .
	
	if exist !PUBLISH_NAME! (
		if %REMOVEALL% EQU 1 (
			del !PUBLISH_NAME!
			echo MSG: !PUBLISH_NAME! DELETED!
		) else (
			echo MSG: Skip !PUBLISH_NAME!
		)
	)
	if not exist !PUBLISH_NAME! (
		echo MSG: rebuilding !PUBLISH_NAME!

		rem update channel name
		echo MSG: coping AndroidManifest.xml file
		cp AndroidManifest.xml.template AndroidManifest.xml
		echo MSG: seding AndroidManifest.xml 
		call sed -i "s/__UC__/%%~nI/g" AndroidManifest.xml
		call sed -i "s/__VERSIONNAME__/%1/g" AndroidManifest.xml
		call sed -i "s/__VERSIONCODE__/%2/g" AndroidManifest.xml
		echo MSG: remove -rf bin
		call rm -rf bin
		echo MSG: ant release, rebuilding project...
		call ant release > ant.build.log.txt

		echo MSG: checking ant output ./bin/!APPNAME!-release.apk
		ls -l ./bin/!APPNAME!-release.apk
		copy .\bin\!APPNAME!-release.apk !PUBLISH_NAME! /Y > nul
		if !ERRORLEVEL! NEQ 0 (
			echo ERR: copy .\bin\!APPNAME!-release.apk !PUBLISH_NAME! failed 
			echo ERR: ****** plz check ant.build.log.txt out for details !!!!!
			goto exit
		) else ( 
			echo INF: !PUBLISH_NAME! CREATED!.
		)
		pause
)
)
goto exit

:help
echo pack vername(aa.bb.cc) versioncode(aabbcc) [rebuild?]

:exit
echo MSG: Leaved.