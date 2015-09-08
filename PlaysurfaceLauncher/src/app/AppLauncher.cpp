/*******************************************************************************
AppLauncher

PURPOSE: Uses the QProcess class for launching other applications: BigBlobby,
         TouchHooks2Tuio, and TUIO-based multitouch apps.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
*******************************************************************************/
/*
 PlaysurfaceLauncher - Provides a game console-like environment for launching
                       TUIO-based multitouch apps.  
 
 Copyright (c) 2014, 2015 J.R.Weber <joe.weber77@gmail.com>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with 
 this program.  If not, go to http://www.gnu.org/licenses/gpl-3.0.en.html or
 write to 
 
 Free Software  Foundation, Inc.
 59 Temple Place, Suite 330
 Boston, MA  02111-1307  USA
*/
#include "app/AppLauncher.h"
#include "playsurface/Playsurface.h"
#include "io/DirectoryReader.h"
#include "gui/DialogBoxUtils.h"
#include "app/AppInfoList.h"
#include "app/AppInfo.h"
#include "utils/WindowUtility.h"
#include "utils/EdgeGesturesUtility.h"
#include "exceptions/FileNotFoundException.h"
#include "exceptions/FileNotReadableException.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTimer>
#include <iostream>

using app::AppLauncher;
using exceptions::FileNotFoundException;
using exceptions::FileNotReadableException;

const bool AppLauncher::ASSUME_FLASHPLAYER_EXE_IS_IN_PATH = false;
const int AppLauncher::FOCUS_DELAY_MILLISECONDS = 400,
          AppLauncher::MOUSE_X = 20,
          AppLauncher::MOUSE_Y = 50,
          AppLauncher::DELAY_FOR_CLOSING_TUIO_SERVER_APP = 3000;

AppLauncher::AppLauncher( playsurface::Playsurface * parent ) :
  parent_( parent ),
  directoryReader_( new io::DirectoryReader() ),
  dialogBoxUtils_( new gui::DialogBoxUtils() ),
  bigBlobbyInfo_( NULL ),
  touchHooksInfo_( NULL ),
  flashPlayerInfo_( NULL ),
  appInfoList_( new app::AppInfoList() ),
  windowUtility_( new utils::WindowUtility() ),
  edgeGesturesUtility_( new utils::EdgeGesturesUtility() ),
  bigBlobbyProcess_( NULL ),
  touchHooksProcess_( NULL ),
  multitouchProcess_( NULL ),
  tuioGenerator_( enums::TuioSourceEnum::BIG_BLOBBY ),
  hideBigBlobbyAfterStartup_( false ),
  hideTouchHooks2TuioAfterStartup_( false )
{

}

AppLauncher::~AppLauncher()
{
    killMultitouchAppProcess();
    killTouchHooks2TuioProcess();
    killBigBlobbyProcess();
    delete edgeGesturesUtility_;
    delete windowUtility_;
    delete appInfoList_;
    delete flashPlayerInfo_;
    delete touchHooksInfo_;
    delete bigBlobbyInfo_;
    delete dialogBoxUtils_;
    delete directoryReader_;
}

enums::TuioSourceEnum::Enum AppLauncher::tuioGenerator()
{
    return tuioGenerator_;
}

void AppLauncher::resetTuioGenerator( enums::TuioSourceEnum::Enum tuioSource )
{
    if( tuioGenerator_ != tuioSource ) {
        tuioGenerator_ = tuioSource;
        killBigBlobbyProcess();
        killTouchHooks2TuioProcess();

        if( tuioGenerator_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
            launchTouchHooks2Tuio();
        }
        else if( tuioGenerator_ == enums::TuioSourceEnum::WINDOWS_8_TOUCH ) {
            launchWindows8Touch();
        }
        else { // Must be BigBlobby
            launchBigBlobby();
        }
    }
}

void AppLauncher::killBigBlobbyProcess()
{
    if( bigBlobbyProcess_ != NULL ) {
        bigBlobbyProcess_->disconnect();
        bigBlobbyProcess_->close();
        delete bigBlobbyProcess_;
        bigBlobbyProcess_ = NULL;
    }
}

void AppLauncher::killTouchHooks2TuioProcess()
{
    if( touchHooksProcess_ != NULL ) {
        touchHooksProcess_->disconnect();
        touchHooksProcess_->close();
        delete touchHooksProcess_;
        touchHooksProcess_ = NULL;
    }
}

