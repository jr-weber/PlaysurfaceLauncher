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
#ifndef LOCALCLIENT_TOUCHHOOKSCLIENT_H
#define LOCALCLIENT_TOUCHHOOKSCLIENT_H

#include <QObject>
#include <QString>
#include <QLocalSocket>

namespace logger { class Log; }
class QNetworkSession;
class QByteArray;

namespace localclient
{
    class TouchHooksClient : public QObject
    {
        Q_OBJECT

    public:
        static const QString DEFAULT_SERVER_NAME,
                             RELEASE_HOOKS_MESSAGE,
                             SUCCESS_MESSAGE,
                             FAILURE_MESSAGE;

        TouchHooksClient();
        ~TouchHooksClient();

        void setServerName( QString & name );
        QString serverName();

    public slots:
        void tellTouchHooks2TuioToReleaseGlobalHook();
        void readTouchHooks2TuioResponse();
        void displayError( QLocalSocket::LocalSocketError socketError );

    signals:
        void touchHooks2TuioReleasedGlobalHook();

    private:
        void addReleaseGlobalHookMessageToBlock( QByteArray & block );
        void logWarning( const QString & message );
        void logInfo( const QString & message );

        QLocalSocket * localSocket_;
        QString serverName_;
        logger::Log & log_;
    };
}

#endif
