/*******************************************************************************
XmlSettings

PURPOSE: Reads/writes XML files for loading/storing the program settings.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
CREATED: 3/14/2015
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
#include "gui/XmlSettings.h"
#include "gui/FullScreenWindow.h"
#include "gui/XmlDialogBoxUtils.h"
#include "xml/SettingsXmlReader.h"
#include "xml/SettingsXmlWriter.h"
#include "xml/SettingsValidator.h"
#include "exceptions/FileNotFoundException.h"
#include "exceptions/SettingsXmlReaderException.h"
#include "exceptions/SettingsXmlWriterException.h"
#include "exceptions/UnknownXmlTagException.h"
#include "exceptions/ValidatorException.h"

using gui::XmlSettings;
using exceptions::FileNotFoundException;
using exceptions::SettingsXmlReaderException;
using exceptions::SettingsXmlWriterException;
using exceptions::UnknownXmlTagException;
using exceptions::ValidatorException;

const QString XmlSettings::DEFAULT_CONFIG_FILE = "Data/Settings/PlaysurfaceLauncherSettings.xml";

XmlSettings::XmlSettings() :
  xmlReader_( new xml::SettingsXmlReader() ),
  xmlWriter_( new xml::SettingsXmlWriter() ),
  validator_( new xml::SettingsValidator() ),
  dialogBox_( new gui::XmlDialogBoxUtils() )
{
}

XmlSettings::~XmlSettings()
{
    delete xmlReader_;
    delete xmlWriter_;
    delete validator_;
    delete dialogBox_;
}

enums::TuioSourceEnum::Enum XmlSettings::tuioSource()
{
    return validator_->tuioSource();
}

bool XmlSettings::isBigBlobbySelected()
{
    return (validator_->tuioSource() == enums::TuioSourceEnum::BIG_BLOBBY);
}

bool XmlSettings::isTouchHooks2TuioSelected()
{
    return (validator_->tuioSource() == enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO);
}

bool XmlSettings::isWindows8TouchSelected()
{
    return (validator_->tuioSource() == enums::TuioSourceEnum::WINDOWS_8_TOUCH);
}

void XmlSettings::readXmlConfigFile()
{
    useXmlFileToUpdateValidator();
}

void XmlSettings::updateMainWindowSettings( gui::FullScreenWindow * mainWindow )
{
    // The tuio source (BigBlobby, TouchHooks2Tuio, or Windows8Touch) is set  
    // by the FullScreenWindow::createOptionsMenu() function.

    bool showShutdownIcon = validator_->showShutdownComputerIcon();
    mainWindow->setOptionsMenuShowShutdownComputerIcon( showShutdownIcon );

    bool usePlusSign = validator_->usePlusGestureToQuitApps();
    mainWindow->setGesturesMenuPlusSignOption( usePlusSign );
}

void XmlSettings::saveSettingsToXmlFile( gui::FullScreenWindow * mainWindow )
{
    enums::TuioSourceEnum::Enum tuioSrc = mainWindow->tuioGenerator();
    validator_->setTuioSource( tuioSrc );

    bool showShutdownIcon = mainWindow->isShutdownComputerIconShowing();
    validator_->showShutdownComputerIcon( showShutdownIcon );

    bool usePlusSign = mainWindow->isPlusSignGestureInUse();
    validator_->usePlusGestureToQuitApps( usePlusSign );

    useValidatorToUpdateXmlFile();
}

void XmlSettings::useXmlFileToUpdateValidator()
{
    try {
        xmlReader_->read( DEFAULT_CONFIG_FILE, validator_ );
    }
    catch( FileNotFoundException e ) {
        dialogBox_->warnUser( e );
    }
    catch( SettingsXmlReaderException e ) {
        dialogBox_->warnUser( e );
    }
    catch( UnknownXmlTagException e ) {
        dialogBox_->warnUser( e );
    }
    catch( ValidatorException e ) {
        dialogBox_->warnUser( e );
    }
    catch( ... ) {
        QString msg = "An unexpected error occurred while trying to read \n'";
        dialogBox_->warnUser( msg + DEFAULT_CONFIG_FILE + "'." );
    }
    if( xmlReader_->hasUnknownXmlTagExceptions() ) {
        dialogBox_->warnUser( xmlReader_->unknownXmlTagExceptions() );
    }
    if( xmlReader_->hasValidatorExceptions() ) {
        dialogBox_->warnUser( xmlReader_->validatorExceptions() );
    }
}

void XmlSettings::useValidatorToUpdateXmlFile()
{
    try {
        xmlWriter_->write( validator_, DEFAULT_CONFIG_FILE );
    }
    catch( SettingsXmlWriterException e ) {
        dialogBox_->warnUser( e );
    }
    catch( ... ) {
        QString msg( "An unexpected error occurred while trying to write \n" );
        dialogBox_->warnUser( msg + DEFAULT_CONFIG_FILE );
    }
}
