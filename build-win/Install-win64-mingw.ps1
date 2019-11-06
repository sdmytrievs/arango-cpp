# powershell -ExecutionPolicy ByPass -File Install-win64-mingw.ps1
# get-executionpolicy
# Start-Process powershell -Verb runAs
# Set-ExecutionPolicy RemoteSigned


#Parameters
#The script could take 2 arguments.
param(  [string]$installPrefix="D:\usr",
        [string]$mingwPath ="C:\Qt5\Tools\mingw730_64\bin",
        [string]$cmakeVersion="3.14.0",
        [string]$curlVersion="7.67.0",
        [string]$ssh2Version="1.9.0" )



#Functions
#2)	Functions

#Create a function named CheckFolder that checks for the existence of a specific directory/folder that is passed 
#to it as a parameter. Also, include a switch parameter named create. If the directory/folder does not exist and 
#the create switch is specified, a new folder should be created using the name of the folder/directory that was 
#passed to the function.
function Check-Folder([string]$path, [switch]$create){
    $exists = Test-Path $path

    if(!$exists -and $create){
        #create the directory because it doesn't exist
        mkdir $path | out-null
        $exists = Test-Path $path
    }
    return $exists
}


# Load data from url to installationPath
Function Load-To ( [string] $url, [string] $fname )
{
  Write-Host ('Downloading {0} installer from {1} ..' -f $fname, $url);
  #Invoke-WebRequest -Uri $url -OutFile $outpath
  (New-Object System.Net.WebClient).DownloadFile($url, $fname);
  Write-Host ('Downloaded {0} bytes' -f (Get-Item $fname).length);
}



#3)	Main processing

#a) Main links.

$binPrefix="$installPrefix\local\bin"
$libPrefix="$installPrefix\local\lib"
$includePrefix="$installPrefix\local\include"

$major, $minor, $patch = $cmakeVersion.split('.')
$cmakeUrl = ('https://cmake.org/files/v{0}.{1}/cmake-{2}-win64-x64.zip' -f $major, $minor, $cmakeVersion)
$cmakeName = "$installPrefix/cmake-$cmakeVersion-win64-x64.zip"

$curlUrl = ('https://curl.haxx.se/windows/dl-{0}/curl-{0}-win64-mingw.zip' -f $curlVersion)
$curlName = "$installPrefix/curl-$curlVersion-win64-mingw.zip"

$ssh2Url = ('https://curl.haxx.se/windows/dl-{0}/libssh2-{1}-win64-mingw.zip' -f $curlVersion, $ssh2Version )
$ssh2Name = "$installPrefix/libssh2-$ssh2Version-win64-mingw.zip"


#b) Test for the existence of the destination folders; create it if it is not found.
$destinationexists = Check-Folder $installPrefix -create

if (!$destinationexists){
    Write-Host "The destination directory is not found. Script can not continue."
    Exit
}

$destinationexists = Check-Folder "$installPrefix\local" -create
$destinationexists = Check-Folder $binPrefix -create
$destinationexists = Check-Folder $libPrefix -create
$destinationexists = Check-Folder $includePrefix -create

#c) Install cmake

Load-To $cmakeUrl $cmakeName 
Expand-Archive $cmakeName -DestinationPath $installPrefix


#d) Install curl

Load-To $curlUrl $curlName
Expand-Archive $curlName -DestinationPath $installPrefix

#e) Install ssh2

Load-To $ssh2Url $ssh2Name
Expand-Archive $ssh2Name -DestinationPath $installPrefix
