/*******************************************************************************
SettingsXmlWriter

PURPOSE: Knows how to read program settings from a SettingsValidator object and 
         write them to an XML file.

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
#include "xml/SettingsXmlWriter.h"
#include "xml/SettingsValidator.h"
#include "exceptions/SettingsXmlWriterException.h"
#include <QFile>
#include <QTextStream>
#include <iostream>

using xml::SettingsXmlWriter;
using exceptions::SettingsXmlWriterException;

SettingsXmlWriter::SettingsXmlWriter() :
  debug_( false )
{
}

SettingsXmlWriter::~SettingsXmlWriter()
{
}

void SettingsXmlWriter::write( xml::SettingsValidator * validator, 
                               const QString & xmlFile )
{
    QFile file( xmlFile );

    if( file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate ) ) {
        QTextStream stream( &file );
        stream << getSettingsAsXml( validator );
        file.close();
        debugPrintLn( xmlFile + " was written out." );
    }
    else {
        QString msg( "" );
        throw SettingsXmlWriterException( msg, "BlobParamsXmlWriter::write()", xmlFile );
    }
}

void SettingsXmlWriter::printXmlToStandardOut( xml::SettingsValidator * validator )
{
    std::cout << getSettingsAsXml( validator ).toStdString();
}

QString SettingsXmlWriter::getSettingsAsXml( xml::SettingsValidator * validator )
{
    QString xml( "<PlaysurfaceLauncherSettings>\n\n" );
    xml.append( getOptionsXml( validator ) );
    xml.append( getGesturesXml( validator ) );
    xml.append( "</PlaysurfaceLauncherSettings>\n" );
    return xml;
}

QString SettingsXmlWriter::getOptionsXml( xml::SettingsValidator * validator )
{
    QString xml( "    <Options>\n" );
    xml.append( createXmlFromEnum( "TouchDetection", validator->tuioSource() ) );
    xml.append( createXmlFromBool( "ShowShutdownComputerIcon", validator->showShutdownComputerIcon() ) );
    xml.append( "    </Options>\n\n" );
    return xml;
}

QString SettingsXmlWriter::getGesturesXml( xml::SettingsValidator * validator )
{
    QString xml( "    <Gestures>\n" );
    xml.append( createXmlFromBool( "UsePlusSignToQuitApps", validator->usePlusGestureToQuitApps() ) );
    xml.append( "    </Gestures>\n\n" );
    return xml;
}

QString SettingsXmlWriter::createXmlFromBool( QString tag, bool b )
{
    return createXmlFromString( tag, (b ? "true" : "false") );
}

QString SettingsXmlWriter::createXmlFromInt( QString tag, int n )
{
    return createXmlFromString( tag, QString::number( n ) );
}

QString SettingsXmlWriter::createXmlFromDouble( QString tag, double n )
{
    return createXmlFromString( tag, QString::number( n, 'g', 3 ) );
}

QString SettingsXmlWriter::createXmlFromString( QString tag, QString theValue )
{
    return QString( "        <" + tag + "> " + theValue + " </" + tag + ">\n" );
}

QString SettingsXmlWriter::createXmlFromEnum( QString tag, enums::TuioSourceEnum::Enum theValue )
{
    QString enumAsString = enums::TuioSourceEnum::convertToString( theValue );
    return QString( "        <" + tag + "> " + enumAsString + " </" + tag + ">\n" );
}

void SettingsXmlWriter::debugPrintLn( const QString & msg )
{
    if( debug_ ) {
        std::cout << msg.toStdString() << "\n";
    }
}
