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
#include "logger/FileLogger.h"

// Disable deprecation warning for _vsnwprintf in boost/log/snprintf.hpp.
#pragma warning(push)
#pragma warning(disable: 4996)
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#pragma warning(pop)

using logger::FileLogger;

const std::string FileLogger::LOG_FILE_NAME( "Data/Log/PlaysurfaceLauncher-Log_%N.log" ),
                  FileLogger::LOG_DIRECTORY( "Data/Log" ),
                  FileLogger::MESSAGE_FORMAT( "[%TimeStamp%] %Message%" );

const int FileLogger::MAX_FILE_SIZE = 100 * 1024,
          FileLogger::MAX_DIRECTORY_SIZE = 2 * 1024 * 1024,
          FileLogger::DEFAULT_SEVERITY_LEVEL = boost::log::trivial::warning;;

/***************************************************************************//**
The constructor initializes the Boost Log library.
*******************************************************************************/
FileLogger::FileLogger()
{
    initialize();
}

FileLogger::~FileLogger()
{
}

/***************************************************************************//**
The Singleton Design Pattern is used here so that the Boost Log library code 
is only initialized once.
*******************************************************************************/
logger::Log & FileLogger::instance()
{
    static FileLogger instance;
    return instance;
}

void FileLogger::initialize()
{
    setFileInfo();
    setFilter();
    addCommonAttributes();
}

void FileLogger::setFileInfo()
{
    boost::log::add_file_log
    (
        boost::log::keywords::file_name = LOG_FILE_NAME,
        boost::log::keywords::target = LOG_DIRECTORY,
        boost::log::keywords::rotation_size = MAX_FILE_SIZE,
        boost::log::keywords::max_size = MAX_DIRECTORY_SIZE,
        boost::log::keywords::format = MESSAGE_FORMAT
    );
}

void FileLogger::setFilter()
{
    boost::log::core::get()->set_filter
    ( 
        boost::log::trivial::severity >= DEFAULT_SEVERITY_LEVEL 
    );
}

void FileLogger::addCommonAttributes()
{
    boost::log::add_common_attributes();
}

void FileLogger::writeTrace( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::trace ) << message;
}

void FileLogger::writeDebug( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::debug ) << message;
}

void FileLogger::writeInfo( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::info ) << message;
}

void FileLogger::writeWarning( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::warning ) << message;
}

void FileLogger::writeError( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::error ) << message;
}

void FileLogger::writeFatal( const std::string & message )
{
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > severityLogger;
    BOOST_LOG_SEV( severityLogger, boost::log::trivial::fatal ) << message;
}