void AppLauncher::shutdownComputer()
{
    QProcess process( this );
    process.startDetached( "shutdown /s /f /t 5" );
}

void AppLauncher::readBigBlobbyInfo()
{
    try {
        bigBlobbyInfo_ = directoryReader_->readBigBlobbyInfo();
    }
    catch( FileNotFoundException e ) {
        dialogBoxUtils_->warnUserOnBigBlobby( e );
    }
    catch( FileNotReadableException e ) {
        dialogBoxUtils_->warnUserOnBigBlobby( e );
    }
}

void AppLauncher::readTouchHooks2TuioInfo()
{
    try {
        touchHooksInfo_ = directoryReader_->readTouchHooks2TuioInfo();
    }
    catch( FileNotFoundException e ) {
        dialogBoxUtils_->warnUserOnTouchHooks2Tuio( e );
    }
    catch( FileNotReadableException e ) {
        dialogBoxUtils_->warnUserOnTouchHooks2Tuio( e );
    }
}

void AppLauncher::readFlashPlayerInfo()
{
    try {
        flashPlayerInfo_ = directoryReader_->readFlashPlayerInfo();
    }
    catch( FileNotFoundException e ) {
        dialogBoxUtils_->warnUserOnFlashPlayer( e );
    }
    catch( FileNotReadableException e ) {
        dialogBoxUtils_->warnUserOnFlashPlayer( e );
    }
}

void AppLauncher::onTuioGeneratorIconTouched()
{
    if( tuioGenerator_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
        onTouchHooks2TuioIconTouched();
    }
    else if( tuioGenerator_ == enums::TuioSourceEnum::WINDOWS_8_TOUCH ) {
        // In Windows8Touch mode, TouchHooks2Tuio is used in a limited way: it
        // blocks emulated mouse clicks and allows the PlaysurfaceLauncher to
        // listen in the background to TUIO UDP messages on port 3334 while
        // a multitouch app using Windows 8 Touch is open in the foreground.
        // That allows the PlaysurfaceLauncher to detect gestures for closing
        // a multitouch app.
        onTouchHooks2TuioIconTouched();
    }
    else { // Must be BigBlobby.
        onBigBlobbyIconTouched();
    }
}

void AppLauncher::onBigBlobbyIconTouched()
{
    if( bigBlobbyProcess_ == NULL ) {
        launchBigBlobby();
    }
    else {
        toggleBigBlobby();
    }
}

void AppLauncher::onTouchHooks2TuioIconTouched()
{
    if( touchHooksProcess_ == NULL ) {
        launchTouchHooks2Tuio();
    }
    else {
        toggleTouchHooks2Tuio();
    }
}

void AppLauncher::launchTuioGenerator( enums::TuioSourceEnum::Enum tuioSource )
{
    tuioGenerator_ = tuioSource;

    if( tuioGenerator_ == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO ) {
        launchTouchHooks2Tuio();
    }
    else if( tuioGenerator_ == enums::TuioSourceEnum::WINDOWS_8_TOUCH ) {
        launchWindows8Touch();
    }
    else { // Must be BigBlobby.
        launchBigBlobby();
    }
}

/**
 * Uses BigBlobby to capture images from a PS3 Eye camera and convert them 
 * into TUIO messages sent on ports 3333 and 3334 and/or Flash XML messages
 * on port 3000.
 */
void AppLauncher::launchBigBlobby()
{
    if( bigBlobbyProcess_ == NULL && bigBlobbyInfo_ != NULL ) {
        bigBlobbyProcess_ = new QProcess( this );
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QString workingDir = bigBlobbyInfo_->directory(),
                pathToExecutable = bigBlobbyInfo_->pathToExecutable();
        
        connectBigBlobbyProcess();
        env.insert( "PATH", env.value("Path") + ";" + workingDir );
        bigBlobbyProcess_->setProcessEnvironment( env );
        bigBlobbyProcess_->setWorkingDirectory( workingDir );
        bigBlobbyProcess_->start( pathToExecutable );
    }
}

/**
 * Uses TouchHooks2Tuio to monitor Windows 8 Touch events and resend them as 
 * /tuio/2Dcur messages on ports 3333 and 3334 and as Flash XML on port 3000.
 * Emulated mouse clicks from Windows 8 Touch events are blocked.
 */
