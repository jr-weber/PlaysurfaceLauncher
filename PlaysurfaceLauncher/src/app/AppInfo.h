/*******************************************************************************
AppInfo

PURPOSE: Holds information on any application that can be launched by 
         the PlaysurfaceLauncher: the path and filename of an executable as well
         as an icon to represent that app on the PlaysurfaceLauncher desktop.

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
#ifndef APP_APPINFO_H
#define APP_APPINFO_H

#include <QString>
#include <QImage>

namespace app
{
    class AppInfo
    {
    public:
        AppInfo( const QString & filename, 
                 const QString & directory, 
                 const QImage & icon );
        virtual ~AppInfo();

        QString directory();
        QString absoluteDirectory();
        QString filename();
        QString pathToExecutable();
        QString absolutePathToFile();
        QImage iconImage();
        bool isFlash();

    private:
        QString directory_,
                filename_;
        QImage iconImage_;
    };
}

#endif
