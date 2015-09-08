/*
    qTUIO - TUIO Interface for Qt

    Original Version by Martin Blankenburg <martin.blankenburg@imis.uni-luebeck.de>
    Integrated into qTUIO by x29a <0.x29a.0@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
NOTE: This class was obtained from GitHub (https://github.com/x29a/qTUIO) and
      slightly modified to work with the PlaysurfaceLauncher program.
          J.R. Weber <joe.weber77@gmail.com>
*/
#ifndef QTUIO_QTUIO_H
#define QTUIO_QTUIO_H

#include "TuioListener.h"
//#include <QThread>
#include <QMap>
#include <QEvent>
#include <QTouchEvent>

namespace TUIO { class TuioClient; }
namespace TUIO { class TuioCursor; }
namespace TUIO { class TuioBlob; }

class QGraphicsView;
class QGraphicsScene;
class QMainWindow;
class QWidget;
class QRect;

namespace qtuio 
{
    class QTuio : public TUIO::TuioListener //, public QThread
    {
    public:
        static const int DEFAULT_TUIO_UDP_PORT;

        explicit QTuio( QObject * parent = 0 );
        virtual ~QTuio();

        void allowTuioProfileFiltering( bool b );
        void setTuioUdpPort( int port );
        void run();

        virtual void addTuioCursor( TUIO::TuioCursor *tcur );
        virtual void updateTuioCursor( TUIO::TuioCursor * tcur );
        virtual void removeTuioCursor( TUIO::TuioCursor * tcur );

        virtual void addTuioBlob( TUIO::TuioBlob * tblb );
        virtual void updateTuioBlob( TUIO::TuioBlob * tblb );
        virtual void removeTuioBlob( TUIO::TuioBlob * tblb );

        virtual void addTuioObject( TUIO::TuioObject * tobj );
        virtual void updateTuioObject( TUIO::TuioObject * tobj );
        virtual void removeTuioObject( TUIO::TuioObject * tobj );

        virtual void refresh( TUIO::TuioTime ftime );

    private:
        bool tuioToQt( TUIO::TuioCursor * tcur, QEvent::Type eventType );
        void setScenePosAndRect( QTouchEvent::TouchPoint & touchPoint, const QPointF & screenPos );
        void setPositionBasedOnTheScene( QTouchEvent::TouchPoint & touchPoint, 
                                         const QPointF & screenPos );
        void setPositionBasedOnTheCentralWidget( QTouchEvent::TouchPoint & touchPoint, 
                                                 const QPointF & screenPos );
        Qt::TouchPointStates addTouchPoint( QTouchEvent::TouchPoint & touchPoint,
                                            QEvent::Type eventType, 
                                            const QPointF & normPos, 
                                            const QPointF & screenPos, 
                                            int id, 
                                            double speed );
        void addNewTouchPoint( QTouchEvent::TouchPoint & touchPoint, 
                               const QPointF & normPos, 
                               const QPointF & screenPos, 
                               int id );
        void addExistingTouchPoint( QTouchEvent::TouchPoint & touchPoint, int id );
        void sendTouchEvent( QEvent::Type eventType, Qt::TouchPointStates touchPointStates, int id );

        bool tuioToQt( TUIO::TuioBlob * tblb, QEvent::Type eventType );
        bool tuioToQt( TUIO::TuioObject * tobj, QEvent::Type eventType );
        void printScreenRect();
        void debugPrintWindowPointers();
        void debugPrintPositions( const QPointF & screenPos,
                                  const QRect & centralWidgetRect,
                                  const QPointF & pointPos );

        QMainWindow * theMainWindow_;
        QWidget * theCentralWidget_;
        QGraphicsView * theView_;
        QGraphicsScene * theScene_;
        QRect screenRect_;
        double screenArea_;
        TUIO::TuioClient * tuioClient_;
        QMap<int, QTouchEvent::TouchPoint> * qTouchPointMap_;
        bool running_;
        int tuioUdpPort_;
    };
}

#endif
