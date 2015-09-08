/*
 TUIO C++ Library - part of the reacTIVision project
 http://reactivision.sourceforge.net/
 
 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "TuioClient.h"

using namespace TUIO;
using namespace osc;

#ifndef WIN32
static void * ClientThreadFunc( void * obj )
#else
static DWORD WINAPI ClientThreadFunc( LPVOID obj )
#endif
{
    static_cast<TuioClient *>(obj)->socket->Run();
    return 0;
};

void TuioClient::lockObjectList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_lock( &objectMutex_ );
#else
    WaitForSingleObject( objectMutex_, INFINITE );
#endif		
}

void TuioClient::unlockObjectList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_unlock( &objectMutex_ );
#else
    ReleaseMutex( objectMutex_ );
#endif
}

void TuioClient::lockCursorList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_lock( &cursorMutex_ );
#else
    WaitForSingleObject( cursorMutex_, INFINITE );
#endif		
}

void TuioClient::unlockCursorList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_unlock( &cursorMutex_ );
#else
    ReleaseMutex( cursorMutex_ );
#endif		
}

void TuioClient::lockBlobList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_lock( &blobMutex_ );
#else
    WaitForSingleObject( blobMutex_, INFINITE );
#endif		
}

void TuioClient::unlockBlobList() 
{
    if( !connected_ ) return;
#ifndef WIN32	
    pthread_mutex_unlock( &blobMutex_ );
#else
    ReleaseMutex( blobMutex_ );
#endif
}

TuioClient::TuioClient( int port ) : 
  socket( NULL ),
  currentFrame_( -1 ), 
  maxCursorID_( -1 ), 
  maxBlobID_( -1 ),
  thread_( NULL ), 
  locked_( false ), 
  connected_( false ),
  tuio2DblbProfileHasBeenSeen_( false ),
  allowProfileFiltering_( false )
{
    try {
        socket = new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS, port ), this );
    } 
    catch( std::exception & e ) { 
        std::cerr << "could not bind to UDP port " << port << std::endl;
        socket = NULL;
    }
    if( socket != NULL ) {
        if( !socket->IsBound() ) {
            delete socket;
            socket = NULL;
        } 
        else {
            std::cout << "listening to TUIO messages on UDP port " << port << std::endl;
        }
    }	
}

TuioClient::~TuioClient() 
{	
    delete socket;
}

void TuioClient::ProcessBundle( const ReceivedBundle & b, const IpEndpointName & remoteEndpoint ) 
{
    try {
        for( ReceivedBundle::const_iterator i = b.ElementsBegin(); i != b.ElementsEnd(); ++i ) {
            if( i->IsBundle() ) {
                ProcessBundle( ReceivedBundle(*i), remoteEndpoint );
            }
            else {
                ProcessMessage( ReceivedMessage(*i), remoteEndpoint );
            }
        }
    } 
    catch( MalformedBundleException & e ) {
        std::cerr << "malformed OSC bundle: " << e.what() << std::endl;
    }
}

void TuioClient::allowTuioProfileFiltering( bool b )
{
    allowProfileFiltering_ = b;
}

bool TuioClient::isTuio2DcurProfileFilterOn()
{
    return (allowProfileFiltering_ && tuio2DblbProfileHasBeenSeen_);
}

void TuioClient::ProcessMessage( const ReceivedMessage & msg, const IpEndpointName & remoteEndpoint ) 
{
    try {
        ReceivedMessageArgumentStream args = msg.ArgumentStream();
        
        if( hasTuio2DblbAddress( msg ) ) {
            processTuio2DblbProfile( args );
            tuio2DblbProfileHasBeenSeen_ = true;
        }
        else if( hasTuio2DcurAddress( msg ) ) {
            if( !isTuio2DcurProfileFilterOn() ) {
                processTuio2DcurProfile( args );
            }
        }
        else if( hasTuio2DobjAddress( msg ) ) {
            processTuio2DobjProfile( args );
        } 
    } 
    catch( Exception & e ) {
        std::cerr << "error parsing TUIO message: " << msg.AddressPattern() 
                  <<  " - " << e.what() << std::endl;
    }
}

bool TuioClient::hasTuio2DblbAddress( const ReceivedMessage & msg )
{
    return (strcmp( msg.AddressPattern(), "/tuio/2Dblb" ) == 0);
}

void TuioClient::processTuio2DblbProfile( ReceivedMessageArgumentStream & args )
{
    const char * cmd;
    args >> cmd;

    if( strcmp( cmd, "set" ) == 0 )        { processTuio2DblbSetMessage( args ); }
    else if( strcmp( cmd, "alive" ) == 0 ) { processTuio2DblbAliveMessage( args ); }
    else if( strcmp( cmd, "fseq" ) == 0 )  { processTuio2DblbFseqMessage( args ); }
}

void TuioClient::processTuio2DblbSetMessage( ReceivedMessageArgumentStream & args )
{
    // One letter codes are from http://tuio.org/?specification.
    int32 s_sessionID;
    float x_position, y_position, a_angle,
          w_widthBoundingBox,  h_heightBoundingBox, f_area,
          X_speed, Y_speed, A_rotationalSpeed,
          m_motionAccel, r_rotationalAccel;

    args >> s_sessionID 
         >> x_position >> y_position >> a_angle 
         >> w_widthBoundingBox >> h_heightBoundingBox >> f_area 
         >> X_speed >> Y_speed >> A_rotationalSpeed 
         >> m_motionAccel >> r_rotationalAccel;

    lockBlobList();
    std::list<TuioBlob *>::iterator tblb;

    // Search blob list for session ID.
    for( tblb = blobList_.begin(); tblb != blobList_.end(); ++tblb ) {
        if( (*tblb)->getSessionID() == (long)s_sessionID ) { break; }
    }
    if( tblb == blobList_.end() ) { 
        // The TuioBlob was not in list, so create a new TuioBlob.
        int32 blobID = -1;
        TuioBlob * addBlob = 
            new TuioBlob( (long)s_sessionID, blobID, 
                          x_position, y_position, a_angle,
                          w_widthBoundingBox, h_heightBoundingBox, f_area );
        frameBlobs_.push_back( addBlob );
    }
    else if( ((*tblb)->getX() != x_position)
          || ((*tblb)->getY() != y_position)
          || ((*tblb)->getAngle() != a_angle)
          || ((*tblb)->getWidth() != w_widthBoundingBox)
          || ((*tblb)->getHeight() != h_heightBoundingBox)
          || ((*tblb)->getArea() != f_area)
          || ((*tblb)->getXSpeed() != X_speed)
          || ((*tblb)->getYSpeed() != Y_speed)
          || ((*tblb)->getRotationSpeed() != A_rotationalSpeed)
          || ((*tblb)->getMotionAccel() != m_motionAccel)
          || ((*tblb)->getRotationAccel() != r_rotationalAccel) ) {
        // TuioBlob exists in list, but some info has changed, so create update.
        TuioBlob * updateBlob = 
            new TuioBlob( (long)s_sessionID, (*tblb)->getBlobID(), 
                          x_position, y_position, a_angle,
                          w_widthBoundingBox, h_heightBoundingBox, f_area );
        updateBlob->update( x_position, y_position, a_angle,
                            w_widthBoundingBox, h_heightBoundingBox, f_area,
                            X_speed, Y_speed, A_rotationalSpeed,
                            m_motionAccel, r_rotationalAccel );
        frameBlobs_.push_back( updateBlob );
    }
    unlockBlobList();
}

void TuioClient::processTuio2DblbAliveMessage( ReceivedMessageArgumentStream & args )
{
    int32 s_sessionID;
    aliveBlobList_.clear();

    while( !args.Eos() ) {
        args >> s_sessionID;
        aliveBlobList_.push_back( (long)s_sessionID );
    }
}

void TuioClient::processTuio2DblbFseqMessage( ReceivedMessageArgumentStream & args )
{
    int32 fseq;
    args >> fseq;
    bool lateFrame = false;

    if( fseq > 0 ) {
        if( fseq > currentFrame_ ) { currentTime_ = TuioTime::getSessionTime(); }
        if( (fseq >= currentFrame_) || ((currentFrame_ - fseq) > 100) ) { currentFrame_ = fseq; }
        else { lateFrame = true; }
    }  
    else if( (TuioTime::getSessionTime().getTotalMilliseconds() - currentTime_.getTotalMilliseconds()) > 100 ) {
        currentTime_ = TuioTime::getSessionTime();
    }
    if( !lateFrame ) {
        lockBlobList();

        // Find the removed blobs first.
        for( std::list<TuioBlob *>::iterator tblb = blobList_.begin(); tblb != blobList_.end(); ++tblb ) {
            std::list<long>::iterator iter = find( aliveBlobList_.begin(), aliveBlobList_.end(), (*tblb)->getSessionID() );

            if( iter == aliveBlobList_.end() ) {
                (*tblb)->remove( currentTime_ );
                frameBlobs_.push_back( *tblb );
            }
        }
        unlockBlobList();

        for( std::list<TuioBlob *>::iterator iter = frameBlobs_.begin(); iter != frameBlobs_.end(); ++iter ) {
            TuioBlob * tblb = (*iter);
            int blobID = -1;
            TuioBlob * frameBlob = NULL;

            switch( tblb->getTuioState() ) {
                case TUIO_REMOVED:
                    frameBlob = tblb;
                    frameBlob->remove( currentTime_ );

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->removeTuioBlob( frameBlob );
                    }
                    lockBlobList();

                    for( std::list<TuioBlob *>::iterator delblb = blobList_.begin(); delblb != blobList_.end(); ++delblb ) {
                        if( (*delblb)->getSessionID() == frameBlob->getSessionID() ) {
                            blobList_.erase( delblb );
                            break;
                        }
                    }
                    if( frameBlob->getBlobID() == maxBlobID_ ) {
                        maxBlobID_ = -1;
                        delete frameBlob;

                        if( blobList_.size() > 0 ) {
                            std::list<TuioBlob *>::iterator blist;

                            for( blist = blobList_.begin(); blist != blobList_.end(); ++blist ) {
                                blobID = (*blist)->getBlobID();
                                if( blobID > maxBlobID_ ) { maxBlobID_ = blobID; }
                            }
                            freeBlobBuffer_.clear();

                            for( std::list<TuioBlob *>::iterator flist = freeBlobList_.begin(); flist != freeBlobList_.end(); ++flist ) {
                                TuioBlob * freeBlob = (*flist);
                                if( freeBlob->getBlobID() > maxBlobID_ ) { delete freeBlob; }
                                else { freeBlobBuffer_.push_back( freeBlob ); }
                            }	
                            freeBlobList_ = freeBlobBuffer_;
                        } 
                        else {
                            for( std::list<TuioBlob *>::iterator flist=freeBlobList_.begin(); flist != freeBlobList_.end(); ++flist ) {
                                TuioBlob * freeBlob = (*flist);
                                delete freeBlob;
                            }
                            freeBlobList_.clear();
                        }
                    } 
                    else if( frameBlob->getBlobID() < maxBlobID_ ) {
                        freeBlobList_.push_back( frameBlob );
                    } 
                    unlockBlobList();
                    break;

                case TUIO_ADDED:
                    lockBlobList();
                    blobID = (int)blobList_.size();

                    if( ((int)(blobList_.size()) <= maxBlobID_) && ((int)(freeBlobList_.size()) > 0) ) {
                        std::list<TuioBlob*>::iterator closestBlob = freeBlobList_.begin();

                        for( std::list<TuioBlob *>::iterator iter = freeBlobList_.begin(); iter != freeBlobList_.end(); ++iter ) {
                            if( (*iter)->getDistance( tblb ) < (*closestBlob)->getDistance( tblb ) )  { closestBlob = iter; }
                        }
                        TuioBlob * freeBlob = (*closestBlob);
                        blobID = freeBlob->getBlobID();
                        freeBlobList_.erase( closestBlob );
                        delete freeBlob;
                    } 
                    else { 
                        maxBlobID_ = blobID;
                    }							
                    frameBlob = new TuioBlob( currentTime_, tblb->getSessionID(), blobID, 
                        tblb->getX(), tblb->getY(), tblb->getAngle(),
                        tblb->getWidth(), tblb->getHeight(), tblb->getArea() );
                    blobList_.push_back( frameBlob );
                    delete tblb;
                    unlockBlobList();

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->addTuioBlob( frameBlob );
                    }
                    break;

                default:
                    lockBlobList();
                    std::list<TuioBlob *>::iterator iter;

                    for( iter = blobList_.begin(); iter != blobList_.end(); ++iter ) {
                        if( (*iter)->getSessionID() == tblb->getSessionID() ) {
                            frameBlob = (*iter);
                            break;
                        }
                    }
                    if( (tblb->getX() != frameBlob->getX() && tblb->getXSpeed() == 0) 
                        || (tblb->getY() != frameBlob->getY() && tblb->getYSpeed() == 0) ) {

                            frameBlob->update( currentTime_,
                                tblb->getX(), tblb->getY(), tblb->getAngle(),
                                tblb->getWidth(), tblb->getHeight(), tblb->getArea() );
                    }
                    else {
                        frameBlob->update( currentTime_, 
                            tblb->getX(), 
                            tblb->getY(),
                            tblb->getAngle(),
                            tblb->getWidth(),
                            tblb->getHeight(),
                            tblb->getArea(),
                            tblb->getXSpeed(), 
                            tblb->getYSpeed(), 
                            tblb->getRotationSpeed(),
                            tblb->getMotionAccel(),
                            tblb->getRotationAccel() );
                    }
                    delete tblb;
                    unlockBlobList();

                    for( std::list<TuioListener*>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->updateTuioBlob( frameBlob );
                    }
            }	
        }
        for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
            (*listener)->refresh(currentTime_);
        }
    } 
    else { // Must be late frame.
        for( std::list<TuioBlob *>::iterator iter = frameBlobs_.begin(); iter != frameBlobs_.end(); ++iter ) {
            TuioBlob * tblb = (*iter);
            delete tblb;
        }
    }
    frameBlobs_.clear();
}

bool TuioClient::hasTuio2DcurAddress( const ReceivedMessage & msg )
{
    return (strcmp( msg.AddressPattern(), "/tuio/2Dcur" ) == 0);
}

void TuioClient::processTuio2DcurProfile( ReceivedMessageArgumentStream & args )
{
    const char * cmd;
    args >> cmd;

    if( strcmp( cmd, "set" ) == 0 )        { processTuio2DcurSetMessage( args ); } 
    else if( strcmp( cmd, "alive" ) == 0 ) { processTuio2DcurAliveMessage( args ); } 
    else if( strcmp( cmd, "fseq" ) == 0 )  { processTuio2DcurFseqMessage( args ); }
}

void TuioClient::processTuio2DcurSetMessage( ReceivedMessageArgumentStream & args )
{
    int32 s_id;
    float xpos, ypos, xspeed, yspeed, maccel;				
    args >> s_id >> xpos >> ypos >> xspeed >> yspeed >> maccel;

    lockCursorList();
    std::list<TuioCursor *>::iterator tcur;

    for( tcur = cursorList_.begin(); tcur != cursorList_.end(); ++tcur ) {
        if( (*tcur)->getSessionID() == (long)s_id ) { 
            break; 
        }
    }
    if( tcur == cursorList_.end() ) {
        TuioCursor * addCursor = new TuioCursor( (long)s_id, -1, xpos, ypos );
        frameCursors_.push_back( addCursor );
    }
    else if( ((*tcur)->getX() != xpos) 
          || ((*tcur)->getY() != ypos) 
          || ((*tcur)->getXSpeed() != xspeed) 
          || ((*tcur)->getYSpeed() != yspeed) 
          || ((*tcur)->getMotionAccel() != maccel) ) {
            TuioCursor * updateCursor = new TuioCursor( (long)s_id, (*tcur)->getCursorID(), xpos, ypos );
            updateCursor->update( xpos, ypos, xspeed, yspeed, maccel );
            frameCursors_.push_back( updateCursor );
    }
    unlockCursorList();
}

void TuioClient::processTuio2DcurAliveMessage( ReceivedMessageArgumentStream & args )
{
    int32 s_id;
    aliveCursorList_.clear();

    while( !args.Eos() ) {
        args >> s_id;
        aliveCursorList_.push_back( (long)s_id );
    }
}

void TuioClient::processTuio2DcurFseqMessage( ReceivedMessageArgumentStream & args )
{
    int32 fseq;
    args >> fseq;
    bool lateFrame = false;

    if( fseq > 0 ) {
        if( fseq > currentFrame_ ) { currentTime_ = TuioTime::getSessionTime(); }
        if( (fseq >= currentFrame_) || ((currentFrame_ - fseq) > 100) ) { currentFrame_ = fseq; }
        else { lateFrame = true; }
    }  
    else if( (TuioTime::getSessionTime().getTotalMilliseconds() - currentTime_.getTotalMilliseconds()) > 100 ) {
        currentTime_ = TuioTime::getSessionTime();
    }
    if( !lateFrame ) {
        lockCursorList();

        // find the removed cursors first
        for( std::list<TuioCursor *>::iterator tcur = cursorList_.begin(); tcur != cursorList_.end(); ++tcur ) {
            std::list<long>::iterator iter = find( aliveCursorList_.begin(), aliveCursorList_.end(), (*tcur)->getSessionID() );

            if( iter == aliveCursorList_.end() ) {
                (*tcur)->remove( currentTime_ );
                frameCursors_.push_back( *tcur );
            }
        }
        unlockCursorList();

        for( std::list<TuioCursor *>::iterator iter = frameCursors_.begin(); iter != frameCursors_.end(); ++iter ) {
            TuioCursor * tcur = (*iter);
            int c_id = -1;
            TuioCursor * frameCursor = NULL;

            switch( tcur->getTuioState() ) {
                case TUIO_REMOVED:
                    frameCursor = tcur;
                    frameCursor->remove( currentTime_ );

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->removeTuioCursor( frameCursor );
                    }
                    lockCursorList();

                    for( std::list<TuioCursor *>::iterator delcur = cursorList_.begin(); delcur != cursorList_.end(); ++delcur ) {
                        if( (*delcur)->getSessionID() == frameCursor->getSessionID() ) {
                            cursorList_.erase( delcur );
                            break;
                        }
                    }
                    if( frameCursor->getCursorID() == maxCursorID_ ) {
                        maxCursorID_ = -1;
                        delete frameCursor;

                        if( cursorList_.size() > 0 ) {
                            std::list<TuioCursor *>::iterator clist;

                            for( clist = cursorList_.begin(); clist != cursorList_.end(); ++clist ) {
                                c_id = (*clist)->getCursorID();
                                if( c_id > maxCursorID_ ) { maxCursorID_ = c_id; }
                            }
                            freeCursorBuffer_.clear();

                            for( std::list<TuioCursor*>::iterator flist = freeCursorList_.begin(); flist != freeCursorList_.end(); ++flist ) {
                                TuioCursor * freeCursor = (*flist);
                                if( freeCursor->getCursorID() > maxCursorID_ ) { delete freeCursor; }
                                else { freeCursorBuffer_.push_back( freeCursor ); }
                            }	
                            freeCursorList_ = freeCursorBuffer_;

                        } 
                        else {
                            for( std::list<TuioCursor*>::iterator flist=freeCursorList_.begin(); flist != freeCursorList_.end(); ++flist ) {
                                TuioCursor * freeCursor = (*flist);
                                delete freeCursor;
                            }
                            freeCursorList_.clear();
                        }
                    } 
                    else if( frameCursor->getCursorID() < maxCursorID_ ) {
                        freeCursorList_.push_back( frameCursor );
                    } 
                    unlockCursorList();
                    break;

                case TUIO_ADDED:
                    lockCursorList();
                    c_id = (int)cursorList_.size();

                    if( ((int)(cursorList_.size()) <= maxCursorID_) && ((int)(freeCursorList_.size()) > 0) ) {
                        std::list<TuioCursor*>::iterator closestCursor = freeCursorList_.begin();

                        for( std::list<TuioCursor *>::iterator iter = freeCursorList_.begin(); iter != freeCursorList_.end(); ++iter ) {
                            if( (*iter)->getDistance( tcur ) < (*closestCursor)->getDistance( tcur ) )  { closestCursor = iter; }
                        }
                        TuioCursor * freeCursor = (*closestCursor);
                        c_id = freeCursor->getCursorID();
                        freeCursorList_.erase( closestCursor );
                        delete freeCursor;
                    } 
                    else { 
                        maxCursorID_ = c_id;
                    }							
                    frameCursor = new TuioCursor( currentTime_, tcur->getSessionID(), c_id, tcur->getX(), tcur->getY() );
                    cursorList_.push_back( frameCursor );
                    delete tcur;
                    unlockCursorList();

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->addTuioCursor( frameCursor );
                    }
                    break;

                default:
                    lockCursorList();
                    std::list<TuioCursor *>::iterator iter;

                    for( iter = cursorList_.begin(); iter != cursorList_.end(); ++iter ) {
                        if( (*iter)->getSessionID() == tcur->getSessionID() ) {
                            frameCursor = (*iter);
                            break;
                        }
                    }
                    if ( (tcur->getX() != frameCursor->getX() && tcur->getXSpeed() == 0) 
                        || (tcur->getY() != frameCursor->getY() && tcur->getYSpeed() == 0) ) {
                            frameCursor->update( currentTime_, tcur->getX(), tcur->getY() );
                    }
                    else {
                        frameCursor->update( currentTime_, 
                            tcur->getX(), 
                            tcur->getY(),
                            tcur->getXSpeed(), 
                            tcur->getYSpeed(), 
                            tcur->getMotionAccel() );
                    }
                    delete tcur;
                    unlockCursorList();

                    for( std::list<TuioListener*>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->updateTuioCursor( frameCursor );
                    }
            }	
        }
        for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
            (*listener)->refresh(currentTime_);
        }
    } 
    else {
        for (std::list<TuioCursor *>::iterator iter = frameCursors_.begin(); iter != frameCursors_.end(); ++iter ) {
            TuioCursor * tcur = (*iter);
            delete tcur;
        }
    }
    frameCursors_.clear();
}

void TuioClient::ProcessPacket( const char *data, int size, const IpEndpointName& remoteEndpoint ) 
{
    try {
        ReceivedPacket p( data, size );
        if(p.IsBundle()) ProcessBundle( ReceivedBundle(p), remoteEndpoint);
        else ProcessMessage( ReceivedMessage(p), remoteEndpoint);
    } 
    catch (MalformedBundleException& e) {
        std::cerr << "malformed OSC bundle: " << e.what() << std::endl;
    }
}

bool TuioClient::hasTuio2DobjAddress( const ReceivedMessage & msg )
{
    return (strcmp( msg.AddressPattern(), "/tuio/2Dobj" ) == 0);
}

void TuioClient::processTuio2DobjProfile( ReceivedMessageArgumentStream & args )
{
    const char * cmd;
    args >> cmd;

    if( strcmp( cmd, "set" ) == 0 )        { processTuio2DobjSetMessage( args ); }
    else if( strcmp( cmd, "alive" ) == 0 ) { processTuio2DobjAliveMessage( args ); } 
    else if( strcmp( cmd, "fseq" ) == 0 )  { processTuio2DobjFseqMessage( args ); }
}

void TuioClient::processTuio2DobjSetMessage( osc::ReceivedMessageArgumentStream & args )
{
    int32 s_id, c_id;
    float xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel;
    args >> s_id >> c_id >> xpos >> ypos >> angle >> xspeed >> yspeed >> rspeed >> maccel >> raccel;

    lockObjectList();
    std::list<TuioObject *>::iterator tobj;

    for( tobj = objectList_.begin(); tobj != objectList_.end(); ++tobj ) {
        if( (*tobj)->getSessionID() == (long)s_id ) { break; }
    }
    if( tobj == objectList_.end() ) {
        TuioObject * addObject = new TuioObject( (long)s_id, (int)c_id, xpos, ypos, angle );
        frameObjects_.push_back( addObject );
    } 
    else if( ((*tobj)->getX() != xpos)
          || ((*tobj)->getY() != ypos) 
          || ((*tobj)->getAngle() != angle) 
          || ((*tobj)->getXSpeed() != xspeed) 
          || ((*tobj)->getYSpeed() != yspeed) 
          || ((*tobj)->getRotationSpeed() != rspeed) 
          || ((*tobj)->getMotionAccel() != maccel)
          || ((*tobj)->getRotationAccel() != raccel) ) {
            TuioObject * updateObject = new TuioObject( (long)s_id, (*tobj)->getSymbolID(), xpos, ypos, angle );
            updateObject->update( xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel );
            frameObjects_.push_back( updateObject );
    }
    unlockObjectList();
}

void TuioClient::processTuio2DobjAliveMessage( osc::ReceivedMessageArgumentStream & args )
{
    int32 s_id;
    aliveObjectList_.clear();

    while( !args.Eos() ) {
        args >> s_id;
        aliveObjectList_.push_back( (long)s_id );
    }
}

void TuioClient::processTuio2DobjFseqMessage( osc::ReceivedMessageArgumentStream & args )
{
    int32 fseq;
    args >> fseq;
    bool lateFrame = false;

    if( fseq > 0 ) {
        if( fseq>currentFrame_ ) { currentTime_ = TuioTime::getSessionTime(); }
        if( (fseq >= currentFrame_) || ((currentFrame_ - fseq) > 100) ) { currentFrame_ = fseq; }
        else { lateFrame = true; }
    } 
    else if( (TuioTime::getSessionTime().getTotalMilliseconds() - currentTime_.getTotalMilliseconds()) > 100 ) {
        currentTime_ = TuioTime::getSessionTime();
    }
    if( !lateFrame ) {
        lockObjectList();

        //find the removed objects first
        for( std::list<TuioObject *>::iterator tobj = objectList_.begin(); tobj != objectList_.end(); ++tobj ) {
            std::list<long>::iterator iter = find( aliveObjectList_.begin(), aliveObjectList_.end(), (*tobj)->getSessionID() );

            if( iter == aliveObjectList_.end() ) {
                (*tobj)->remove( currentTime_ );
                frameObjects_.push_back( *tobj );							
            }
        }
        unlockObjectList();

        for( std::list<TuioObject *>::iterator iter = frameObjects_.begin(); iter != frameObjects_.end(); ++iter ) {
            TuioObject * tobj = (*iter);
            TuioObject * frameObject = NULL;

            switch( tobj->getTuioState() ) {
                case TUIO_REMOVED:
                    frameObject = tobj;
                    frameObject->remove( currentTime_ );

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->removeTuioObject( frameObject );
                    }
                    lockObjectList();

                    for( std::list<TuioObject *>::iterator delobj = objectList_.begin(); delobj != objectList_.end(); ++delobj ) {
                        if( (*delobj)->getSessionID() == frameObject->getSessionID() ) {
                            objectList_.erase( delobj );
                            break;
                        }
                    }
                    unlockObjectList();
                    break;

                case TUIO_ADDED:
                    lockObjectList();
                    frameObject = new TuioObject( currentTime_, tobj->getSessionID(), tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getAngle() );
                    objectList_.push_back( frameObject );
                    unlockObjectList();

                    for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->addTuioObject( frameObject );
                    }
                    break;

                default:
                    lockObjectList();
                    std::list<TuioObject*>::iterator iter;

                    for( iter = objectList_.begin(); iter != objectList_.end(); ++iter ) {
                        if( (*iter)->getSessionID() == tobj->getSessionID() ) {
                            frameObject = (*iter);
                            break;
                        }
                    }
                    if( iter == objectList_.end() ) { break; }

                    if( (tobj->getX() != frameObject->getX() && tobj->getXSpeed() == 0)
                        || (tobj->getY() != frameObject->getY() && tobj->getYSpeed() == 0) ) {
                            frameObject->update( currentTime_, 
                                tobj->getX(), 
                                tobj->getY(),
                                tobj->getAngle() );
                    }
                    else {
                        frameObject->update( currentTime_,
                            tobj->getX(),
                            tobj->getY(), 
                            tobj->getAngle(), 
                            tobj->getXSpeed(), 
                            tobj->getYSpeed(), 
                            tobj->getRotationSpeed(), 
                            tobj->getMotionAccel(), 
                            tobj->getRotationAccel() );
                    }
                    unlockObjectList();

                    for( std::list<TuioListener*>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
                        (*listener)->updateTuioObject( frameObject );
                    }
            }
            delete tobj;
        }
        for( std::list<TuioListener *>::iterator listener = listenerList_.begin(); listener != listenerList_.end(); ++listener ) {
            (*listener)->refresh( currentTime_ );
        }
    }
    else {
        for( std::list<TuioObject *>::iterator iter = frameObjects_.begin(); iter != frameObjects_.end(); ++iter ) {
            TuioObject *tobj = (*iter);
            delete tobj;
        }
    }
    frameObjects_.clear();
}

void TuioClient::connect( bool lk ) 
{
#ifndef WIN32	
    /*pthread_mutexattr_settype( &attr_p, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &cursorMutex_, &attr_p );
    pthread_mutex_init( &objectMutex_, &attr_p );
    pthread_mutex_init( &blobMutex_, &attr_p );*/

    pthread_mutex_init( &cursorMutex_, NULL );
    pthread_mutex_init( &objectMutex_, NULL );	
    pthread_mutex_init( &blobMutex_, NULL );	
