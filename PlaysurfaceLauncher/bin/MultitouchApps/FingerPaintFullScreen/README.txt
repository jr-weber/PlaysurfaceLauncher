Modified the FingerPaint demo on 9/18/2014 so that if /tuio/2Dblb messages are 
sent by BigBlobby/CCV, those messages will be used in preference over 
/tuio/2Dcur messages.  The /tuio/2Dblb specification includes pressure info
(based on blob area).  Relative pressure is now used for determining the 
paintbrush diameter.

Modified the FingerPaint demo on 3/24/2014 so that it will take a TUIO port 
number as a command line argument.  The default port for TUIO UDP is 3333, 
but there is now a version of BigBlobby/CCV that can send TUIO messages on 
a second port that is specified in the DefaultBlobParams.xml file.  For testing 
purposes, it will be very useful to have this version of FingerPaint, which 
can be set to listen on other port numbers.

-Joe Weber <joe.weber@alumni.duke.edu>

--------------------------------------------------------------------------------
Purpose of the FingerPaint Demo                                      1/29/2014
--------------------------------------------------------------------------------
The FingerPaint program is a proof-of-concept demo intended to show that a
Qt/C++ program can be used with multitouch events detected by the BigBlobby/CCV 
program used with the Playsurface touch table (http://playsurface.org/). 

For an overview of BigBlobby/CCV go to 

    https://sites.google.com/site/bigblobbygui4ccv/home 

For Directions on downloading and installing BigBlobby/CCV go to 

    https://sites.google.com/site/bigblobbygui4ccv/getting-started 

Once BigBlobby/CCV is up and running, go to the Network menu at the top of the 
GUI and select "Use TUIO UDP". The TUIO messages will be sent on port 3333, 
which is the port that the FingerPaint program will be listening to. The QTuio 
class in the FingerPaint demo will convert the TUIO messages to QTouchEvent 
objects, and then send each QTouchEvent object to the event() function of the 
central widget of the main window. The central widget is a class called 
ScribbleArea, and its event() function knows how to extract info from each 
QTouchEvent object and use that info for painting. 

--------------------------------------------------------------------------------
Composition of the FingerPaint Visual Studio C++ Project
--------------------------------------------------------------------------------
Building the FingerPaint demo requires Visual Studio 2008 (or later) for 
C++, the Qt 4.8 (or later) framework with the Qt Visual Studio plugin 
(qt-vs-addin-1.1.11 or later). If you look in the project's src directory, 
where main.cpp is located, you should see three subdirectories: 

    fingerPaint 
    qtuio 
    TUIO_CPP 

--------------------------------------------------------------------------------
fingerPaint subdirectory
--------------------------------------------------------------------------------
The Qt 4.8 framework comes with a small set of multitouch demos, including 
the FingerPaint program. If you installed Qt 4.8 in "C:\Qt\Qt-4.8.5" as I did, 
the multitouch demos can be found in the "C:\Qt\Qt-4.8.5\examples\touch" 
directory. The MainWindow and ScribbleArea classes in the fingerPaint 
subdirectory of the project were copied directly from the 
"C:\Qt\Qt-4.8.5\examples\touch\fingerpaint".

-------------------------------------------------------------------------------- 
TUIO_CPP subdirectory
--------------------------------------------------------------------------------
The classes in the TUIO_CPP subdirectory were downloaded from the tuio.org 
software page at http://tuio.org/?software as the TUIO_CPP-1.4.zip file found 
in the TUIO Server Reference Implementation section. The TUIO_CPP library was
written by reacto-techno musicians at Universitat Pompeu Fabra in Barcelona,
Spain (http://reactivision.sourceforge.net and http://www.reactable.com), and 
it is the same library that is used by Community Core Vision for sending out 
the TUIO messages as UDP packets. The TUIO_CPP library is free software 
licensed under the GNU General Public License. A copy of the GNU General 
Public License and a readme.html file can be found in the TUIO_CPP subdirectory. 

-------------------------------------------------------------------------------- 
qtuio subdirectory 
--------------------------------------------------------------------------------
The QTuio class in the qtuio subdirectory was obtained from the qTUIO project 
on GitHub (https://github.com/x29a/qTUIO). The QTuio class is built on top of 
the TUIO_CPP library, and is also distributed under the GNU General Public 
License. The QTuio class was used by adding the following two lines of code 
to the main() function in main.cpp: 

    qtuio::QTuio qTuio( &window ); 
    qTuio.run(); 

QTuio is a subclass of TuioListener and QThread, and the window object given 
to its constructor is expected to be a QMainWindow object. When run() is called,
the QTuio object will create a TuioClient that listens for TUIO messages as UDP 
packets on port 3333. When a TUIO message is received, the QTuio::tuioToQt() 
function will extract info from a TuioCursor object and repackage that info 
into a QTouchEvent object. The QTouchEvent object will then be sent to the 
event(() function of the central widget of the QMainWindow. In the case of 
the FingerPaint program, the central widget of the main window is a class 
called ScribbleArea, and its event() function knows how to paint with info 
it extracts from the QTouchEvent.

A touch accuracy issue that may come up when using FingerPaint is that the 
QTuio class has hard-coded x-offset and y-offset values that are used to 
adjust the (x, y) position of each touch event. The current values were set 
to work ok with a monitor/projector resolution of 1280 x 800 pixels. If used 
with a different monitor/projector resolution, the offset would probably not 
be quite right, so there might be an obvious difference between where your 
finger tips touch the surface and where the paint lines are drawn. The 
hard-coded offsets should eventually be replaced by calculations based 
on the actual top left position of the central widget, or perhaps even 
some kind of a menu option should allow the user to adjust the x or y 
offset at runtime. 

--------------------------------------------------------------------------------
Creating New Qt/C++ Multitouch Projects 
--------------------------------------------------------------------------------
Getting all of the pieces of the FingerPaint demo to compile on Windows 
required a fair amount of time experimenting with settings in the Visual Studio 
project Properties box. To create another Qt/C++ multitouch project, the 
simplest approach would be to just copy the entire existing FingerPaint 
solution, rename it, and then replace the fingerPaint namespace/subdirectory 
with something else and modify main.cpp accordingly. Additional Qt multitouch 
demos can be found in the "C:\Qt\Qt-4.8.5\examples\touch" (depending on where 
you installed Qt), or at the following Qt Project web site page: 

    http://qt-project.org/doc/qt-4.8/examples-touch.html 

I have not tested any of the other Qt multitouch demos at this time, so don't 
be surprised if there are things that need to be fixed up. 

-Joe Weber (joe.weber@alumni.duke.edu) 