void AppLauncher::launchTouchHooks2Tuio()
{
    if( touchHooksProcess_ == NULL && touchHooksInfo_ != NULL ) {
        touchHooksProcess_ = new QProcess( this );
        QStringList args;
        args << "useGlobalHook=true"
             << "useTuioUdpChannelOne=true"
             << "useTuioUdpChannelTwo=true"
             << "useFlashXmlChannel=true";
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QString workingDir = touchHooksInfo_->directory(),
                pathToExecutable = touchHooksInfo_->pathToExecutable();
        
        connectTouchHooks2TuioProcess();
        env.insert( "PATH", env.value("Path") + ";" + workingDir );
        touchHooksProcess_->setProcessEnvironment( env );
        touchHooksProcess_->setWorkingDirectory( workingDir );
        touchHooksProcess_->start( pathToExecutable, args );
    }
}

/**
 * This function and launchToucHooks2Tuio are almost identical, except that
 * this version shuts off tuioUdpChannelOne and flashXmlChannel, as they 
 * are not needed in Windows8Touch mode.  However, tuioUdpChannelTwo is 
 * set true because that will allow the PlaysurfaceLauncher to listen in 
 * the background for gestures when a multitouch app is open in front of it.
 * TouchHooks2Tuio is also used to block emulated mouse clicks that are 
 * automatically produced by Windows 8 Touch.
 */
void AppLauncher::launchWindows8Touch()
{
    if( touchHooksProcess_ == NULL && touchHooksInfo_ != NULL ) {
        touchHooksProcess_ = new QProcess( this );
        QStringList args;
        args << "useGlobalHook=true"
             << "useTuioUdpChannelOne=false"
             << "useTuioUdpChannelTwo=true"
             << "useFlashXmlChannel=false";
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QString workingDir = touchHooksInfo_->directory(),
                pathToExecutable = touchHooksInfo_->pathToExecutable();
        
        connectTouchHooks2TuioProcess();
        env.insert( "PATH", env.value("Path") + ";" + workingDir );
        touchHooksProcess_->setProcessEnvironment( env );
        touchHooksProcess_->setWorkingDirectory( workingDir );
        touchHooksProcess_->start( pathToExecutable, args );
    }
}

void AppLauncher::connectBigBlobbyProcess()
{
    connect( bigBlobbyProcess_, SIGNAL( error( QProcess::ProcessError ) ), 
             this, SLOT( reportBigBlobbyError( QProcess::ProcessError ) ) );

    connect( bigBlobbyProcess_, SIGNAL( finished( int, QProcess::ExitStatus ) ), 
             this, SLOT( reportBigBlobbyFinished( int, QProcess::ExitStatus ) ) );

    connect( bigBlobbyProcess_, SIGNAL( started() ),
             this, SLOT( onBigBlobbyStarted() ) );
}

void AppLauncher::connectTouchHooks2TuioProcess()
{
    connect( touchHooksProcess_, SIGNAL( error( QProcess::ProcessError ) ), 
             this, SLOT( reportTouchHooks2TuioError( QProcess::ProcessError ) ) );

    connect( touchHooksProcess_, SIGNAL( finished( int, QProcess::ExitStatus ) ), 
             this, SLOT( reportTouchHooks2TuioFinished( int, QProcess::ExitStatus ) ) );

    connect( touchHooksProcess_, SIGNAL( started() ),
             this, SLOT( onTouchHooks2TuioStarted() ) );
}

void AppLauncher::onBigBlobbyStarted()
{
    if( hideBigBlobbyAfterStartup_ ) {
        QTimer::singleShot( DELAY_FOR_CLOSING_TUIO_SERVER_APP, this, SLOT( hideBigBlobby() ) );
        hideBigBlobbyAfterStartup_ = false;
    }
}

void AppLauncher::onTouchHooks2TuioStarted()
{
    if( hideTouchHooks2TuioAfterStartup_ ) {
        QTimer::singleShot( DELAY_FOR_CLOSING_TUIO_SERVER_APP, this, SLOT( hideTouchHooks2Tuio() ) );
        hideTouchHooks2TuioAfterStartup_ = false;
    }
}

