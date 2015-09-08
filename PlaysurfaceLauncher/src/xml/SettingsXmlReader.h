/*******************************************************************************
SettingsXmlReader

PURPOSE: Reads program settings from an XML file.

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
#ifndef XML_SETTINGSXMLREADER_H
#define XML_SETTINGSXMLREADER_H

#include "exceptions/ValidatorException.h"
#include "exceptions/UnknownXmlTagException.h"
#include <QString>
#include <QtCore/QByteArray>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <vector>

namespace xml { class SettingsValidator; }

namespace xml
{
    /***********************************************************************//**
    Reads program settings from an XML file.
    ***************************************************************************/
    class SettingsXmlReader
    {
    public:
        SettingsXmlReader();
        ~SettingsXmlReader();

        void read( const QString & xmlFile, xml::SettingsValidator * validator );

        bool hasUnknownXmlTagExceptions();
        std::vector<exceptions::UnknownXmlTagException> unknownXmlTagExceptions();
        bool hasValidatorExceptions();
        std::vector<exceptions::ValidatorException> validatorExceptions();

    private:
        void extractParams( QDomDocument & doc, xml::SettingsValidator * validator );
        void storeParams( QDomElement element, xml::SettingsValidator * validator );
        void storeOptionsParams( QDomNode & node, xml::SettingsValidator * validator );
        void storeGesturesParams( QDomNode & node, xml::SettingsValidator * validator );

        void debugPrintLn( const QString & msg );

        std::vector<exceptions::UnknownXmlTagException> unknownXmlTagExceptions_;
        std::vector<exceptions::ValidatorException> validatorExceptions_;
        QString xmlFile_;
        bool debugPrint_;
    };
}

#endif
