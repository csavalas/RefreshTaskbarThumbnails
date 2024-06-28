# Refresh Taskbar Thumbails

## Description

* This program serves as a workaround to an _(as of 6/27/24)_ unfixed issue in Windows 10/11 whereby taskbar thumbnail previews are intermittently lost.
    * Z-order and windows priority are preserved
    * The fix is acheived by toggling visibility of all minimized windows a specified amount of times (By default, 3).
        * Depending on the speed of your machine, you may want to modify this and other defaults for optimal results. Please see the `Command Line Arguments` section for more details.

## Notes on UAC
* Due to Windows security restrictions, this program can only manage windows belonging to elevated processes (eg. taskmgr.exe) when run as administrator. As such, the following options are available:
    1. Install the utility as an elevated task (Installer instructions below)
    2. Manually run the utility as Administrator, needing to confirm the UAC dialog on each run
    3. Simply run the utility as-is, understanding that elevated windows will lose focus and position in z-order


## Usage (Task Install)

### `Installation Instructions`:
* Download installer.zip from [here](https://github.com/csavalas/RefreshTaskbarThumbnails).
* Unzip contents
* Run `setup.bat` as administrator
    * NOTE: Your _currently logged-in_ account must have administrator privileges
    * When prompted for command line arguments:
        * Leave blank to use defaults
            * *or*
        * Refer to the `Command line arguments` section below
    * Start menu shortcuts will be created:
        * RefreshTaskbarThumbnails.lnk (Manually runs the utility with no UAC prompt)
        * Uninstall RefreshTaskbarThumbnails.link (Uninstalls utility) 

### `Shortcut`:
* Run `RefreshTaskbarThumbnails.lnk` from Start Menu (created during installation)
* Pin `RefreshTaskbarThumbnails.lnk` task shortcut to the taskbar/copy to the desktop

### `Autohotkey`:
* Assign a key/key combo to run the elevated task:
    * App:
        * `C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe`
    * Arguments
        * `-WindowStyle hidden Start-ScheduledTask -TaskPath RefreshTaskbarThumbnails -TaskName RefreshTaskbarThumbnails`

### `Hotkey Mapper` (_Lenovo ThinkPads only_) [Link to utility](https://github.com/csavalas/HotkeyMapper):
* Assign a function hotkey to run the elevated task:
    * App:
        * `C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe`
    * Arguments
        * `-WindowStyle hidden Start-ScheduledTask -TaskPath RefreshTaskbarThumbnails -TaskName RefreshTaskbarThumbnails`

## Usage (No Install, `RefreshTaskbarThumbnails.exe` located in `installer.zip\resource`)

### `Shortcut/Run Directly`:
* Pin `RefreshTaskbarThumbnails.exe` to the taskbar, copy/create shortcut on the desktop

### `Autohotkey`:
* Assign any key/key combo to open `RefreshTaskbarThumbnails.exe`
    * App (required):
        * `your\path\to\utility\RefreshTaskbarThumbnails.exe`
    * Arguments (optional):
        * See `Command line arguments` section below for guidance 

### `Hotkey Mapper` (_Lenovo ThinkPads only_) [Link to utility](https://github.com/csavalas/HotkeyMapper):
* Assign a function hotkey to open an app:
    * App (required):
        * `your\path\to\utility\RefreshTaskbarThumbnails.exe`
    * Arguments (optional):
        * See `Command line arguments` section below for guidance 

## Command line arguments
### ```RefreshTaskbarThumbnails.exe --iterations (some number) --waittime (some number in ms) --showui (1 or 0) --help```

#### `--iterations`
* How many times should the utility toggle visibility of minimized windows

#### `--waittime`
* How long to allow minimized windows to refresh content per iteration

#### `--showui`
* Whether to show on-screen progress while refreshing

#### `--help`
* Show CLA info