void AppLauncher::toggleBigBlobby()
{
    if( bigBlobbyProcess_ != NULL && bigBlobbyProcess_->state() == QProcess::Running ) {
        windowUtility_->toggleWindowVisibility( bigBlobbyProcess_->pid() );
    }
}

void AppLauncher::toggleTouchHooks2Tuio()
{
    if( touchHooksProcess_ != NULL && touchHooksProcess_->state() == QProcess::Running ) {
        windowUtility_->toggleWindowVisibility( touchHooksProcess_->pid() );
    }
}

/*
 * This function is used only for catching a FailedToStart error for
 *  BigBlobby.exe.
 *
 * When connected to the error signal from bigBlobbyProcess_, this function
 * does not detect as many errors as when reportBigBlobbyFinished() is 
 * connected to the finished() signal.  For example, removing PS3EyeMulticam.dll
 * from the BigBlobby directory results in an error() signal, but removing 
 * either QtCore4.dll or QtGui4.dll does not result in an error() signal, even
 * though all three of these errors are picked ub by the error detection code
 * in reportBigBlobbyFinished().
 */
void AppLauncher::reportBigBlobbyError( QProcess::ProcessError processError )
{
    if( processError == QProcess::FailedToStart ) {
        killBigBlobbyProcess();
        dialogBoxUtils_->warnUserBigBlobbyFailedToStart();
    }
}

void AppLauncher::reportTouchHooks2TuioError( QProcess::ProcessError processError )
{
    if( processError == QProcess::FailedToStart ) {
        killTouchHooks2TuioProcess();
        dialogBoxUtils_->warnUserTouchHooks2TuioFailedToStart();
    }
}

void AppLauncher::reportBigBlobbyFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    killBigBlobbyProcess();

    if( exitCode != 0 ) {
        QProcess::ProcessError processError = QProcess::UnknownError;

        if( bigBlobbyProcess_ != NULL ) {
            processError = bigBlobbyProcess_->error();
        }
        dialogBoxUtils_->warnUserBigBlobbyCrashed( processError );
    }
    else {
        dialogBoxUtils_->warnUserBigBlobbyClosed();
    }
}

void AppLauncher::reportTouchHooks2TuioFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    killTouchHooks2TuioProcess();

    if( exitCode != 0 ) {
        QProcess::ProcessError processError = QProcess::UnknownError;

        if( touchHooksProcess_ != NULL ) {
            processError = touchHooksProcess_->error();
        }
        dialogBoxUtils_->warnUserTouchHooks2TuioCrashed( processError );
    }
    else {
        dialogBoxUtils_->warnUserTouchHooks2TuioClosed();
    }
}

void AppLauncher::readMultitouchAppsInfo()
{
    appInfoList_->clear();
    directoryReader_->readMultitouchAppsInfo( appInfoList_ );
}

void AppLauncher::reportAnyMultitouchAppInfoErrors()
{
    if( directoryReader_->hasAppInfoErrors() ) {
        dialogBoxUtils_->warnUser( directoryReader_->appInfoErrors() );
    }
}

std::vector<QImage> * AppLauncher::multitouchAppIcons()
{
    return appInfoList_->appIcons();
}

void AppLauncher::launchMultitouchApp( int index )
{
    if( multitouchProcess_ == NULL ) {
        app::AppInfo * appInfo = appInfoList_->appInfo( index );

        if( appInfo != NULL ) {
            placeAllOtherWindowsInTaskbar();
            moveMouseToBottomCenter();
            windowUtility_->leftClickMouse();

            if( appInfo->isFlash() ) {
                launchFlashMultitouchApp( appInfo );
            }
            else {
                launchExeMultitouchApp( appInfo );
            }
        }
    }
}

void AppLauncher::moveMouseToBottomCenter()
{
    QRect screenRect = QApplication::desktop()->screenGeometry();
    int x = screenRect.width() / 2,
        y = screenRect.height();

    windowUtility_->moveMouse( x, y );
}

void AppLauncher::placeAllOtherWindowsInTaskbar()
{
    windowUtility_->placeAllOtherWindowsInTaskbar();
}

void AppLauncher::hideBigBlobbyOnStartup()
{
    hideBigBlobbyAfterStartup_ = true;
}

void AppLauncher::hideTouchHooks2TuioOnStartup()
{
    hideTouchHooks2TuioAfterStartup_ = true;
}

