/*******************************************************************************
UnknownXmlTagException

PURPOSE: For reporting an unrecognized tag in an XML file.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
Created: 8/25/2014
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
#ifndef EXCEPTIONS_UNKNOWNXMLTAGEXCEPTION_H
#define EXCEPTIONS_UNKNOWNXMLTAGEXCEPTION_H

#include "exceptions/IOException.h"

namespace exceptions
{
    /***********************************************************************//**
    For reporting an unrecognized tag in an XML file.
    ***************************************************************************/
    class UnknownXmlTagException : public exceptions::IOException
    {
        public:
            UnknownXmlTagException( const QString & message,
                                    const QString & source ,
                                    const QString & tagName, 
                                    const QString & filename );
            virtual ~UnknownXmlTagException();

            QString getTagName();
            QString getFilename();

        private:
            QString tagName_,
                    filename_;
    };
}

#endif
