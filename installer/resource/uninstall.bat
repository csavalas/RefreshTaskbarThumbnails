@echo off
echo *******************************************
echo RefreshTaskbarThumbnails Uninstaller Script
echo *******************************************
echo:
goto check_Permissions
:check_Permissions
    echo Detecting permissions...
    net session >nul 2>&1
    if %errorLevel% == 0 (
        echo PASSED
    ) else (
        echo FAILED: Please run the batch file as Administrator
        echo.
        SET /P tempo=Press Enter to Exit
        exit
    )

echo:
echo This will completely remove RefreshTaskbarThumbnails from your PC...
pause
echo:
echo Setting environment variables
set installdir=RefreshTaskbarThumbnails
echo Changing to current working directory...
cd /D "%~dp0"
echo Removing Start Menu links...
del /f /s /q "%userprofile%\Start Menu\Programs\%installdir%.lnk" >NUL  2>NUL
del /f /s /q "%userprofile%\Start Menu\Programs\%installdir% Uninstall.lnk" >NUL  2>NUL
echo Removing Task...
schtasks /Delete /TN "\%installdir%\%installdir%" /F >NUL  2>NUL
echo Removing files at directory: %LOCALAPPDATA%\%installdir%\...
rd /s /q "%LOCALAPPDATA%\%installdir%\"  >NUL  2>NUL
echo Removing Task