void AppLauncher::hideBigBlobby()
{
    if( bigBlobbyProcess_ != NULL && bigBlobbyProcess_->state() == QProcess::Running ) {
        windowUtility_->hideWindowIfVisible( bigBlobbyProcess_->pid() );
    }
    // After hiding BigBlobby, give focus to the 
    // PlaysurfaceLauncher so keyboard shortcuts will work.
    windowUtility_->moveMouse( MOUSE_X, MOUSE_Y );
    windowUtility_->leftClickMouse();
}

void AppLauncher::hideTouchHooks2Tuio()
{
    if( touchHooksProcess_ != NULL && touchHooksProcess_->state() == QProcess::Running ) {
        windowUtility_->hideWindowIfVisible( touchHooksProcess_->pid() );
    }
    // After hiding TouchHooks2Tuio, give focus to the 
    // PlaysurfaceLauncher so keyboard shortcuts will work.
    windowUtility_->moveMouse( MOUSE_X, MOUSE_Y );
    windowUtility_->leftClickMouse();
}

void AppLauncher::launchFlashMultitouchApp( app::AppInfo * appInfo )
{
    if( flashPlayerInfo_ != NULL ) {
        multitouchProcess_ = new QProcess( this );
        connectMultitouchProcess();
        QString flashPlayerDir = flashPlayerInfo_->absoluteDirectory(),
                flashCommand = flashPlayerInfo_->filename(),
                workingDir = appInfo->absoluteDirectory(),
                pathToFlashSwfFile = appInfo->absolutePathToFile();

        // If FlashPlayer.exe is not in the Windows Path environment variable,
        // then add the full directory path to the front of the command.  In 
        // that case, the user will have to ok running the FlashPlayer the first
        // time it is started up.  Having it in the Windows Path would avoid 
        // the need for the user to have to click ok on a dialog box.
        if( !ASSUME_FLASHPLAYER_EXE_IS_IN_PATH ) {
            flashCommand = flashPlayerDir + "/" + flashCommand;
        }
        QStringList args;
        args.append( pathToFlashSwfFile );

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert( "PATH", flashPlayerDir + ";" + env.value("Path")  );
        QString envPath = env.value( "Path" );
        //std::cout << "\nflashPlayerDir = " << flashPlayerDir.toStdString() << "\n\n"
        //          << "flashCommand = " << flashCommand.toStdString() << "\n\n"
        //          << "workingDir = " << workingDir.toStdString() << "\n\n"
        //          << "pathToFlashSwfFile = " << pathToFlashSwfFile.toStdString() << "\n\n"
        //          << "Path = " << envPath.toStdString() << "\n\n";
        multitouchProcess_->setProcessEnvironment( env );
        multitouchProcess_->setWorkingDirectory( workingDir );
        multitouchProcess_->start( flashCommand, args );
    }
    // If flashPlayerInfo_ is NULL, send out a signal to inform user?
}

void AppLauncher::launchExeMultitouchApp( app::AppInfo * appInfo )
{
    multitouchProcess_ = new QProcess( this );
    connectMultitouchProcess();
    multitouchProcess_->setWorkingDirectory( appInfo->directory() );
    multitouchProcess_->start( appInfo->pathToExecutable() );
}

void AppLauncher::connectMultitouchProcess()
{
    connect( multitouchProcess_, SIGNAL( started() ), 
             this, SLOT( onMultitouchProcessStarted() ) );
    connect( multitouchProcess_, SIGNAL( error( QProcess::ProcessError ) ), 
             this, SLOT( reportMultitouchAppError( QProcess::ProcessError ) ) );
    connect( multitouchProcess_, SIGNAL( finished( int, QProcess::ExitStatus ) ), 
             this, SLOT( reportMultitouchAppFinished( int, QProcess::ExitStatus ) ) );
}

void AppLauncher::onMultitouchProcessStarted()
{
    disableEdgeGesturesForFullscreenApp();
    QTimer::singleShot( FOCUS_DELAY_MILLISECONDS, this, SLOT( focusOnMultitouchApp() ) );
}

void AppLauncher::disableEdgeGesturesForFullscreenApp()
{
    if( multitouchProcess_ != NULL ) {
        unsigned long appPid = (unsigned long)multitouchProcess_->processId();
        edgeGesturesUtility_->disableFullscreenAppEdgeGestures( appPid );
    }
}

