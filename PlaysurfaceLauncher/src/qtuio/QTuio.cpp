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
#include "QTuio.h"
#include "TuioClient.h"
#include "TuioCursor.h"
#include <QEvent>
#include <QTouchEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QMainWindow>
#include <QtDebug>
#include <iostream>
#include <iomanip>

using qtuio::QTuio;

// CCV (Community Core Vision) sends TUIO UDP packets on port 3333,
// so that is the port the TuioClient should listen on by default.
// If needed, setTuioUdpPort(port) can be used to change the port number.
const int QTuio::DEFAULT_TUIO_UDP_PORT = 3333;

QTuio::QTuio( QObject * parent ) :
  theMainWindow_( qobject_cast<QMainWindow *>(parent) ),
  theCentralWidget_( NULL ),
  theView_( qobject_cast<QGraphicsView *>(parent) ),
  theScene_( NULL ),
  screenRect_(),
  screenArea_( 0.0 ),
  tuioClient_( NULL ),
  qTouchPointMap_( NULL ),
  running_( false ),
  tuioUdpPort_( DEFAULT_TUIO_UDP_PORT )
{
    // If a QMainWindow exists, get the central widget.
    if( theMainWindow_ != NULL ) {
        theCentralWidget_ = theMainWindow_->centralWidget();
    }
    else if( theView_ != NULL ) { // Must be a QGraphicsScene instead.
        theScene_ = theView_->scene();
    }
    else {
        theScene_ = qobject_cast<QGraphicsScene *>(parent);
    }
    //debugPrintWindowPointers();
}

QTuio::~QTuio()
{
    if( running_ ) {
        tuioClient_->disconnect();
        delete tuioClient_;
        delete qTouchPointMap_;
        running_ = false;
        ////wait();
    }
}

void QTuio::setTuioUdpPort( int port )
{
    tuioUdpPort_ = port;
}

void QTuio::run()
{
    running_ = true;
    screenRect_ = QApplication::desktop()->screenGeometry();
    screenArea_ = screenRect_.width() * screenRect_.height();
    //printScreenRect();
    qTouchPointMap_ = new QMap<int, QTouchEvent::TouchPoint>();

    tuioClient_ = new TUIO::TuioClient( tuioUdpPort_ );
    tuioClient_->addTuioListener( this );
    tuioClient_->connect();
}

/**
 * If the /tuio/2Dblb TUIO message profile is seen, it may be desirable
 * to allow filter out any /tuio/2Dcur messages.  2Dcur is a subset of
 * 2Dblb, so no additional information is present.
 */
void QTuio::allowTuioProfileFiltering( bool b )
{
    if( tuioClient_ != NULL ) {
        tuioClient_->allowTuioProfileFiltering( b );
    }
}

void QTuio::addTuioCursor( TUIO::TuioCursor * tcur )
{
    QTuio::tuioToQt( tcur, QEvent::TouchBegin );
}

void QTuio::updateTuioCursor( TUIO::TuioCursor * tcur )
{
    QTuio::tuioToQt( tcur, QEvent::TouchUpdate );
}

void QTuio::removeTuioCursor( TUIO::TuioCursor * tcur )
{
    QTuio::tuioToQt( tcur, QEvent::TouchEnd );
}

bool QTuio::tuioToQt( TUIO::TuioCursor * tcur, QEvent::Type eventType )
{
    int id = tcur->getSessionID();
    //std::cout << "id = " << id << "\n";
    double speed = tcur->getMotionSpeed();
    const QPointF normPos( tcur->getX(), tcur->getY() );
    const QPointF screenPos( screenRect_.width() * normPos.x(), 
                             screenRect_.height() * normPos.y() );

    QTouchEvent::TouchPoint touchPoint( id );
    touchPoint.setNormalizedPos( normPos );
    touchPoint.setRect( QRectF() );
    touchPoint.setPressure( 1.0 );
    touchPoint.setScreenRect( screenRect_ );
    touchPoint.setScreenPos( screenPos );
    setScenePosAndRect( touchPoint, screenPos );

    Qt::TouchPointStates states;
    states = addTouchPoint( touchPoint, eventType, normPos, screenPos, id, speed );
    sendTouchEvent( eventType, states, id );
    return true;
}

void QTuio::setScenePosAndRect( QTouchEvent::TouchPoint & touchPoint, 
                                const QPointF & screenPos )
{
    if( theScene_ != NULL ) {
        setPositionBasedOnTheScene( touchPoint, screenPos );
    } 
    else if( theCentralWidget_ != NULL ) {
        setPositionBasedOnTheCentralWidget( touchPoint, screenPos );
    }
}

