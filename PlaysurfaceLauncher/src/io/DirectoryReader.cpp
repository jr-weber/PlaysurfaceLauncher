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
#include "io/DirectoryReader.h"
#include "app/AppInfo.h"
#include "app/AppInfoList.h"
#include "exceptions/FileNotFoundException.h"
#include "exceptions/FileNotReadableException.h"
#include <QFileInfo>
#include <QDir>
#include <iostream>

using io::DirectoryReader;
using exceptions::IOException;
using exceptions::FileNotFoundException;
using exceptions::FileNotReadableException;

const QString DirectoryReader::BIG_BLOBBY_DIR = "./BigBlobby",
              DirectoryReader::BIG_BLOBBY_EXECUTABLE = "BigBlobby.exe",
              DirectoryReader::BIG_BLOBBY_IMAGE_FILE = "BigBlobby-playsurface-icon.png",
              DirectoryReader::TOUCH_HOOKS_DIR = "./TouchHooks2Tuio",
              DirectoryReader::TOUCH_HOOKS_EXECUTABLE = "TouchHooks2Tuio.exe",
              DirectoryReader::TOUCH_HOOKS_IMAGE_FILE = "TouchHooks2Tuio-playsurface-icon.png",
              DirectoryReader::FLASH_PLAYER_DIR = "./FlashPlayer",
              DirectoryReader::FLASH_PLAYER_EXECUTABLE = "flashplayer.exe",
              DirectoryReader::FLASH_PLAYER_IMAGE_FILE = "Flash-playsurface-icon.png",
              DirectoryReader::MULTI_TOUCH_DIR = "./MultitouchApps",
              DirectoryReader::USB_DRIVE_MULTI_TOUCH_DIR = "MultitouchAppsUsbDrive",
              DirectoryReader::DRIVE_SYMBOL = ":/";

DirectoryReader::DirectoryReader() :
  errors_(),
  drivesInUse_()
{
}

DirectoryReader::~DirectoryReader()
{
}

app::AppInfo * DirectoryReader::readBigBlobbyInfo()
{
    checkThatFileExists( BIG_BLOBBY_EXECUTABLE, BIG_BLOBBY_DIR );
    QImage iconImage = loadImage( BIG_BLOBBY_IMAGE_FILE, BIG_BLOBBY_DIR );
    return new app::AppInfo( BIG_BLOBBY_EXECUTABLE, BIG_BLOBBY_DIR, iconImage );
}

app::AppInfo * DirectoryReader::readTouchHooks2TuioInfo()
{
    checkThatFileExists( TOUCH_HOOKS_EXECUTABLE, TOUCH_HOOKS_DIR );
    QImage iconImage = loadImage( TOUCH_HOOKS_IMAGE_FILE, TOUCH_HOOKS_DIR );
    return new app::AppInfo( TOUCH_HOOKS_EXECUTABLE, TOUCH_HOOKS_DIR, iconImage );
}

void DirectoryReader::checkThatFileExists( const QString & filename, 
                                           const QString & fullSubdirName )
{
    QString pathToExecutable = fullSubdirName + "/" + filename;
    QFileInfo fileInfo( pathToExecutable );

    if( !fileInfo.exists() ) {
        throw FileNotFoundException( "File Not Found: " + pathToExecutable,
                                     "DirectoryReader::checkThatFileExists()",
                                     pathToExecutable );
    }
}

QImage DirectoryReader::loadImage( const QString & filename, 
                                   const QString & fullSubdirName )
{
    QImage image;
    QString pathToImageFile = fullSubdirName + "/" + filename;
    bool ok = image.load( pathToImageFile );

    if( !ok ) {
        QFileInfo fileInfo( pathToImageFile );
        
        if( fileInfo.exists() && !fileInfo.isReadable() ) {
            throw FileNotReadableException( "Image Not Readable: " + pathToImageFile,
                                            "DirectoryReader::loadImage()",
                                            pathToImageFile );
        }
        else {
            throw FileNotFoundException( "Image Not Found: " + pathToImageFile,
                                         "DirectoryReader::loadImage()",
                                         pathToImageFile );
        }
    }
    return image;
}

app::AppInfo * DirectoryReader::readFlashPlayerInfo()
{
    checkThatFileExists( FLASH_PLAYER_EXECUTABLE, FLASH_PLAYER_DIR );
    QImage iconImage = loadImage( FLASH_PLAYER_IMAGE_FILE, FLASH_PLAYER_DIR );
    return new app::AppInfo( FLASH_PLAYER_EXECUTABLE, FLASH_PLAYER_DIR, iconImage );
}

void DirectoryReader::readMultitouchAppsInfo( app::AppInfoList * appInfoList )
{
    clearListOfDriveNames();
    readMultitouchAppsInfo( MULTI_TOUCH_DIR, appInfoList );
    readMultitouchAppsOnUsbDrive( appInfoList );
}

