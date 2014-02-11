MyCleanerCPP
============

1. Build the project with visual studio
2. Find the file AutoClean.exe in the Debug Folder $path
3. Open CMD in Administrator Mode
4. cd $path
5. Commands
  5.1 AutoClean.exe -p "C:\tmp" // Define the path which will be cleaned
  5.2 AutoClean.exe -i // Install Service
  5.3 AutoClean.exe -d // Unistall Service

Open windows run and enter services.msc to open the service manager panel
Start the service "AutoClean"
Open the Log file in C:\Log.txt for monitoring

