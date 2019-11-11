# powershell -ExecutionPolicy ByPass ".\Update-win64-mingw.ps1 -installPrefix "C:\usr" -mingwPath "C:\Qt5\Tools\mingw730_64\bin" "


#Parameters
# C:\Qt5\Tools\mingw730_64\bin\
# C:\usr\cmake-3.14.0-win64-x64\bin\
#The script could take 2 arguments.
param(  [string]$installPrefix="C:\usr",
        [string]$mingwPath ="C:\Qt5\Tools\mingw730_64\bin",
        [string]$cmakeVersion="3.14.0" )

#3)	Main processing

#a) Main links.

$localPrefix="$installPrefix\local"
$cmakePath = "$installPrefix\cmake-$cmakeVersion-win64-x64\bin\"

#b) Set Environment Path

$Env:Path += ";$cmakePath;$mingwPath"
#Add-Content -Path $Profile.CurrentUserAllHosts -Value '$Env:Path +=";$binPrefix;$mingwPath"'
$Env:Path

#cd  C:/Qt5/Tools/mingw730_64/bin
#mklink make.exe mingw32-make.exe

#c) Install dependencies
#chmod u+x install-dependencies-Win.sh
.\install-dependencies-Win.sh $localPrefix


