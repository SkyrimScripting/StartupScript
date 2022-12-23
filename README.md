# Skyrim Game Start Script

<a 
    title="Download Skyrim Game Start Script SKSE Plugin - Install in your Mod Manager"
    href="https://github.com/SkyrimScripting/StartupScript/releases/download/0.0.1/StartupScript.7z">
    <img
        alt="Download Skyrim Game Start Script SKSE Plugin - Install in your Mod Manager"
        src="https://github.com/SkyrimScripting/Download/raw/main/DownloadButton_256.png"
        width="100"
        />
</a>

> Run a script whenever the Main Menu loads
>
> e.g. `coc riverwood`

![Logo](Images/Logo.png)

### How it works

If `Data/StartupScript.bat` is provided (_e.g. via a MO2 plugin_) then it is run when the Main Menu loads.

Else if an environment variable named `SKYRIM_STARTUP_SCRIPT` is configured to the path of a script, that script is run when the Main Menu loads.

That's all!

_e.g. I use it to run an AutoHotkey script to `coc riverwood` from the Main Menu `*`_

> `*` _because trying to `coc` to riverwood using SKSE doesn't work from the Main Menu - if you get it working, PLEASE let me know!_
