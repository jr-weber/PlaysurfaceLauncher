/*******************************************************************************
FullScreenWindow

PURPOSE: Provides the full screen window for the PlaysurfaceLauncher, which 
         presents a menu bar (when a user mouse clicks near the top of the 
         screen) and holds the CentralWidget that controls rendering of 
         multitouch app icons and listens for touch events.

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
#include "gui/FullScreenWindow.h"
#include "app/AppLauncher.h"
#include "utils/TaskbarUtility.h"
#include "gui/CentralWidget.h"
#include "gui/Help.h"
#include "gui/XmlSettings.h"
#include "sound/IconSound.h"
#include "localclient/BigBlobbyClient.h"
#include "localclient/TouchHooksClient.h"
#include "playsurface/Playsurface.h"
#include <QtGui>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QTimer>
#include <iostream> 
#include <Windows.h>
#include <Dbt.h>
#include <Setupapi.h>

using gui::FullScreenWindow;

FullScreenWindow::FullScreenWindow( playsurface::Playsurface * parent,
                                    app::AppLauncher * appLauncher,
                                    utils::TaskbarUtility * taskbarUtility,
                                    gui::XmlSettings * xmlSettings,
                                    bool showGesturesMenu ) :
  playsurface_( parent ),
  appLauncher_( appLauncher ),
  taskbarUtility_( taskbarUtility ),
  centralWidget_( new gui::CentralWidget( appLauncher, this ) ),
  help_( new gui::Help() ),
  fileMenu_( NULL ),
  exitAct_( NULL ),
  shutdownAct_( NULL ),
  optionsMenu_( NULL ),
  showTaskbarAct_( NULL ),
  hideTaskbarAct_( NULL ),
  touchDetectorMenu_( NULL ),
  bigBlobbyAct_( NULL ),
  touchHooksAct_( NULL ),
  windows8TouchAct_( NULL ),
  touchDetectorGroup_( NULL ),
  showShutdownComputerAct_( NULL ),
  gesturesMenu_( NULL ),
  usePlusGestureToQuitApps_( NULL ),
  gesturesModeAct_( NULL ),
  soundMenu_( NULL ),
  soundActionGroup_( NULL ),
  iconSound_( centralWidget_->iconSound() ),
  helpMenu_( NULL ),
  aboutBoxAct_( NULL ),
  onlineHelpAct_( NULL ),
  menuSystemHelpAct_( NULL ),
  isMenuBarVisible_( false ),
  bigBlobbyClient_( new localclient::BigBlobbyClient() ),
  touchHooksClient_( new localclient::TouchHooksClient() ),
  xmlSettings_( xmlSettings )
{
    connect( bigBlobbyClient_, 
             SIGNAL( bigBlobbySavedSettingsToXmlFiles() ),
             this, 
             SLOT( close() ) );
    connect( touchHooksClient_, 
             SIGNAL( touchHooks2TuioReleasedGlobalHook() ),
             this, 
             SLOT( close() ) );

    setCentralWidget( centralWidget_ );
    setWindowFlags( windowFlags() | Qt::WindowStaysOnBottomHint );
    createFileMenu();
    createOptionsMenu();
    setWindowTitle( "Playsurface Launcher" );

    if( showGesturesMenu ) {
        createGesturesMenu();
    }
    createSoundMenu();
    createHelpMenu();
    hideMenuBar();
}

FullScreenWindow::~FullScreenWindow()
{
    delete bigBlobbyClient_;
    delete help_;
    delete centralWidget_;
}

void FullScreenWindow::createFileMenu()
{
    fileMenu_ = new QMenu( tr( "&File" ), this );
    exitAct_ = new QAction( tr( "&Exit" ), this );
    exitAct_->setShortcut( tr( "Ctrl+Q" ) );
    shutdownAct_ = new QAction( tr( "&Shutdown Computer" ), this );
    shutdownAct_->setShortcut( tr( "Ctrl+K" ) );

    connect( exitAct_, SIGNAL( triggered() ),
             this, SLOT( closeProgramAfterWarningTuioGenerator() ) );
    connect( shutdownAct_, SIGNAL( triggered() ), 
             this, SLOT( shutdownComputer() ) );

    fileMenu_->addAction( exitAct_ );
    fileMenu_->addSeparator();
    fileMenu_->addAction( shutdownAct_ );
    menuBar()->addMenu( fileMenu_ );
}

void FullScreenWindow::createOptionsMenu()
{
    optionsMenu_ = new QMenu( tr( "&Options" ), this );
    showTaskbarAct_ = new QAction( tr( "&Show Taskbar" ), this );
    hideTaskbarAct_ = new QAction( tr( "&Hide Taskbar" ), this );
    showShutdownComputerAct_ = new QAction( tr( "&Show Shutdown Computer Icon" ), this );
    showShutdownComputerAct_->setCheckable( true );
    showShutdownComputerAct_->setChecked( false );

    connect( showTaskbarAct_, SIGNAL( triggered( )), this, SLOT( showTaskbar() ) );
    connect( hideTaskbarAct_, SIGNAL( triggered() ), this, SLOT( hideTaskbar() ) );
    connect( showShutdownComputerAct_, SIGNAL( toggled( bool ) ), 
             centralWidget_, SLOT( showShutdownComputerIcon( bool ) ) );

    touchDetectorMenu_ = new QMenu( tr( "&Touch Detector" ), this );
    touchDetectorGroup_ = new QActionGroup( this );
    bigBlobbyAct_ = new QAction( tr( "&BigBlobby (camera)" ), this );
    touchHooksAct_ = new QAction( tr( "&TouchHooks2Tuio (bezel)" ), this );
    windows8TouchAct_ = new QAction( tr( "&Windows 8 Touch Screen" ), this );

    bigBlobbyAct_->setCheckable( true );
    bigBlobbyAct_->setChecked( xmlSettings_->isBigBlobbySelected() );
    touchHooksAct_->setCheckable( true );
    touchHooksAct_->setChecked( xmlSettings_->isTouchHooks2TuioSelected() );
    windows8TouchAct_->setCheckable( true );
    windows8TouchAct_->setChecked( xmlSettings_->isWindows8TouchSelected() );

    touchDetectorGroup_->addAction( bigBlobbyAct_ );
    touchDetectorGroup_->addAction( touchHooksAct_ );
    touchDetectorGroup_->addAction( windows8TouchAct_ );
    touchDetectorMenu_->addAction( bigBlobbyAct_ );
    touchDetectorMenu_->addAction( touchHooksAct_ );
    touchDetectorMenu_->addAction( windows8TouchAct_ );

    connect( bigBlobbyAct_, SIGNAL( toggled( bool ) ),
             this, SLOT( onBigBlobbyGeneratorSelected( bool ) ) );
    connect( touchHooksAct_, SIGNAL( toggled( bool ) ),
             this, SLOT( onTouchHooks2TuioGeneratorSelected( bool ) ) );
    connect( windows8TouchAct_, SIGNAL( toggled( bool ) ),
             this, SLOT( onWindows8TouchSelected( bool ) ) );

    optionsMenu_->addAction( showTaskbarAct_ );
    optionsMenu_->addAction( hideTaskbarAct_ );
    optionsMenu_->addSeparator();
    optionsMenu_->addMenu( touchDetectorMenu_ );
    optionsMenu_->addSeparator();
    optionsMenu_->addAction( showShutdownComputerAct_ );
    menuBar()->addMenu( optionsMenu_ );
}

void FullScreenWindow::onBigBlobbyGeneratorSelected( bool b )
{
    //std::cout << "FullScreenWindow::onBigBlobbyGeneratorSelected(" << b << ") called...\n";

    if( b ) {
        appLauncher_->resetTuioGenerator( enums::TuioSourceEnum::BIG_BLOBBY );
        centralWidget_->setTuioGeneratorIcon( enums::TuioSourceEnum::BIG_BLOBBY );
        playsurface_->allowTuio2DcurFiltering( true );
    }
}

void FullScreenWindow::onTouchHooks2TuioGeneratorSelected( bool b )
{
    //std::cout << "FullScreenWindow::onTouchHooks2TuioGeneratorSelected(" << b << ") called...\n";

    if( b ) {
        appLauncher_->resetTuioGenerator( enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO );
        centralWidget_->setTuioGeneratorIcon( enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO );
        playsurface_->allowTuio2DcurFiltering( false );
    }
}

void FullScreenWindow::onWindows8TouchSelected( bool b )
{
    //std::cout << "FullScreenWindow::onWindows8TouchSelected(" << b << ") called...\n";

    if( b ) {
        appLauncher_->resetTuioGenerator( enums::TuioSourceEnum::WINDOWS_8_TOUCH );
        centralWidget_->setTuioGeneratorIcon( enums::TuioSourceEnum::WINDOWS_8_TOUCH );
        playsurface_->allowTuio2DcurFiltering( false );
    }
}

enums::TuioSourceEnum::Enum FullScreenWindow::tuioGenerator()
{
    return appLauncher_->tuioGenerator();
}

void FullScreenWindow::createGesturesMenu()
{
    gesturesMenu_ = new QMenu( tr( "&Gestures" ), this );
    usePlusGestureToQuitApps_ = new QAction( tr( "&Use Plus-Gesture to Quit Apps" ), this );
    gesturesModeAct_ = new QAction( tr( "&Show Gesture Panel" ), this );
    clearGesturesPanelAct_ = new QAction( tr( "&Clear Gesture Panel" ), this );
    usePlusGestureToQuitApps_->setCheckable( true );
    usePlusGestureToQuitApps_->setChecked( false );
    gesturesModeAct_->setCheckable( true );
    gesturesModeAct_->setChecked( false );

    connect( usePlusGestureToQuitApps_, SIGNAL( toggled( bool ) ),
             centralWidget_, SLOT( usePlusGestureToQuitApps( bool ) ) );
    connect( gesturesModeAct_, SIGNAL( toggled( bool ) ), 
             centralWidget_, SLOT( showGesturePanel( bool ) ) );
    connect( clearGesturesPanelAct_, SIGNAL( triggered() ), 
             centralWidget_, SLOT( clearGesturePanelImage() ) );

    gesturesMenu_->addAction( usePlusGestureToQuitApps_ );
    gesturesMenu_->addAction( gesturesModeAct_ );
    gesturesMenu_->addAction( clearGesturesPanelAct_ );
    menuBar()->addMenu( gesturesMenu_ );
}

void FullScreenWindow::createSoundMenu()
{
    soundMenu_ = new QMenu( tr( "&Sounds" ), this );
    soundActionGroup_ = new QActionGroup( this );

    QStringList menuNames = iconSound_->menuNames();
    QString currentSound = iconSound_->currentSound();
    int size = menuNames.size();

    for( int i = 0; i < size; ++i ) {
        QString menuName = menuNames.at( i );
        QAction * action = new QAction( menuName, this );
        action->setCheckable( true );

        if( menuName == currentSound ) {
            action->setChecked( true );
        }
        connect( action, SIGNAL( toggled( bool ) ),
                 this, SLOT( onSoundSelected( bool ) ) );
        soundActionGroup_->addAction( action );
        soundMenu_->addAction( action );
    }
    menuBar()->addMenu( soundMenu_ );
}

void FullScreenWindow::onSoundSelected( bool b )
{
    if( b ) {
        QAction * action = soundActionGroup_->checkedAction();

        if( action != NULL ) {
            QString menuOption = action->text();
            iconSound_->setSound( menuOption );
            iconSound_->play();
        }
    }
}

void FullScreenWindow::createHelpMenu()
{
    helpMenu_ = new QMenu( tr( "&Help" ), this );
    aboutBoxAct_ = new QAction( tr( "&About..." ), this );
    onlineHelpAct_ = new QAction( tr( "&Online Help..." ), this );
    menuSystemHelpAct_ = new QAction( tr( "&Menu System Help" ), this );

    connect( aboutBoxAct_, SIGNAL( triggered( )), help_, SLOT( openAboutBox() ) );
    connect( onlineHelpAct_, SIGNAL( triggered() ), help_, SLOT( openOnlineHelp() ) );
    connect( menuSystemHelpAct_, SIGNAL( triggered() ), help_, SLOT( openMenuOptionsHelpWidget() ) );

    helpMenu_->addAction( aboutBoxAct_ );
    helpMenu_->addAction( onlineHelpAct_ );
    helpMenu_->addAction( menuSystemHelpAct_ );
    menuBar()->addMenu( helpMenu_ );
}

void FullScreenWindow::readMultitouchAppsInfo()
{
    centralWidget_->readMultitouchAppsInfo();
}

void FullScreenWindow::showFullScreenWindow()
{
    showFullScreen();
    activateWindow();
}

void FullScreenWindow::showTaskbar()
{
    taskbarUtility_->hideTaskbar();
    taskbarUtility_->showTaskbar();
}

void FullScreenWindow::hideTaskbar()
{
    taskbarUtility_->hideTaskbar();
}

void FullScreenWindow::closeEvent( QCloseEvent * event )
{
    help_->closeHelpMenuWidget();
    centralWidget_->hideConfirmExitBox();
    centralWidget_->hideConfirmShutdownBox();
    setVisible( false );
    xmlSettings_->saveSettingsToXmlFile( this );
    QMainWindow::closeEvent( event );
}

void FullScreenWindow::exitWithoutShuttingDownProjector()
{
    playsurface_->shutDownProjectorIfOn( false );
    closeProgramAfterWarningTuioGenerator();
}

void FullScreenWindow::closeProgramAfterWarningTuioGenerator()
{
    if( appLauncher_->tuioGenerator() == enums::TuioSourceEnum::BIG_BLOBBY ) {
        //std::cout << "About to call tellBigBlobbyToSaveSettingsToXmlFiles()...\n";
        bigBlobbyClient_->tellBigBlobbyToSaveSettingsToXmlFiles();
    }
    else { // Must be using TouchHooks2Tuio.
        //std::cout << "About to call tellTouchHooks2TuioToReleaseGlobalHook()...\n";
        touchHooksClient_->tellTouchHooks2TuioToReleaseGlobalHook();
    }
    // A response from either BigBlobby or TouchHooks2Tuio will call on close.
    // However, as a backup plan, a fairly short timeout should always be set.
    QTimer::singleShot( 400, this, SLOT( close() ) );
}

void FullScreenWindow::shutdownComputer()
{
    appLauncher_->shutdownComputer();
    closeProgramAfterWarningTuioGenerator();
}

void FullScreenWindow::keyPressEvent( QKeyEvent * e )
{
    if( e->modifiers() & Qt::ControlModifier ) {
        switch( e->key() ) {
            case Qt::Key_K:  shutdownComputer();                   break;
            case Qt::Key_Q:  closeProgramAfterWarningTuioGenerator();  break;
        } 
    }
}

bool FullScreenWindow::winEvent( MSG * msg, long * result )
{
    char driveLetter = 'z';
    int msgType = msg->message;

    if( msgType == WM_DEVICECHANGE ) {
        switch( msg->wParam ) {
            case DBT_DEVICEARRIVAL:
                driveLetter = getDriveLetterFromMsg( msg );
                centralWidget_->onDeviceArrival( driveLetter );
                //std::cout << "Device arrival detected: drive = " << driveLetter << "\n";
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                driveLetter = getDriveLetterFromMsg( msg );
                centralWidget_->onDeviceRemoval( driveLetter );
                //std::cout << "Device removal detected: drive = " << driveLetter << "\n";
                break;
        }
    }
    return false;
}

char FullScreenWindow::getDriveLetterFromMsg( MSG * msg )
{
    char DriveLetter = 0;
    PDEV_BROADCAST_HDR drive_hdr = (PDEV_BROADCAST_HDR)msg->lParam;

    if( drive_hdr -> dbch_devicetype == DBT_DEVTYP_VOLUME ) {
        PDEV_BROADCAST_VOLUME volume_hdr = (PDEV_BROADCAST_VOLUME)drive_hdr;
        PDEV_BROADCAST_DEVICEINTERFACE device_info = (PDEV_BROADCAST_DEVICEINTERFACE)drive_hdr;
        char name = (char)device_info->dbcc_name;
        DriveLetter = firstDriveFromMask( volume_hdr->dbcv_unitmask );
    }
    return DriveLetter;
}

char FullScreenWindow::firstDriveFromMask( ULONG unitmask )
{
    char i;

    for( i = 0; i < 26; ++i ){
        if( unitmask & 0x1 )
            break;
        unitmask = unitmask >> 1;
    }
    return( i + 'A' );
}

void FullScreenWindow::hideMenuBar()
{
    isMenuBarVisible_ = false;
    menuBar()->setVisible( false );
}

void FullScreenWindow::showMenuBar()
{
    isMenuBarVisible_ = true;
    menuBar()->setVisible( true );
}

bool FullScreenWindow::isMenuBarOpen()
{
    return isMenuBarVisible_;
}

void FullScreenWindow::updateMainWindowSettings()
{
    xmlSettings_->updateMainWindowSettings( this );
}

void FullScreenWindow::setGesturesMenuPlusSignOption( bool usePlusSign )
{
    usePlusGestureToQuitApps_->setChecked( usePlusSign );
}

bool FullScreenWindow::isPlusSignGestureInUse()
{
    return usePlusGestureToQuitApps_->isChecked();
}

void FullScreenWindow::setOptionsMenuShowShutdownComputerIcon( bool b )
{
    showShutdownComputerAct_->setChecked( b );
}

bool FullScreenWindow::isShutdownComputerIconShowing()
{
    return showShutdownComputerAct_->isChecked();
}
