# powershell -ExecutionPolicy ByPass -File Install-win64-mingw.ps1
# get-executionpolicy
# Start-Process powershell -Verb runAs
# Set-ExecutionPolicy RemoteSigned

# must be run as Admin

#Parameters
#The script could take 2 arguments.
param(  [string]$installPrefix="D:\usr",
        [string]$mingwPath ="C:\Qt5\Tools\mingw730_64\bin" )



# Add “C:\usr\cmake-3.14.0-rc1-win64-x64\bin” to you PATH under Control Panel->System->Advanced
#Add-Content -Path $Profile.CurrentUserAllHosts -Value '$Env:Path +=";D:\usr\local\bin\"'

#$Env:Path += ";D:\usr\local\bin\"
#[Environment]::SetEnvironmentVariable("Path", $env:Path, [System.EnvironmentVariableTarget]::Machine)

[Environment]::SetEnvironmentVariable(
    "Path",
    [Environment]::GetEnvironmentVariable("Path", [EnvironmentVariableTarget]::Machine) + ";D:\usr\local\bin\",
    [EnvironmentVariableTarget]::Machine)


#d) Link cmake to qt mingw dir

# Add “C:/Qt5/Tools/mingw730_64/bin” to you PATH under Control Panel->System->Advanced

# For convenience, symlink make to mingw32-make
#   ( do it as Admin, "Command prompt" right mouse bottom run as admin )
# ~~~
# cd  C:/Qt5/Tools/mingw730_64/bin
# mklink make.exe mingw32-make.exe
# ~~~


