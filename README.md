# PlaysurfaceLaucher
The primary goal of the PlaysurfaceLauncher is to provide a touch-based 
interface for launching TUIO-based multitouch apps without the need for
using a mouse or keyboard to navigate between apps.  The PlaysurfaceLauncher
was originally written to support a touch table that uses an infrared camera
below a projector surface to track finger movements 
(see http://playsurface.org).  For camera mode, the PlaysurfaceLauncher uses
a program called BigBlobby (https://github.com/jr-weber/BigBlobby), which 
processes the camera images and sends out TUIO UDP messages or Flash XML TCP 
messages.  However, if a Windows 8 touch device is used instead, the 
PlaysurfaceLauncher can be easily reconfigured to use a program called 
TouchHooks2Tuio, which monitors Windows 8 touch events and resends them as 
TUIO messages.

Run Requirements:

* Windows 7 or 8 if a table with an infrared camera and BigBlobby is used.
* Windows 8 if a touch device/monitor is used with the TouchHooks2Tuio converter.

Build Requirements:

* Microsoft Visual Studio 2013 Community Edition
      https://www.visualstudio.com/en-us/news/vs2013-community-vs.aspx
* Qt Framework version 5.4 (or higher)
      http://www.qt.io/download/
* Visual Studio Add-in 1.2.4 (or higher) for Qt 5
      http://www.qt.io/download-open-source/#section-2
	
![PlaysurfaceLauncher GUI Image](https://github.com/jr-weber/PlaysurfaceLauncher/blob/master/PlaysurfaceLauncher-GUI.jpg)

Basic Usage:

If using a touch table with a projector, the computer display should be 
set to mirroring, rather than a side-by-side configuration.  After 
double-clicking on PlaysurfaceLauncher.exe, the program will 
automatically place all other open windows down into the taskbar and 
then hide the taskbar so that the desktop will look simililar to the 
PlaysurfaceLauncher-GUI.jpg image that should be included with this 
README.md file.  By default, the PlaysurfaceLauncher will launch the 
BigBlobby application, which expects input from an infrared camera 
for tracking fingertip movements and sending them out as TUIO messages.
See https://github.com/jr-weber/BigBlobby for links on how to calibrate 
BigBlobby.

A touch or mouse click on the upper-left icon (black with 5 white circles)
should pull BigBlobby out of the hidden taskbar so that calibration can 
be done.  Touching or clicking on the same icon will hide BigBlobby 
again.  Do not quit BigBlobby after calibration: just hide it.  It must
stay open for touch detection to work.  BigBlobby will send out TUIO 
messages on 3 ports at the same time: port 3333 (most TUIO-enabled
multitouch apps), port 3334 (for the PlaysurfaceLauncher), and port 
3000 (for Flash XML apps).  For Flash apps, there will likely be 
some special permissions issues to deal with.  Those are discussed 
in the "Flash-TrustedLocationSettings.txt" file that should be in 
the same directory as this README.md file.

The icon right below below the BigBlobby icon is for reloading 
all the desktop icons for multitouch apps.  When the PlaysurfaceLauncher
first starts, or whenever the reload icon is touched, the program scans
each subdirectory of the MultitouchApps directory and looks for a runnable 
multitouch app along with an image file to display on the 
PlaysurfaceLauncher desktop.  The TUIO-enabled multitouch app should
end with ".exe" for a Unity or C++ app, or ".swf" for a Flash app.
The image file should be a PNG (Portable Network Graphics) file whose
name ends with "-playsurface-icon.png".  The icon images will be 
displayed at 200 x 200 pixels no matter what their original size.
Assuming that BigBlobby is working correctly and sending out TUIO 
messages, touching one of these icons on the PlaysurfaceLauncher 
desktop should launch the associated multitouch app.  

When a multitouch app is running, the PlaysurfaceLauncher will listen
in the background for a plus-sign gesture that will open a touch-based
dialog box for quiting the multitouch app and returning to the 
PlaysurfaceLauncher desktop.  For more information on the plus-sign
gesture, use a mouse to click near the top of the PlaysurfaceLauncher
desktop, and a menu bar with a "Gestures" menu will appear.  If you
select "Show Gesture Panel" from this menu, a fingerpainting panel
will appear with directions on how to test that you are doing the
plus-sign gesture correctly.  To return to the PlaysurfaceLauncher
desktop, just go back to the "Gestures" menu and deselect the 
"Show Gesture Panel" option.  The "Gestures" menu also has an 
option for enabling/disabling use of the plus-sign gesture to 
force quit multitiouch apps.

The red "Shutdown Icon" is for opening a dialog box with options to 
shutdown the computer, shutdown only the PlaysurfaceLauncher, or
cancel.  If this feature is not desired, use a mouse to click near
the top of the desktop and show the menubar, and then go to the 
bottom of the "Options" menu and deselect "Show Shutdown Computer
Icon".  The "Options" menu also has options to show/hide the 
Windows 7/8 taskbar that is always hidden as soon as the 
PlaysurfaceLauncher starts up.  The directory where the
PlaysurfaceLauncher.exe file is found also has an OpenTaskbar.exe
and a HideTaskbar.exe program that can be used by double-clicking
with a mouse.

The "Options" menu also has a "Touch Detector" submenu that
allows you to switch from BigBlobby, which processes infrared
camera images to generate TUIO messages, to the TouchHooks2Tuio
program, which takes Windows 8 touch events from a monitor or
other device and converts that input to TUIO messages.  The 
TouchHooks2Tuio program also blocks the emulated mouse clicks
that normally accompany Windows 8 touch events.  Blocking 
those emulated mouse clicks ensures that the TUIO-based 
multitouch apps are only touch responsive.  Although a 
mouse is needed for using the PlaysurfaceLauncher menubar 
and for making adjustments to BigBlobby or TouchHooks2Tuio,
the thinking is that afterwards the PlaysurfaceLauncher and 
the multitouch apps it launches should be a touch-only kind
of environment.  

The BigBlobby and TouchHooks2Tuio modes assume that all of 
the apps under the MultitouchApps directory are TUIO-based 
multitouch apps.  If those apps were replaced entirely with 
Windows 8 touch responsive apps, then it would make sense 
to use the third mode provided in the "Touch Detector"
submenu, "Windows 8 Touch Screen".  In this mode, the 
TouchHooks2Tuio program is still used, but it only sends 
out TUIO messages on port 3334, which is the port that 
the PlaysurfaceLauncher itself listens on.

The menubar that is opened by clicking near the very top 
of the PlaysurfaceLauncher desktop also has a Sounds menu 
for changing the short sound click that is played when an 
icon is touched.  And finally there is a Help menu with 
an "About..." box and a "Menu System Help" option that 
opens a tabbed panel with info on each of the menus in 
the menubar.

Acknowledgements:

1. To allow the PlaysurfaceLauncher to respond to TUIO touch events, the
qTUIO class (https://github.com/x29a/qTUIO) and the TUIO_CPP client reference
implementation (http://tuio.org/?software) are used to convert TUIO messages 
to QTouchEvents that Qt framework classes can listen for.  The TUIO_CPP library
was written by reacto-techno musicians at Universitat Pompeu Fabra in Barcelona,
Spain (http://reactivision.sourceforge.net and http://www.reactable.com).

2. The qextserialport library (https://github.com/qextserialport/qextserialport)
is used for turning a projector on/off (requires having a USB to serial
RS-232 adapter cable for connecting the computer to a BenQ projector).

License:

GNU General Public License, Version 3.0
http://www.gnu.org/licenses/gpl-3.0.en.html

Disclaimer:

PlaysurfaceLauncher is a work in progress, and has only been tested on a few
machines, so I cannot guarantee that it will work with your hardware setup.
In legal terms: This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

-J.R. Weber (joe.weber77@gmail.com) 
