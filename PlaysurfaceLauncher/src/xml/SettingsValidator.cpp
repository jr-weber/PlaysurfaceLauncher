/*******************************************************************************
SettingsValidator

PURPOSE: Performs basic data validation on program settings read from an XML 
         file. 

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
#include "xml/SettingsValidator.h"
#include "exceptions/ValidatorException.h"

using xml::SettingsValidator;
using exceptions::ValidatorException;

SettingsValidator::SettingsValidator() :
  xmlConfigFilename_(),
  usePlusGestureToQuitApps_( false ),
  showShutdownComputerIcon_( false ),
  tuioSource_( enums::TuioSourceEnum::BIG_BLOBBY )
{
}

SettingsValidator::~SettingsValidator()
{
}

void SettingsValidator::setXmlConfigFilename( const QString & filename )
{
    xmlConfigFilename_ = filename;
}

void SettingsValidator::setTuioSource( const QString & tagValue )
{
    QString source = tagValue.trimmed().toLower();

    if( source.compare( "bigblobby" ) == 0 ) {
        tuioSource_ = enums::TuioSourceEnum::BIG_BLOBBY;
    }
    else if( source.compare( "touchhooks2tuio" ) == 0 ) {
        tuioSource_ = enums::TuioSourceEnum::TOUCH_HOOKS_2_TUIO;
    }
    else if( source.compare( "windows8touch" ) == 0 ) {
        tuioSource_ = enums::TuioSourceEnum::WINDOWS_8_TOUCH;
    }
    else {
        throw ValidatorException( "Invalid startup setting detected.",
                                  "SettingsValidator::setTuioSource()",
                                  "tuioSource",
                                  tagValue,
                                  "BigBlobby, TouchHooks2Tuio, or Windows8Touch",
                                  xmlConfigFilename_ );
    }
}

enums::TuioSourceEnum::Enum SettingsValidator::tuioSource()
{
    return tuioSource_;
}

void SettingsValidator::setTuioSource( enums::TuioSourceEnum::Enum source )
{
    tuioSource_ = source;
}

void SettingsValidator::usePlusGestureToQuitApps( const QString & tagValue )
{
    QString b = tagValue.trimmed().toLower();

    if( b == "true" ) {
        usePlusGestureToQuitApps_ = true;
    }
    else if( b == "false" ) {
        usePlusGestureToQuitApps_ = false;
    }
    else {
        throw ValidatorException( "Invalid startup setting detected.",
                                  "SettingsValidator::usePlusGestureToQuitApps()",
                                  "usePlusGestureToQuitApps",
                                  tagValue,
                                  "true or false",
                                  xmlConfigFilename_ );
    }
}

bool SettingsValidator::usePlusGestureToQuitApps()
{
    return usePlusGestureToQuitApps_;
}

void SettingsValidator::usePlusGestureToQuitApps( bool b )
{
    usePlusGestureToQuitApps_ = b;
}

void SettingsValidator::showShutdownComputerIcon( const QString & tagValue )
{
    QString b = tagValue.trimmed().toLower();

    if( b == "true" ) {
        showShutdownComputerIcon_ = true;
    }
    else if( b == "false" ) {
        showShutdownComputerIcon_ = false;
    }
    else {
        throw ValidatorException( "Invalid startup setting detected.",
                                  "SettingsValidator::showShutdownComputerIcon()",
                                  "showShutdownComputerIcon",
                                  tagValue,
                                  "true or false",
                                  xmlConfigFilename_ );
    }
}

bool SettingsValidator::showShutdownComputerIcon()
{
    return showShutdownComputerIcon_;
}

void SettingsValidator::showShutdownComputerIcon( bool b )
{
    showShutdownComputerIcon_ = b;
}
