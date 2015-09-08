/*******************************************************************************
AppInfoList

PURPOSE: For managing a list of AppInfo objects.  An AppInfo object will hold
         the path/filename of an executable along with an icon to represent
         the application on the PlaysurfaceLauncher desktop.

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
#include "app/AppInfoList.h"
#include "app/AppInfo.h"

using app::AppInfoList;

AppInfoList::AppInfoList() :
  appInfos_(),
  appIcons_()
{
}

AppInfoList::~AppInfoList()
{
    clear();
}

void AppInfoList::clear()
{
    for( int i = 0; i < appInfos_.size(); ++i ) {
        delete appInfos_.at( i );
    }
    appInfos_.clear();
    appIcons_.clear();
}

void AppInfoList::add( app::AppInfo * appInfo )
{
    appInfos_.push_back( appInfo );
    appIcons_.push_back( appInfo->iconImage() );
}

app::AppInfo * AppInfoList::appInfo( int index )
{
    if( index < appInfos_.size() ) {
        return appInfos_.at( index );
    }
    return NULL;
}

int AppInfoList::size()
{
    return appInfos_.size();
}

std::vector<app::AppInfo *> * AppInfoList::appInfos()
{
    return &appInfos_;
}

std::vector<QImage> * AppInfoList::appIcons()
{
    return &appIcons_;
}
