/*******************************************************************************
ProjectorPort

PURPOSE: For turning a BenQ projector on/off.  Depends on having a USB to serial
         RS-232 adapter cable for connecting the computer to the projector.

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
#include "serialport/ProjectorPort.h"
#include "qextserialport/qextserialenumerator.h"
#include "qextserialport/qextserialport.h"
#include <QList>
#include <iostream>

using serialport::ProjectorPort;

ProjectorPort::ProjectorPort()
{
    //debugPrintPorts();
    //debugPrintComPorts();
}

ProjectorPort::~ProjectorPort()
{
}

void ProjectorPort::turnOnProjectorIfFound()
{
    sendCommand( "\r*pow=on#\r" );
}

void ProjectorPort::turnOffProjectorIfFound()
{
    sendCommand( "\r*pow=off#\r" );
}

void ProjectorPort::sendCommand( const QString & cmd )
{
    QList<QString> comPortNames = listComPorts();

    foreach( QString portName, comPortNames ) {
        QextSerialPort * port = createPort( portName );

        if( port->open( QIODevice::ReadWrite ) ) {
            int ok = port->write( cmd.toLatin1() );
            //std::cout << "Device opened: " << portName.toStdString()
            //          << "\n    write = " << ok << "\n";
        }
        else {
            std::string err = port->errorString().toStdString();
            std::cout << "Device failed to open: " << err << "\n";
        }
        delete port;
    }
}

QextSerialPort * ProjectorPort::createPort( const QString & portName )
{
    QextSerialPort * port = new QextSerialPort( portName.toLatin1(), QextSerialPort::Polling );
    port->setBaudRate( BAUD115200 );
    port->setFlowControl( FLOW_OFF );
    port->setParity( PAR_NONE );
    port->setDataBits( DATA_8 );
    port->setStopBits( STOP_1 );
    return port;
}

QList<QString> ProjectorPort::listComPorts()
{
    QList<QString> comPortNames;
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    foreach( QextPortInfo info, ports ) {
        QString portName = info.portName;

        if( portName.startsWith( "COM" ) ) {
            comPortNames.append( portName );
        }
    }
    return comPortNames;
}

void ProjectorPort::debugPrintPorts()
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    std::cout << "\nProjectorPort::debugPrintPorts() called.\n"
              << "List of ports:\n\n";

    foreach( QextPortInfo info, ports ) {
        std::cout << "Port Name:       " << info.portName.toStdString()   << "\n"
                  << "Friendly Name:   " << info.friendName.toStdString() << "\n"
                  << "Physical Name:   " << info.physName.toStdString()   << "\n"
                  << "Enumerator Name: " << info.enumName.toStdString()   << "\n"
                  << "Vendor ID:       " << info.vendorID                 << "\n"
                  << "Product ID:      " << info.productID                << "\n\n";
    }
}

void ProjectorPort::debugPrintComPorts()
{
    QList<QString> comPortNames( listComPorts() );
    std::cout << "\nProjectorPort::debugPrintComPorts() called.\n";

    foreach( QString portName, comPortNames ) {
        std::cout << "COM port = " << portName.toStdString() << "\n";
    }
    std::cout << "\n";
}