#else
    cursorMutex_ = CreateMutex( NULL, FALSE, L"cursorMutex_" );
    objectMutex_ = CreateMutex( NULL, FALSE, L"objectMutex_" );
    blobMutex_   = CreateMutex( NULL, FALSE, L"blobMutex_" );
#endif		
        
    if( socket == NULL ) { 
        return; 
    }
    TuioTime::initSession();
    currentTime_.reset();
    locked_ = lk;

    if( !locked_ ) {
#ifndef WIN32
        pthread_create( &thread_ , NULL, ClientThreadFunc, this );
#else
        DWORD threadId;
        thread_ = CreateThread( 0, 0, ClientThreadFunc, this, 0, &threadId );
#endif
    } 
    else {
        socket->Run(); 
    }
    connected_ = true;
    unlockCursorList();
    unlockObjectList();
    unlockBlobList();
}

void TuioClient::disconnect()
{
    if( socket == NULL ) { 
        return; 
    }
    socket->Break();
    
    if( !locked_ ) {
#ifdef WIN32
        if( thread_ ) { CloseHandle( thread_ ); }
#endif
        thread_ = 0;
        locked_ = false;
    }
    
#ifndef WIN32	
    pthread_mutex_destroy( &cursorMutex_ );
    pthread_mutex_destroy( &objectMutex_ );
    pthread_mutex_destroy( &blobMutex_ );
#else
    CloseHandle( cursorMutex_ );
    CloseHandle( objectMutex_ );
    CloseHandle( blobMutex_ );
#endif

    aliveObjectList_.clear();
    aliveCursorList_.clear();
    aliveBlobList_.clear();

    for( std::list<TuioObject *>::iterator iter = objectList_.begin(); iter != objectList_.end(); ++iter )
        delete (*iter);
    objectList_.clear();

    for( std::list<TuioCursor *>::iterator iter = cursorList_.begin(); iter != cursorList_.end(); ++iter )
        delete (*iter);
    cursorList_.clear();

    for( std::list<TuioBlob *>::iterator iter = blobList_.begin(); iter != blobList_.end(); ++iter )
        delete (*iter);
    blobList_.clear();
    
    for( std::list<TuioCursor*>::iterator iter = freeCursorList_.begin(); iter != freeCursorList_.end(); ++iter )
        delete(*iter);
    freeCursorList_.clear();

    connected_ = false;
}