void QTuio::setPositionBasedOnTheScene( QTouchEvent::TouchPoint & touchPoint, 
                                        const QPointF & screenPos )
{
    touchPoint.setSceneRect( theScene_->sceneRect() );

    if (theView_) {
        double x = screenPos.x() - theView_->geometry().x(),
               y = screenPos.y() - theView_->geometry().y();
        touchPoint.setPos( QPointF( x, y ) );
        touchPoint.setScenePos( theView_->mapToScene( (int)x, (int)y ) );
    } 
    else {
        foreach( QGraphicsView * view, theScene_->views() ) {
            if( view->isActiveWindow() ) {
                double x = screenPos.x() - view->geometry().x(),
                       y = screenPos.y() - view->geometry().y();
                touchPoint.setPos( QPointF( x, y ) );
                touchPoint.setScenePos( view->mapToScene( (int)x, (int)y ) );
            }
        }
    }
}

void QTuio::setPositionBasedOnTheCentralWidget( QTouchEvent::TouchPoint & touchPoint, 
                                                const QPointF & screenPos )
{
    const QRect centralWidgetRect = theCentralWidget_->geometry();
    const QPointF pointPos( screenPos.x() - centralWidgetRect.x(),
                            screenPos.y() - centralWidgetRect.y() );
    //debugPrintPositions( screenPos, centralWidgetRect, pointPos );

    touchPoint.setPos( pointPos );
    touchPoint.setSceneRect( QRectF() );
    touchPoint.setScenePos( pointPos );
}

Qt::TouchPointStates QTuio::addTouchPoint( QTouchEvent::TouchPoint & touchPoint,
                                           QEvent::Type eventType, 
                                           const QPointF & normPos, 
                                           const QPointF & screenPos, 
                                           int id, 
                                           double speed )
{
    Qt::TouchPointStates touchPointStates;

    switch( eventType ) {
        case QEvent::TouchBegin:
            touchPointStates = Qt::TouchPointPressed;
            touchPoint.setState( Qt::TouchPointPressed );
            addNewTouchPoint( touchPoint, normPos, screenPos, id );
            break;
        case QEvent::TouchUpdate:
            if( speed > 0 ) {
                touchPointStates = Qt::TouchPointMoved;
                touchPoint.setState( Qt::TouchPointMoved );
            }
            else {
                touchPointStates = Qt::TouchPointStationary;
                touchPoint.setState( Qt::TouchPointStationary );
            }
            addExistingTouchPoint( touchPoint, id );
            break;
        case QEvent::TouchEnd:
            touchPointStates = Qt::TouchPointReleased;
            touchPoint.setState( Qt::TouchPointReleased );
            addExistingTouchPoint( touchPoint, id );
            break;
        default:  break;
    }
    return touchPointStates;
}

void QTuio::addNewTouchPoint( QTouchEvent::TouchPoint & touchPoint, 
                              const QPointF & normPos, 
                              const QPointF & screenPos, 
                              int id )
{
    touchPoint.setStartNormalizedPos( normPos );
    touchPoint.setStartPos( touchPoint.pos() );
    touchPoint.setStartScreenPos( screenPos );
    touchPoint.setStartScenePos( touchPoint.scenePos() );
    touchPoint.setLastNormalizedPos( normPos );
    touchPoint.setLastPos( touchPoint.pos() );
    touchPoint.setLastScreenPos( screenPos );
    touchPoint.setLastScenePos( touchPoint.scenePos() );
    qTouchPointMap_->insert( id, touchPoint );
}

void QTuio::addExistingTouchPoint( QTouchEvent::TouchPoint & touchPoint, int id )
{
    QTouchEvent::TouchPoint previousPoint = qTouchPointMap_->value( id );
    touchPoint.setStartNormalizedPos( previousPoint.startNormalizedPos() );
    touchPoint.setStartPos( previousPoint.startPos() );
    touchPoint.setStartScreenPos( previousPoint.startScreenPos() );
    touchPoint.setStartScenePos( previousPoint.startScenePos() );
    touchPoint.setLastNormalizedPos( previousPoint.normalizedPos() );
    touchPoint.setLastPos( previousPoint.pos() );
    touchPoint.setLastScreenPos( previousPoint.screenPos() );
    touchPoint.setLastScenePos( previousPoint.scenePos() );
    qTouchPointMap_->insert( id, touchPoint );
}

void QTuio::sendTouchEvent( QEvent::Type eventType, Qt::TouchPointStates touchPointStates, int id )
{
    // The CentraWidget constructor should have the flag for accepting Windows
    // 8 touch events set to false (by default).  Otherwise, the CentralWidget  
    // would respond directly to Windows 8 touch events in addition to Windows 
    // 8 touch events that are converted to TUIO by a program such as 
    // TouchHooks2Tuio.  However, when that flag is shut off, the Qt framework 
    // appears to always filter out TouchBegin events (but not TouchUpdate or
    // TouchEnd).  Therefore, it is necessary to convert any TouchBegin events 
    // to TouchUpdate.  Note, however, that the list of touch points held by 
    // the QTouchEvent will still contain any touch points that are labeled 
    // as a QEvent::TouchBegin, so the CentralWidget will still process them 
    // correctly.
    if( eventType == QEvent::TouchBegin ) {
        eventType = QEvent::TouchUpdate;
    }
    QEvent * touchEvent = new QTouchEvent( eventType, 
                                           0, 
                                           Qt::NoModifier, 
                                           touchPointStates, 
                                           qTouchPointMap_->values() );

    if( theView_ && theView_->viewport() ) {
        qApp->postEvent( theView_->viewport(), touchEvent );
    }
    else if( theScene_ ) {
        qApp->postEvent( theScene_, touchEvent );
    }
    else {
        qApp->postEvent( theMainWindow_->centralWidget(), touchEvent );
    }
    if( eventType == QEvent::TouchEnd ) {
        qTouchPointMap_->remove( id );
    }
}

