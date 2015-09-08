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
#include "xml/SettingsXmlReader.h"
#include "xml/SettingsValidator.h"
#include "exceptions/FileNotFoundException.h"
#include "exceptions/SettingsXmlReaderException.h"
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QtXml/QDomDocument>
#include <QtXml/QDomDocumentType>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomAttr>
#include <QtXml/QDomText>
#include <iostream>

using xml::SettingsXmlReader;
using exceptions::ValidatorException;
using exceptions::UnknownXmlTagException;
using exceptions::FileNotFoundException;
using exceptions::SettingsXmlReaderException;

SettingsXmlReader::SettingsXmlReader() :
  unknownXmlTagExceptions_(),
  validatorExceptions_(),
  xmlFile_( "unknown" ),
  debugPrint_( false )
{
}

SettingsXmlReader::~SettingsXmlReader()
{
}

void SettingsXmlReader::read( const QString & xmlFile, 
                              xml::SettingsValidator * validator )
{
    xmlFile_ = xmlFile;
    validator->setXmlConfigFilename( xmlFile_ );
    unknownXmlTagExceptions_.clear();
    validatorExceptions_.clear();
    debugPrintLn( "\nSettingsXmlReader::readXmlFile( " + xmlFile_ + " ) called..." );

    QFile file( xmlFile_ );
    QDomDocument doc;
    QString err;
    int line = 0,
        col = 0;

    if( !file.open( QIODevice::ReadOnly ) ) {
        QString msg( "The XML configuration file could not be found." );
        throw FileNotFoundException( msg, "SettingsXmlReader::read()", xmlFile );
    }
    if( !doc.setContent( file.readAll(), &err, &line, &col ) ) {
        QString msg( "An error occurred while reading the XML configuration file." );
        throw SettingsXmlReaderException( msg, "BlobParamsXmlReader::read()",
                                          err, line, col, xmlFile_ );
    }
    extractParams( doc, validator );
}

void SettingsXmlReader::extractParams( QDomDocument & doc, 
                                       xml::SettingsValidator * validator )
{
    QDomElement root = doc.documentElement();
    QDomNode childNode = root.firstChild();
    QString tag = root.tagName().trimmed();
    debugPrintLn( "root = <" + tag + ">" );

    if( tag.compare( "PlaysurfaceLauncherSettings", Qt::CaseInsensitive ) != 0 ) {
        QString msg( "Unrecognized XML tag found." );
        throw UnknownXmlTagException( msg, "SettingsXmlReader::extractParams()",
                                      tag, xmlFile_ );
    }
    while( !childNode.isNull() ) {
        if( childNode.isElement() ) {
            storeParams( childNode.toElement(), validator );
        }
        childNode = childNode.nextSibling();
    }
}

void SettingsXmlReader::storeParams( QDomElement element, 
                                     xml::SettingsValidator * validator )
{
    QString tag = element.tagName().trimmed();
    QDomNode childNode = element.firstChild();
    debugPrintLn( "   Element = <" + tag + ">" );
    tag = tag.toLower();

    if( tag == "options" ) {
        storeOptionsParams( childNode, validator );
    }
    else if( tag == "gestures" ) { 
        storeGesturesParams( childNode, validator );
    }
    else { 
        if( tag.size() == 0 ) { tag = "NO VALUE GIVEN"; }
        QString msg( "Unrecognized XML tag found." );
        UnknownXmlTagException e( msg, "SettingsXmlReader::storeParams()",
                                  tag, xmlFile_ );
        unknownXmlTagExceptions_.push_back( e );
    }
}

void SettingsXmlReader::storeOptionsParams( QDomNode & node, 
                                            xml::SettingsValidator * validator )
{
    while( !node.isNull() ) {
        if( node.isElement() ) {
            QDomElement subelement = node.toElement();
            QString tag = subelement.tagName().trimmed(),
                    text = subelement.text().trimmed();
            debugPrintLn( "        XML tag: " + tag + " = " + text );
            tag = tag.toLower();

            try {
                if( tag == "touchdetection" ) {
                    validator->setTuioSource( text );
                }
                else if( tag == "showshutdowncomputericon" ) { 
                    validator->showShutdownComputerIcon( text ); 
                }
                else { 
                    if( tag.size() == 0 ) { tag = "NO VALUE GIVEN"; }
                    QString msg( "Unrecognized XML tag found." );
                    UnknownXmlTagException e( msg, "SettingsXmlReader::storeOptionsParams()",
                                              tag, xmlFile_ );
                    unknownXmlTagExceptions_.push_back( e );
                }
            }
            catch( ValidatorException e ) {
                validatorExceptions_.push_back( e );
            }
        }
        node = node.nextSibling();
    }
}

void SettingsXmlReader::storeGesturesParams( QDomNode & node, 
                                             xml::SettingsValidator * validator )
{
    while( !node.isNull() ) {
        if( node.isElement() ) {
            QDomElement subelement = node.toElement();
            QString tag = subelement.tagName().trimmed(),
                    text = subelement.text().trimmed();
            debugPrintLn( "        XML tag: " + tag + " = " + text );
            tag = tag.toLower();

            try {
                if( tag == "useplussigntoquitapps" ) { 
                    validator->usePlusGestureToQuitApps( text ); 
                }
                else { 
                    if( tag.size() == 0 ) { tag = "NO VALUE GIVEN"; }
                    QString msg( "Unrecognized XML tag found." );
                    UnknownXmlTagException e( msg, "SettingsXmlReader::storeGesturesParams()",
                                              tag, xmlFile_ );
                    unknownXmlTagExceptions_.push_back( e );
                }
            }
            catch( ValidatorException e ) {
                validatorExceptions_.push_back( e );
            }
        }
        node = node.nextSibling();
    }
}

bool SettingsXmlReader::hasUnknownXmlTagExceptions()
{
    return (unknownXmlTagExceptions_.size() > 0);
}

std::vector<exceptions::UnknownXmlTagException> SettingsXmlReader::unknownXmlTagExceptions()
{
    return unknownXmlTagExceptions_;
}

bool SettingsXmlReader::hasValidatorExceptions()
{
    return (validatorExceptions_.size() > 0);
}

std::vector<exceptions::ValidatorException> SettingsXmlReader::validatorExceptions()
{
    return validatorExceptions_;
}

void SettingsXmlReader::debugPrintLn( const QString & msg )
{
    if( debugPrint_ ) {
        std::cout << msg.toStdString() << "\n";
    }
}
