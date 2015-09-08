/*******************************************************************************
IconSound

PURPOSE: For playing short sound effects from ".wav" files.  Intended to be used
         when a PlaysurfaceLauncher deskto icon is touched.

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
#include "sound/IconSound.h"
#include <QSound>
#include <QDir>
#include <QFileInfo>
#include <iostream>

using sound::IconSound;

const QString IconSound::SOUND_DIR = "./Sounds",
              IconSound::DEFAULT_FILE = "Towhee Click.wav";

IconSound::IconSound() :
  sounds_(),
  menuNames_(),
  currentIndex_( -1 )
{
    readSoundFiles();
    setSound( DEFAULT_FILE );
}

IconSound::~IconSound()
{
    int size = sounds_.size();

    for( int i = 0; i < size; ++i ) {
        delete sounds_.at( i );
    }
}

QStringList IconSound::menuNames()
{
    return menuNames_;
}

QString IconSound::currentSound()
{
    QString currentSound = "None";

    if( currentIndex_ >= 0 && currentIndex_ < menuNames_.size() ) {
       currentSound = menuNames_.at( currentIndex_ );
    }
    return currentSound;
}

void IconSound::setSound( const QString & menuName )
{
    QString filename = menuName;
    int size = sounds_.size();
    
    if( !menuName.endsWith( ".wav", Qt::CaseInsensitive ) ) {
        filename += ".wav";
    }

    for( int i = 0; i < size; ++i ) {
        QSound * iconSound = sounds_.at( i );
        QString fullFileName = iconSound->fileName();

        if( fullFileName.endsWith( filename, Qt::CaseInsensitive ) ) {
            currentIndex_ = i;
            break;
        }
    }
}

void IconSound::play()
{
    if( currentIndex_ >= 0 && currentIndex_ < sounds_.size() ) {
        QSound * iconSound = sounds_.at( currentIndex_ );
        iconSound->play();
    }
}

void IconSound::readSoundFiles()
{
    QDir dir( SOUND_DIR );
    QDir::Filters filters = QDir::Files | QDir::NoSymLinks;
    QStringList nameFilters( "*.wav" );
    QFileInfoList fileInfoList = dir.entryInfoList( nameFilters, filters );
    int size = fileInfoList.size();

    for( int i = 0; i < size; ++i ) {
        QFileInfo fileInfo = fileInfoList.at( i );
        QString filePath = fileInfo.canonicalFilePath(),
                filename = fileInfo.fileName();

        QSound * iconSound = new QSound( filePath );
        sounds_.append( iconSound );
        menuNames_.append( filename.remove( ".wav" ) );
    }

    if( sounds_.size() > 0 ) {
        currentIndex_ = 0;
    }
}
