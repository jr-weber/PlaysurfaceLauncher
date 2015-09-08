/*******************************************************************************
TouchHooksClient

PURPOSE: Uses a QLocalSocket to contact TouchHooks2Tuio and request that the
         global hook be released.  Releasing resources before shutdown is 
         always a good idea.  Because of the way the AppLauncher shuts down
         TouchHooks2Tuio, it does not have time to clean up resources, which 
         is why this class should be used before killing TouchHooks2Tuio.
         The following constants have the same definitions in TouchHooks2Tuio
         as here: DEFAULT_SERVER_NAME, RELEASE_HOOKS_MESSAGE, SUCCESS_MESSAGE,
         and FAILURE_MESSAGE;

ORIGIN: This class is a highly modified version of the Qt framework Local
        Fortune Client demo found at http://doc.qt.io/qt-5/examples-ipc.html
            J.R. Weber <joe.weber77@gmail.com>
*******************************************************************************/
/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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
#include "localclient/TouchHooksClient.h"
#include "logger/FileLogger.h"
#include <QLocalSocket>
#include <QtNetwork>
#include <iostream>

using localclient::TouchHooksClient;

const QString TouchHooksClient::DEFAULT_SERVER_NAME = "TouchHooks2Tuio-LocalServer",
              TouchHooksClient::RELEASE_HOOKS_MESSAGE = "TouchHooks2Tuio:RELEASE_HOOKS",
              TouchHooksClient::SUCCESS_MESSAGE = "TouchHooks2Tuio:Success",
              TouchHooksClient::FAILURE_MESSAGE = "TouchHooks2Tuio:Failure";

TouchHooksClient::TouchHooksClient() :
  localSocket_( new QLocalSocket( this ) ),
  serverName_( DEFAULT_SERVER_NAME ),
  log_( logger::FileLogger::instance() )
{
    connect( localSocket_, 
             SIGNAL( readyRead() ),
             this, 
             SLOT( readTouchHooks2TuioResponse() ) );

    connect( localSocket_, 
             SIGNAL( error( QLocalSocket::LocalSocketError ) ),
             this, 
             SLOT( displayError( QLocalSocket::LocalSocketError ) ) );
}

TouchHooksClient::~TouchHooksClient()
{
    if( localSocket_ != NULL ) {
        localSocket_->abort();
        delete localSocket_;
    }
}

void TouchHooksClient::setServerName( QString & serverName )
{
    serverName_ = serverName;
}

QString TouchHooksClient::serverName()
{
    return serverName_;
}

void TouchHooksClient::tellTouchHooks2TuioToReleaseGlobalHook()
{
    if( localSocket_ != NULL ) {
        localSocket_->abort();
        localSocket_->connectToServer( serverName_ );
        bool isConnected = localSocket_->waitForConnected();

        if( isConnected ) {
            QByteArray block;
            addReleaseGlobalHookMessageToBlock( block );
            localSocket_->write( block );
            bool messageSentOk = localSocket_->waitForBytesWritten();

            if( messageSentOk ) {
                logInfo( "Message sent to TouchHooks2Tuio.\n" );
            }
            else {
                logWarning( "Message not sent to TouchHooks2Tuio." );
            }
        }
        else {
            logWarning( "Could not connect to TouchHooks2Tuio.\n" );
        }
    }
}

void TouchHooksClient::addReleaseGlobalHookMessageToBlock( QByteArray & block )
{
    QDataStream out( &block, QIODevice::ReadWrite );
    out.setVersion( QDataStream::Qt_5_4 );
    out << (quint16)0;
    out << RELEASE_HOOKS_MESSAGE;
    out.device()->seek( 0 );
    out << (quint16)(block.size() - sizeof( quint16 ));
}

void TouchHooksClient::readTouchHooks2TuioResponse()
{
    QDataStream in( localSocket_ );
    in.setVersion( QDataStream::Qt_5_4 );

    if( localSocket_->bytesAvailable() < (int)sizeof(quint16) ) {
        return;
    }
    quint16 responseBlockSize;
    in >> responseBlockSize;
    
    if( localSocket_->bytesAvailable() < responseBlockSize ) {
        return;
    }
    QString touchHooksResponse;
    in >> touchHooksResponse;
    //std::cout << "TouchHooks2Tuio response = " << touchHooksResponse.toStdString() << "\n";

    if( touchHooksResponse.compare( SUCCESS_MESSAGE, Qt::CaseInsensitive ) == 0 ) {
        emit touchHooks2TuioReleasedGlobalHook();
    }
}

void TouchHooksClient::displayError( QLocalSocket::LocalSocketError socketError )
{
    switch( socketError ) {
        case QLocalSocket::ServerNotFoundError:
            logWarning( "The TouchHooks2Tuio LocalServer could not be found." );
            break;
        case QLocalSocket::ConnectionRefusedError:
            logWarning( "The TouchHooks2Tuio LocalServer refused the connection." );
            break;
        default:
            logWarning( "An unexpected problem occurred: "
                        + localSocket_->errorString() );
    }
}


void TouchHooksClient::logWarning( const QString & message )
{
    log_.writeWarning( "TouchHooksClient Warning: " + message.toStdString() );
}

void TouchHooksClient::logInfo( const QString & message )
{
    log_.writeInfo( "TouchHooksClient Info: " + message.toStdString() );
}
