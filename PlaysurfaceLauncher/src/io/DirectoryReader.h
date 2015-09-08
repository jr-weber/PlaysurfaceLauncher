/*******************************************************************************
DirectoryReader

PURPOSE: Primarily written for reading info on the contents of the 
         subdirectories of the MultitouchApp directory.  In each subdirectory,
         it will look for the first executable (file ending in ".exe" or ".swf")
         and also for an associated icon image (a ".png" file) for use on the
         PlaysurfaceLauncher desktop.  Also reads info on BigBlobby, 
         TouchHooks2Tuioe, and a flashplayer.exe program.

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
#ifndef IO_DIRECTORYREADER_H
#define IO_DIRECTORYREADER_H

#include "exceptions/IOException.h"
#include <QObject>
#include <QImage>
#include <QString>
#include <QSet>
#include <vector>

namespace app { class AppInfo; }
namespace app { class AppInfoList; }
class QDir;

namespace io
{
    class DirectoryReader : public QObject
    {
        Q_OBJECT

    public:
        static const QString BIG_BLOBBY_DIR,
                             BIG_BLOBBY_EXECUTABLE,
                             BIG_BLOBBY_IMAGE_FILE,
                             TOUCH_HOOKS_DIR,
                             TOUCH_HOOKS_EXECUTABLE,
                             TOUCH_HOOKS_IMAGE_FILE,
                             FLASH_PLAYER_DIR,
                             FLASH_PLAYER_EXECUTABLE,
                             FLASH_PLAYER_IMAGE_FILE,
                             MULTI_TOUCH_DIR,
                             USB_DRIVE_MULTI_TOUCH_DIR,
                             DRIVE_SYMBOL;

        DirectoryReader();
        virtual ~DirectoryReader();

        app::AppInfo * readBigBlobbyInfo();
        app::AppInfo * readTouchHooks2TuioInfo();
        app::AppInfo * readFlashPlayerInfo();
        void readMultitouchAppsInfo( app::AppInfoList * appInfoList );
        bool hasAppInfoErrors();
        std::vector<exceptions::IOException> appInfoErrors();
        bool hasMultitouchAppsUsbDriveDir( char driveLetter );
        bool isMultitouchAppsDirInUse( char driveLetter );
        
    public slots:

    signals:

    private:
        void checkThatFileExists( const QString & filename, const QString & fullSubdirName );
        QImage loadImage( const QString & filename, const QString & fullSubdirName );
        void readMultitouchAppsInfo( const QString & multitouchDirName, app::AppInfoList * appInfoList );
        void readDir( const QString & fullSubdirName, app::AppInfoList * appInfoList );
        QString multitouchAppFilename( const QString & fullSubdirName );
        QString filename( const QString & fullSubdirName, const QStringList & nameFilters );
        QString iconImageFilename( const QString & fullSubdirName );
        void createAppInfo( const QString & appFilename, 
                            const QString & iconFilename, 
                            const QString & fullSubdirName,
                            app::AppInfoList * appInfoList );
        void addDrivenameToList( const QString & fullSubDirname );
        void clearListOfDriveNames();
        void readMultitouchAppsOnUsbDrive( app::AppInfoList * appInfoList );
        QString multitouchDirectory( const QDir & dir );

        std::vector<exceptions::IOException> errors_;
        QSet<QString> drivesInUse_;
    };
}

#endif