void DirectoryReader::readMultitouchAppsInfo( const QString & multitouchDirName,
                                              app::AppInfoList * appInfoList )
{
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks;
    QDir multitouchDir( multitouchDirName );
    QFileInfoList dirInfoList = multitouchDir.entryInfoList( filters );

    for( int i = 0; i < dirInfoList.size(); ++i ) {
        QFileInfo fileInfo = dirInfoList.at( i );
        QString fullSubdirName = multitouchDirName + "/" + fileInfo.fileName();
        //std::cout << "pathToSubdir = " << fullSubdirName.toStdString() << "\n";
        readDir( fullSubdirName, appInfoList );
    }
}

void DirectoryReader::readDir( const QString & fullSubdirName, 
                               app::AppInfoList * appInfoList )
{
    QString appFilename = multitouchAppFilename( fullSubdirName );

    if( appFilename.size() > 0 ) {
        QString iconFilename = iconImageFilename( fullSubdirName );

        if( iconFilename.size() > 0 ) {
            createAppInfo( appFilename, iconFilename, fullSubdirName, appInfoList );
        }
        else { 
            QString errorSrc = "DirectoryReader::readDir";
            QString msg = "An image file ending with '-playsurface-icon.png' was\n";
            msg += "not found for '" + appFilename + "'\n";
            msg += "in the directory '" + fullSubdirName + "'.\n";
            FileNotFoundException e( msg, errorSrc, "*-playsurface-icon.png" );
            errors_.push_back( e );
        }
    }
}

QString DirectoryReader::multitouchAppFilename( const QString & fullSubdirName )
{
    QStringList nameFilters;
    nameFilters << "*.exe" << "*.swf";
    return filename( fullSubdirName, nameFilters );
}

QString DirectoryReader::filename( const QString & fullSubdirName, 
                                   const QStringList & nameFilters )
{
    QString filename( "" );
    QDir dir( fullSubdirName );
    QDir::Filters filters = QDir::Files | QDir::NoSymLinks;
    QFileInfoList fileInfoList = dir.entryInfoList( nameFilters, filters );

    if( fileInfoList.size() > 0 ) {
        QFileInfo fileInfo = fileInfoList.at( 0 );
        filename = fileInfo.fileName();
    }
    return filename;
}

QString DirectoryReader::iconImageFilename( const QString & fullSubdirName )
{
    QStringList nameFilters;
    nameFilters << "*-playsurface-icon.png";
    return filename( fullSubdirName, nameFilters );
}

void DirectoryReader::createAppInfo( const QString & appFilename, 
                                     const QString & iconFilename, 
                                     const QString & fullSubdirName,
                                     app::AppInfoList * appInfoList )
{
    try {
        QImage iconImage = loadImage( iconFilename, fullSubdirName );
        checkThatFileExists( appFilename, fullSubdirName );
        appInfoList->add( new app::AppInfo( appFilename, fullSubdirName, iconImage ) );
        addDrivenameToList( fullSubdirName );
    }
    catch( IOException e ) {
        errors_.push_back( e );
    }
}

void DirectoryReader::addDrivenameToList( const QString & fullSubDirname )
{
    QDir dir( fullSubDirname );
    QString absolutePath = dir.absolutePath();

    if( absolutePath.size() >= 3 ) {
        QString driveName = absolutePath.left( 3 );
        //std::cout << "driveName added to set: " << driveName.toStdString() << "\n";
        drivesInUse_.insert( driveName );
    }
}

void DirectoryReader::clearListOfDriveNames()
{
    drivesInUse_.clear();
}

void DirectoryReader::readMultitouchAppsOnUsbDrive( app::AppInfoList * appInfoList )
{
    QFileInfoList driveList = QDir::drives();

    for( int i = 0; i < driveList.size(); ++i ) {
        QFileInfo driveInfo = driveList.at( i );
        QDir dir( driveInfo.filePath() );
        QString multitouchDirName = multitouchDirectory( dir );
        
        if( multitouchDirName.size() > 0 ) {
            //std::cout << "multitouchDirName: " << multitouchDirName.toStdString() << "\n";
            readMultitouchAppsInfo( multitouchDirName, appInfoList );
        }
    }
}

QString DirectoryReader::multitouchDirectory( const QDir & dir )
{
    QString multitouchDir = "";
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks;
    QFileInfoList dirInfoList = dir.entryInfoList( filters );

    for( int i = 0; i < dirInfoList.size(); ++i ) {
        QFileInfo fileInfo = dirInfoList.at( i );
        QString path = fileInfo.filePath();
        //std::cout << "Path = " << path.toStdString() << "\n";
        
        if( path.endsWith( USB_DRIVE_MULTI_TOUCH_DIR, Qt::CaseInsensitive ) ) {
            multitouchDir = path;
            break;
        }
    }
    return multitouchDir;
}

bool DirectoryReader::hasAppInfoErrors()
{
    return ( errors_.size() > 0 );
}

std::vector<exceptions::IOException> DirectoryReader::appInfoErrors()
{
    return errors_;
}

bool DirectoryReader::hasMultitouchAppsUsbDriveDir( char driveLetter )
{
    QDir dir( driveLetter + DRIVE_SYMBOL );
    QString multitouchDirName = multitouchDirectory( dir );
    return (multitouchDirName.size() > 0);
}

bool DirectoryReader::isMultitouchAppsDirInUse( char driveLetter )
{
    return drivesInUse_.contains( driveLetter + DRIVE_SYMBOL );
}