void QTuio::addTuioBlob( TUIO::TuioBlob * tblb )
{
    QTuio::tuioToQt( tblb, QEvent::TouchBegin );
}

void QTuio::updateTuioBlob( TUIO::TuioBlob * tblb )
{
    QTuio::tuioToQt( tblb, QEvent::TouchUpdate );
}

void QTuio::removeTuioBlob( TUIO::TuioBlob * tblb )
{
    QTuio::tuioToQt( tblb, QEvent::TouchEnd );
}

bool QTuio::tuioToQt( TUIO::TuioBlob * tblb, QEvent::Type eventType )
{
    int id = tblb->getSessionID();
    double speed = tblb->getMotionSpeed(),
           relativeArea = tblb->getArea(),
           width = tblb->getWidth() * screenRect_.width(),
           height = tblb->getHeight() * screenRect_.height();

    const QPointF normPos( tblb->getX(), tblb->getY() );
    const QPointF screenPos( screenRect_.width() * normPos.x(), 
                             screenRect_.height() * normPos.y() );
    const QRectF boundingBox( screenPos.x() - (width / 2.0),
                              screenPos.y() - (height / 2.0), 
                              width, 
                              height );

    QTouchEvent::TouchPoint touchPoint( id );
    touchPoint.setNormalizedPos( normPos );
    touchPoint.setRect( boundingBox );

    // "Area" sent by CCV code is a relative area: camera blob area divided by 
    // the camera image area, which will be a number between 0.0 and 1.0.  The 
    // value will typically be a lot closer to the 0.0 end of the scale, as blob 
    // area is only finger tip size.  The relative area (between 0.0 and 1.0) 
    // is used below as pressure.  It could also be converted back to blob area 
    // in pixels by multiplying by the screen area, in which case the relative 
    // values (as finger contact area changes) would be on a scale much larger 
    // than 1.0 as a max.
    touchPoint.setPressure( relativeArea );
    //touchPoint.setPressure( relativeArea * screenArea_ );
    touchPoint.setScreenRect( screenRect_ );
    touchPoint.setScreenPos( screenPos );

    setScenePosAndRect( touchPoint, screenPos );

    Qt::TouchPointStates states;
    states = addTouchPoint( touchPoint, eventType, normPos, screenPos, id, speed );
    sendTouchEvent( eventType, states, id );
    return true;
}

void QTuio::addTuioObject( TUIO::TuioObject * tobj ) 
{
    QTuio::tuioToQt( tobj, QEvent::TouchBegin );
}

void QTuio::updateTuioObject( TUIO::TuioObject * tobj ) 
{
     QTuio::tuioToQt( tobj, QEvent::TouchUpdate );
}

void QTuio::removeTuioObject( TUIO::TuioObject * tobj ) 
{
    QTuio::tuioToQt( tobj, QEvent::TouchEnd );
}

bool QTuio::tuioToQt( TUIO::TuioObject * tobj, QEvent::Type eventType )
{
    // NOT SUPPORTED YET.

    return true;
}

void QTuio::refresh( TUIO::TuioTime frameTime ) 
{
}

void QTuio::printScreenRect()
{
    int x = screenRect_.x(),
        y = screenRect_.y(),
        width = screenRect_.width(),
        height = screenRect_.height();

    std::cout << "screenRect_( x, y, width, height ) = ( " 
              << x << ", " << y << ", " << width << ", " << height << " )\n";
}

void QTuio::debugPrintWindowPointers()
{
    std::cout << "\nQTuio::debugPrintWindowPointers() called.\n"
              << "    theScene_ = " << theScene_ << "\n"
              << "    theView_ = " << theView_ << "\n"
              << "    theMainWindow_ = " << theMainWindow_ << "\n"
              << "    theCentralWidget_ = " << theCentralWidget_ << "\n\n";
}

void QTuio::debugPrintPositions( const QPointF & screenPos,
                                 const QRect & centralWidgetRect,
                                 const QPointF & pointPos )
{
    std::cout << std::fixed << std::setprecision( 1 )
              << "\nscreenPos(x, y)   = (" 
              << screenPos.x() << ", " << screenPos.y() << ")\n"
              << "centralWidget(x, y) = (" 
              << centralWidgetRect.x() << ", " << centralWidgetRect.y() << ")\n"
              << "pointPos(x, y)      = (" 
              << pointPos.x() << ", " << pointPos.y() << ")\n";
}
