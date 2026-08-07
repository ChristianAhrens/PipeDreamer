@echo off

set INNOSETUPDIR=C:\Program Files (x86)\Inno Setup 6
set WORKSPACE=..\..\..
echo.

echo Using variables:
echo INNOSETUPDIR = %INNOSETUPDIR%
echo WORKSPACE = %WORKSPACE%
echo.

echo Build project
CALL build_PipeDreamer.bat

echo Build installer
"%INNOSETUPDIR%\ISCC.exe" create_PipeDreamer-installer.iss
echo.

echo Move setup executable to workspace root
move Output\PipeDreamerSetup*.exe %WORKSPACE%
