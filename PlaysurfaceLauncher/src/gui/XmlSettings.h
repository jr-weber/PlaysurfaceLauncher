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
#ifndef GUI_XMLSETTINGS_H
#define GUI_XMLSETTINGS_H

#include "enums/TuioSourceEnum.h"
#include <QObject>
#include <QString>

namespace xml { class SettingsXmlReader; }
namespace xml { class SettingsXmlWriter; }
namespace xml { class SettingsValidator; }
namespace gui { class XmlDialogBoxUtils; }
namespace gui { class FullScreenWindow; }

namespace gui
{
    class XmlSettings : public QObject
    {
        Q_OBJECT

    public:
        static const QString DEFAULT_CONFIG_FILE;

        XmlSettings();
        ~XmlSettings();

        enums::TuioSourceEnum::Enum tuioSource();
        bool isBigBlobbySelected();
        bool isTouchHooks2TuioSelected();
        bool isWindows8TouchSelected();

    public slots:
        void readXmlConfigFile();
        void updateMainWindowSettings( gui::FullScreenWindow * mainWindow );
        void saveSettingsToXmlFile( gui::FullScreenWindow * mainWindow );

    private:
        void useXmlFileToUpdateValidator();
        void useValidatorToUpdateXmlFile();

        xml::SettingsXmlReader * xmlReader_;
        xml::SettingsXmlWriter * xmlWriter_;
        xml::SettingsValidator * validator_;
        gui::XmlDialogBoxUtils * dialogBox_;
    };
}

#endif
