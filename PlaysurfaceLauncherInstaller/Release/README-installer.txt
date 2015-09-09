Double-click the setup.exe file to install. 

Installation Directory:
By default, the installer will create a directory called
"FTL Labs" at the top level of drive C and install
everything in that directory.  Installing there instead of
in the "Program Files (x86)" directory avoids a lot of 
permissions issues.

MutltitouchApps Directory:
The installer will create a desktop shortcut for starting 
the PlaysurfaceLauncher, as well as a shortcut for opening 
a MultitouchApps directory.  The MultitouchApps directory 
will initially be empty.  If you do not already have any 
TUIO-enabled apps for testing, the zip file with the 
setup.exe and PlaysurfaceLauncherInstaller.ms files should 
have a MultitouchApps directory that you can copy 
subdirectories out of.  

MultitouchApps Subdirectories
When the PlaysurfaceLauncher starts up, it will scan each 
subdirectory of the MultitouchApps directory for a multitouch 
app and an associated image file.  The TUIO-enabled multitouch 
app should end with ".exe" for a Unity or C++ app, or ".swf" 
for a Flash app.  The image file should be a PNG 
Portable Network Graphics) file whose name ends with 
"-playsurface-icon.png".  The icon images will be displayed 
at 200 x 200 pixels no matter what their original size.

Startup Directory:
The installer will also place an alias for starting the
PlaysurfaceLauncher in the StartUp directory:

C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp

This alias should be removed if you do not want the 
PlaysurfaceLauncher to launch automatically when the
computer is rebooted.