void TuioClient::addTuioListener( TuioListener * listener ) 
{
    listenerList_.push_back(listener);
}

void TuioClient::removeTuioListener( TuioListener * listener ) 
{
    std::list<TuioListener*>::iterator result = find(listenerList_.begin(),listenerList_.end(),listener);
    if (result!=listenerList_.end()) listenerList_.remove(listener);
}

TuioObject * TuioClient::getTuioObject( long s_id )
{
    lockObjectList();

    for( std::list<TuioObject *>::iterator iter = objectList_.begin(); iter != objectList_.end(); ++iter ) {
        if( (*iter)->getSessionID() == s_id ) {
            unlockObjectList();
            return (*iter);
        }
    }	
    unlockObjectList();
    return NULL;
}

TuioCursor * TuioClient::getTuioCursor( long s_id ) 
{
    lockCursorList();

    for( std::list<TuioCursor *>::iterator iter = cursorList_.begin(); iter != cursorList_.end(); ++iter ) {
        if( (*iter)->getSessionID() == s_id ) {
            unlockCursorList();
            return (*iter);
        }
    }	
    unlockCursorList();
    return NULL;
}

TuioBlob * TuioClient::getTuioBlob( long s_id )
{
    lockBlobList();

    for( std::list<TuioBlob *>::iterator iter = blobList_.begin(); iter != blobList_.end(); ++iter ) {
        if( (*iter)->getSessionID() == s_id ) {
            unlockBlobList();
            return (*iter);
        }
    }	
    unlockBlobList();
    return NULL;
}

std::list<TuioObject *> TuioClient::getTuioObjects() 
{
    lockObjectList();
    std::list<TuioObject *> listBuffer = objectList_;
    unlockObjectList();
    return listBuffer;
}

std::list<TuioCursor *> TuioClient::getTuioCursors() 
{
    lockCursorList();
    std::list<TuioCursor *> listBuffer = cursorList_;
    unlockCursorList();
    return listBuffer;
}

std::list<TuioBlob *> TuioClient::getTuioBlobs()
{
    lockBlobList();
    std::list<TuioBlob *> listBuffer = blobList_;
    unlockBlobList();
    return listBuffer;
}
