/*******************************************************************************
XmlDialogBoxUtils

PURPOSE: Extracts info from an XML-related exception and uses a modal dialog 
         box to warn the user.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
CREATED: 3/15/2015
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
#ifndef GUI_XMLDIALOGBOXUTILS_H
#define GUI_XMLDIALOGBOXUTILS_H

#include "exceptions/FileNotFoundException.h"
#include "exceptions/SettingsXmlReaderException.h"
#include "exceptions/SettingsXmlWriterException.h"
#include "exceptions/UnknownXmlTagException.h"
#include "exceptions/ValidatorException.h"
#include <QString>
#include <vector>

namespace gui 
{
    /***********************************************************************//**
    Extracts info from an XML-related exception and uses a modal dialog 
    box to warn the user. 
    ***************************************************************************/
    class XmlDialogBoxUtils
    {

    public:
        XmlDialogBoxUtils();
        virtual ~XmlDialogBoxUtils();

        void warnUser( exceptions::FileNotFoundException & e );
        void warnUser( exceptions::SettingsXmlReaderException & e );
        void warnUser( exceptions::SettingsXmlWriterException & e );
        void warnUser( exceptions::UnknownXmlTagException & e );
        void warnUser( exceptions::ValidatorException & e );
        void warnUser( std::vector<exceptions::UnknownXmlTagException> & errors );
        void warnUser( std::vector<exceptions::ValidatorException> & errors );
        void warnUser( const QString & message );

    private:
        void openWarningMessageBox( const QString & title, const QString & message );
    };
}

#endif
