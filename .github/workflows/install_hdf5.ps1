mkdir download
cd download

$hdf5_url = "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.8/bin/windows/hdf5-1.10.8-Std-win10_64-vs15.zip" 
$cwd = Get-Location
$hdf5_file = "$cwd" + "/hdf5.zip"

Write-Host "Downloading HDF5..."
$web_cli= new-object System.Net.WebClient
$web_cli.DownloadFile($hdf5_url, $hdf5_file)
Write-Host "Downloading completed"

Start-Process "C:/Program Files/7-zip/7z.exe" -Wait -ArgumentList 'x ./hdf5.zip'

ls hdf
cd hdf

Write-Host "Installing HDF5..."
Start-Process -FilePath msiexec.exe -ArgumentList "/i HDF5-1.10.8-win64.msi /qn" -Wait
Write-Host "Installing HDF5 completed"

$HDF5_InstallDir = "C:/Program Files/HDF_Group/HDF5/1.10.8"
