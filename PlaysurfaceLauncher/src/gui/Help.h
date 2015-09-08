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
#ifndef GUI_HELP_H
#define GUI_HELP_H

#include <QObject>
#include <QUrl>

namespace gui { class HelpMenuWidget; }

namespace gui 
{
    class Help : public QObject
    {
        Q_OBJECT

    public:
        static const QString PLAYSURFACE_ICON;
        static const QUrl PLAYSURFACE_HELP_URL;
        static const QString ABOUT_BOX_MSG;

        Help();
        virtual ~Help();

    public slots:
        void openAboutBox();
        void openOnlineHelp();
        void openMenuOptionsHelpWidget();
        void closeHelpMenuWidget();

    signals:

    private:
        gui::HelpMenuWidget * helpMenuWidget_;
    };
}

#endif
