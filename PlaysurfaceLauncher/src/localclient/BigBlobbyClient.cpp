/*******************************************************************************
BigBlobbyClient

PURPOSE: Uses a QTcpSocket to tell BigBlobby to save any changes to its XML
         config file because the PlaysurfaceLauncher is about to kill it.
         At some point this class should probably be rewritten to use a 
         QLocalSocket (and a QLocalServer on BigBlobby) just to avoid using
         yet another port number (a named pipe is used instead).
         The TouchHooksClient class can be used as a model of how to do that.

ORIGIN: This class is a highly modified version of the Qt TCP Local Client demo
        found at http://doc.qt.io/qt-5/qtnetwork-fortuneclient-example.html
            J.R. Weber <joe.weber77@gmail.com>
*******************************************************************************/
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "localclient/BigBlobbyClient.h"
#include "logger/FileLogger.h"
#include <QtNetwork>
#include <iostream>

using localclient::BigBlobbyClient;

const int BigBlobbyClient::DEFAULT_PORT_NUMBER = 3002;
const QString BigBlobbyClient::SAVE_SETTINGS_MESSAGE = "BigBlobby:SaveSettingsToXmlFiles",
              BigBlobbyClient::SUCCESS_MESSAGE = "BigBlobby:Success",
              BigBlobbyClient::FAILURE_MESSAGE = "BigBlobby:Failure";

BigBlobbyClient::BigBlobbyClient() :
  networkSession_( NULL ),
  tcpSocket_( new QTcpSocket( this ) ),
  portNumber_( DEFAULT_PORT_NUMBER ),
  log_( logger::FileLogger::instance() )
{
    connect( tcpSocket_, SIGNAL( readyRead() ), this, SLOT( readBigBlobbyResponse() ) );
    connect( tcpSocket_, SIGNAL( error( QAbstractSocket::SocketError ) ),
             this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

    QNetworkConfigurationManager manager;

    if( manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired ) {
        // Get saved network configuration
        QSettings settings( QSettings::UserScope, QLatin1String( "BigBlobby" ) );
        settings.beginGroup( QLatin1String( "QtNetwork" ) );
        const QString id = settings.value( QLatin1String( "DefaultNetworkConfiguration" ) ).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier( id );

        if( (config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered ) {
            config = manager.defaultConfiguration();
        }
        networkSession_ = new QNetworkSession( config, this );
        connect( networkSession_, SIGNAL( opened() ), this, SLOT( sessionOpened() ) );
        networkSession_->open();
    }
}

BigBlobbyClient::~BigBlobbyClient()
{
    if( tcpSocket_ != NULL ) {
        tcpSocket_->abort();
        delete tcpSocket_;
    }
    delete networkSession_;
}

void BigBlobbyClient::setPortNumber( int port )
{
    portNumber_ = port;
}

int BigBlobbyClient::portNumber()
{
    return portNumber_;
}

void BigBlobbyClient::tellBigBlobbyToSaveSettingsToXmlFiles()
{
    if( tcpSocket_ != NULL ) {
        tcpSocket_->abort();
        tcpSocket_->connectToHost( QHostAddress::LocalHost, portNumber_ );
        bool isConnected = tcpSocket_->waitForConnected();

        if( isConnected ) {
            QByteArray block;
            addSaveSettingsMessageToBlock( block );
            tcpSocket_->write( block );
            bool messageSentOk = tcpSocket_->waitForBytesWritten();

            if( messageSentOk ) {
                logInfo( "Message sent to BigBlobby.\n" );
            }
            else {
                logWarning( "Message not sent to BigBlobby." );
            }
        }
        else {
            logWarning( "Could not connect to BigBlobby.\n" );
        }
    }
}

void BigBlobbyClient::addSaveSettingsMessageToBlock( QByteArray & block )
{
    QDataStream out( &block, QIODevice::ReadWrite );
    out.setVersion( QDataStream::Qt_4_0 );
    out << (quint16)0;
    out << SAVE_SETTINGS_MESSAGE;
    out.device()->seek( 0 );
    out << (quint16)(block.size() - sizeof( quint16 ));
}

void BigBlobbyClient::readBigBlobbyResponse()
{
    QDataStream in( tcpSocket_ );
    in.setVersion( QDataStream::Qt_4_0 );

    if( tcpSocket_->bytesAvailable() < (int)sizeof(quint16) ) {
        return;
    }
    quint16 responseBlockSize;
    in >> responseBlockSize;
    
    if( tcpSocket_->bytesAvailable() < responseBlockSize ) {
        return;
    }
    QString bigBlobbyResponse;
    in >> bigBlobbyResponse;
    //std::cout << "BigBlobby response = " << bigBlobbyResponse.toStdString() << "\n";

    if( bigBlobbyResponse.compare( SUCCESS_MESSAGE, Qt::CaseInsensitive ) == 0 ) {
        //std::cout << "    emit bigBlobbySavedSettingsToXmlFiles()\n";
        emit bigBlobbySavedSettingsToXmlFiles();
    }
}

void BigBlobbyClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            logWarning( "The BigBlobby SimpleMessageServer closed." );
            break;
        case QAbstractSocket::HostNotFoundError:
            logWarning( "The BigBlobby SimpleMessageServer could not be found." );
            break;
        case QAbstractSocket::ConnectionRefusedError:
            logWarning( "The BigBlobby SimpleMessageServer refused the connection." );
            break;
        default:
            logWarning( "An unexpected problem occurred: "
                        + tcpSocket_->errorString() );
    }
}

void BigBlobbyClient::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession_->configuration();

    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice ) {
        id = networkSession_->sessionProperty( QLatin1String( "UserChoiceConfiguration" ) ).toString();
    }
    else {
        id = config.identifier();
    }
    QSettings settings( QSettings::UserScope, QLatin1String( "Trolltech" ) );
    settings.beginGroup( QLatin1String( "QtNetwork" ) );
    settings.setValue( QLatin1String( "DefaultNetworkConfiguration" ), id );
    settings.endGroup();
}

void BigBlobbyClient::logWarning( const QString & message )
{
    log_.writeWarning( "BigBlobbyClient Warning: " + message.toStdString() );
}

void BigBlobbyClient::logInfo( const QString & message )
{
    log_.writeInfo( "BigBlobbyClient Info: " + message.toStdString() );
}