void AppLauncher::disablePlaysurfaceLauncherEdgeGestures()
{
    edgeGesturesUtility_->disablePlaysurfaceLauncherEdgeGestures();
}


void AppLauncher::focusOnMultitouchApp()
{
    if( multitouchProcess_ != NULL && multitouchProcess_->state() == QProcess::Running ) {
        windowUtility_->focusOnWindow( multitouchProcess_->pid() );

        // Some apps move mouse cursor to center, so move it out of the way.
        moveMouseToBottomCenter();
    }
}

void AppLauncher::reportMultitouchAppError( QProcess::ProcessError processError )
{
    emit multitouchAppClosed();

    if( processError == QProcess::FailedToStart ) {
        dialogBoxUtils_->warnUserMultitouchAppFailedToStart();
        multitouchProcess_->disconnect();
        multitouchProcess_->close();
        //delete multitouchProcess_;
        multitouchProcess_ = NULL;
    }
}

void AppLauncher::killMultitouchAppProcess()
{
    emit multitouchAppClosed();

    if( multitouchProcess_ != NULL ) {
        multitouchProcess_->disconnect();
        multitouchProcess_->close();
        delete multitouchProcess_;
        multitouchProcess_ = NULL;
    }
}

void AppLauncher::reportMultitouchAppFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    if( exitCode != 0 ) {
        QProcess::ProcessError processError = QProcess::UnknownError;

        if( multitouchProcess_ != NULL ) {
            processError = multitouchProcess_->error();
        }
        dialogBoxUtils_->warnUserMultitouchAppCrashed( processError );
    }
    killMultitouchAppProcess();
}

void AppLauncher::hideMultitouchApp()
{
    if( multitouchProcess_ != NULL && multitouchProcess_->state() == QProcess::Running ) {
        windowUtility_->moveWindowToBottomZOrder( multitouchProcess_->pid() );
    }
}

bool AppLauncher::hasMultitouchAppsDirectory( char driveLetter )
{
    return directoryReader_->hasMultitouchAppsUsbDriveDir( driveLetter );
}

bool AppLauncher::isMultitouchAppsDirInUse( char driveLetter )
{
    return directoryReader_->isMultitouchAppsDirInUse( driveLetter );
}

/**
 * If BigBlobby is already open when the PlaysurfaceLauncher starts up, 
 * it should be killed because it would have a hold of the camera and that
 * would prevent the copy of BigBlobby opened by this class from getting
 * the camera.  It would also likely be a problem for getting the ports
 * needed by the TUIO servers.
 */
void AppLauncher::killBigBlobbyIfAlreadyOpen()
{
    unsigned long pid = windowUtility_->getPidForBigBlobbyIfAlreadyOpen();
    //std::cout << "BigBlobby pid = " << pid << "\n";

    if( pid > 0 ) {
        QStringList args;
        args << "/F" << "/PID" << QString::number( pid );
        bool success = QProcess::startDetached( "taskkill", args );
        
        if( !success ) {
            std::cout << "AppLauncher::killBigBlobbyIfAlreadyOpen() "
                      << "failed to kill pid " << pid << ".\n";
        }
    }
}

/**
 * If a copy of TouchHooks2Tuio is already open when the PlaysurfaceLauncher 
 * fires up, it should be killed to free up any ports its servers are using.
 * Those ports will likely be needed by the copy of BigBlobby or TouchHooks2Tuio
 * that is started up by this class.
 */
void AppLauncher::killTouchHooks2TuioIfAlreadyOpen()
{
    unsigned long pid = windowUtility_->getPidForTouchHooks2TuioIfAlreadyOpen();
    std::cout << "TouchHooks2Tuio pid = " << pid << "\n";

    if( pid > 0 ) {
        QStringList args;
        args << "/F" << "/PID" << QString::number( pid );
        bool success = QProcess::startDetached( "taskkill", args );
        
        if( !success ) {
            std::cout << "AppLauncher::killTouchHooks2TuioIfAlreadyOpen() "
                      << "failed to kill pid " << pid << ".\n";
        }
        else {
            std::cout << "AppLauncher::killTouchHooks2TuioIfAlreadyOpen() "
                      << "killed pid " << pid << ".\n";
        }
    }
}
