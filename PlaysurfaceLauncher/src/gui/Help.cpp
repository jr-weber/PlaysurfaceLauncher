/*******************************************************************************
Help

PURPOSE: Responds to the options in the Help menu by opening a widget with info
         or opening PlaysurfaceLauncher help in the desktop browser.

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
#include "gui/Help.h"
#include "gui/HelpMenuWidget.h"
#include <QString>
#include <QMessagebox>
#include <QPixmap>
#include <QDesktopServices>

using gui::Help;

const QString Help::PLAYSURFACE_ICON( ":/QResources/logos/Playsurface-200x70.png" );
const QUrl Help::PLAYSURFACE_HELP_URL( "http://playsurface.org/index.php/getting-started" );
const QString Help::ABOUT_BOX_MSG(
    "Playsurface Launcher is a free, open source application that provides "
    "a touch interface for launching multitouch applications. <br /><br />"

    "Written by Joe Weber <br />"
    "joe.weber@alumni.duke.edu <br />"
    "GNU General Public License Version 3 <br /><br />"

    "<b>TUIO Framework</b> <br />"
    "The Playsurface Launcher uses BigBlobby/CCV for multitouch support "
    "based on the TUIO specification found at http://tuio.org. <br /><br />"

    "<b>Qt Framework Version 4.8</b> <br />"
    "The Playsurface Launcher GUI is written using Qt. <br />"
    "Qt Project: http://qt-project.org <br />"
    "GNU Lesser General Public License Version 2.1 <br /><br />"

    "<b>QTuio Project</b><br />"
    "Playsurface Launcher uses modified versions of the QTuio class "
    "(https://github.com/x29a/qTUIO) and the TUIO_CPP client (http://tuio.org)."
);

Help::Help() :
  helpMenuWidget_( new gui::HelpMenuWidget() )
{
}

Help::~Help()
{
    helpMenuWidget_->setVisible( false );
    delete helpMenuWidget_;
}

/***************************************************************************//**
Opens an About box with info on the PlaysurfaceLauncher.
*******************************************************************************/
void Help::openAboutBox()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle( "About PlaysurfaceLauncher" );
    messageBox.setIconPixmap( QPixmap( PLAYSURFACE_ICON ) );
    QString msg( ABOUT_BOX_MSG );
    messageBox.setTextFormat( Qt::RichText );
    messageBox.setText( msg );
    messageBox.exec();
}

/***************************************************************************//**
Opens the default web browser to a web page with help info.  If the default web
browser cannot be found, a warning message box will inform the user and give
the URL for Playsurface online help.
*******************************************************************************/
void Help::openOnlineHelp()
{
    bool success = QDesktopServices::openUrl( PLAYSURFACE_HELP_URL );

    if( !success ) {
        QMessageBox messageBox;
        messageBox.setWindowTitle( "Web Browser Not Found!" );
        messageBox.setIcon( QMessageBox::Warning );

        QString msg( "The default desktop web browser could not be found." );
        msg += "<br />For online help, point a web browser to <br /><br />";
        messageBox.setText( msg + PLAYSURFACE_HELP_URL.toString() );
        messageBox.exec();
    }
}

void Help::openMenuOptionsHelpWidget()
{
    helpMenuWidget_->showNormal();
    helpMenuWidget_->setVisible( true );
}

void Help::closeHelpMenuWidget()
{
    helpMenuWidget_->setVisible( false );
}
