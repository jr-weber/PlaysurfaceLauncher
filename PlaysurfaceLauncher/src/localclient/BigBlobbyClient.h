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
#ifndef LOCALCLIENT_BIGBLOBBYCLIENT_H
#define LOCALCLIENT_BIGBLOBBYCLIENT_H

#include <QObject>
#include <QTcpSocket>

namespace logger { class Log; }
class QNetworkSession;
class QByteArray;

namespace localclient
{
    class BigBlobbyClient : public QObject
    {
        Q_OBJECT

    public:
        static const int DEFAULT_PORT_NUMBER;
        static const QString SAVE_SETTINGS_MESSAGE,
                             SUCCESS_MESSAGE,
                             FAILURE_MESSAGE;

        BigBlobbyClient();
        ~BigBlobbyClient();

        void setPortNumber( int port );
        int portNumber();

    public slots:
        void tellBigBlobbyToSaveSettingsToXmlFiles();
        void readBigBlobbyResponse();
        void displayError( QAbstractSocket::SocketError socketError );
        void sessionOpened();

    signals:
        void bigBlobbySavedSettingsToXmlFiles();

    private:
        void addSaveSettingsMessageToBlock( QByteArray & block );
        void logWarning( const QString & message );
        void logInfo( const QString & message );

        QNetworkSession * networkSession_;
        QTcpSocket * tcpSocket_;
        int portNumber_;
        logger::Log & log_;
    };
}

#endif
