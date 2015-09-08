/*******************************************************************************
FileLogger

PURPOSE: This concrete implementation of the abstract Log class uses the 
         Log library of Boost to write log messages to a text file.

AUTHOR:  J.R. Weber <joe.weber77@gmail.com>
CREATED: 4/19/2015
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
#ifndef LOGGER_FILELOGGER_H
#define LOGGER_FILELOGGER_H

#include "logger/Log.h"

namespace logger 
{
    class FileLogger : public Log
    {
    private:
        FileLogger();

    public:
        static const std::string LOG_FILE_NAME,
                                 LOG_DIRECTORY,
                                 MESSAGE_FORMAT;
        static const int MAX_FILE_SIZE,
                         MAX_DIRECTORY_SIZE,
                         DEFAULT_SEVERITY_LEVEL;

        virtual ~FileLogger();
        static Log & instance();

        virtual void initialize();
        virtual void writeTrace( const std::string & message );
        virtual void writeDebug( const std::string & message );
        virtual void writeInfo( const std::string & message );
        virtual void writeWarning( const std::string & message );
        virtual void writeError( const std::string & message );
        virtual void writeFatal( const std::string & message );

    private:
        void setFileInfo();
        void setFilter();
        void addCommonAttributes();
    };
}

#endif
