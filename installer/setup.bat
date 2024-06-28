@echo off
echo *********************************************
echo RefreshTaskbarThumbnails Admin Task Installer
echo *********************************************
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


echo Setting environment variables
set resourcedir=resource
set installdir=RefreshTaskbarThumbnails
set appmain=%installdir%.exe
set apptask=%installdir%.xml
set batuninstall=uninstall.bat
echo Changing to current working directory...
cd /d %~dp0


echo Creating Installation Directory at %LOCALAPPDATA%\%installdir%\...
if not exist "%LOCALAPPDATA%\%installdir%\" mkdir %LOCALAPPDATA%\%installdir%\  >NUL  2>NUL
echo Copying main app...
copy /Y "%resourcedir%\%appmain%" "%LOCALAPPDATA%\%installdir%\%appmain%"  >NUL  2>NUL
echo Copying uninstall script...
copy /Y "%resourcedir%\%batuninstall%" "%LOCALAPPDATA%\%installdir%\%batuninstall%"  >NUL  2>NUL
echo Creating Start Menu links...
copy %resourcedir%\%installdir%.lnk "%userprofile%\Start Menu\Programs\%installdir%.lnk"
powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%userprofile%\Start Menu\Programs\%installdir% Uninstall.lnk');$s.TargetPath='%LOCALAPPDATA%\%installdir%\%batuninstall%';$s.Save();$bytes = [System.IO.File]::ReadAllBytes('%userprofile%\Start Menu\Programs\%installdir% Uninstall.lnk');$bytes[0x15] = $bytes[0x15] -bor 0x20;[System.IO.File]::WriteAllBytes('%userprofile%\Start Menu\Programs\%installdir% Uninstall.lnk', $bytes);"

echo:
echo Example:
echo  --iterations (some number) --waittime (some number in ms) --showui (1 or 0) --help
echo:
set /p "args=Enter custom command line arguments (press [Enter] for none): "
copy /Y "%resourcedir%\%apptask%" "%resourcedir%\backup_%apptask%"  >NUL 2>NUL
powershell -Command "(gc %resourcedir%\%apptask%) -replace '<Arguments></Arguments>', '<Arguments>%args%</Arguments>' | Out-File -encoding unicode %resourcedir%\%apptask%"

echo Creating Task...
schtasks /Delete /TN "\%installdir%\%installdir%" /F >NUL  2>NUL

:create_task
schtasks /create /xml "%resourcedir%\%apptask%" /tn "\%installdir%\%installdir%" /ru "%computername%\%username%"
set "err=%errorlevel%"
if %err% neq 0 (
    goto create_task
)

echo Cleaning up...
copy /Y "%resourcedir%\backup_%apptask%" "%resourcedir%\%apptask%"  >NUL 2>NUL
del "%resourcedir%\backup_%apptask%" /F /Q >NUL 2>NUL

echo Installation complete...
pause