/*******************************************************************************
GestureDetector

PURPOSE: Detects horizontal and vertical line gestures, and whether a horizontal
         and vertical line form a plus sign.

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
#include "gestures/GestureDetector.h"
#include "gestures/LineGesture.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <iomanip>

typedef std::list<gestures::LineGesture *>::iterator GestureIter;

using gestures::GestureDetector;


GestureDetector::GestureDetector() :
  inProgressGestures_(),
  completedGestures_()
{
}

GestureDetector::~GestureDetector()
{
}

void GestureDetector::clear()
{
    clearInProgressGestures();
    clearCompletedGestures();
}

void GestureDetector::clearInProgressGestures()
{
    GestureIter start = inProgressGestures_.begin(),
                end = inProgressGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        delete (*iter);
    }
    inProgressGestures_.clear();
}

void GestureDetector::clearCompletedGestures()
{
    GestureIter start = completedGestures_.begin(),
                end = completedGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        delete (*iter);
    }
    completedGestures_.clear();
}

/*******************************************************************************
Adds a new gesture to the list and returns true if the id argument is not 
already in the list.  Otherwise, returns false and does not add a new gesture.
If the touch detection system were perfect, the same id number should not be
reused.  However, I have seen circumstances where the same id is seen more 
than once for a touch begin.
*******************************************************************************/
bool GestureDetector::startGesture( int id, double x, double y )
{
    if( !isIdFoundInGestureList( id ) ) {
        gestures::LineGesture * gesture = new gestures::LineGesture( id, x, y );
        inProgressGestures_.push_back( gesture );
        //std::cout << "id = " << id << " was added to the gesture list.\n";
        return true;
    }
    //std::cout << "id = " << id << " is already in the gesture list, so it was not added.\n";
    return false;
}

bool GestureDetector::isIdFoundInGestureList( int id )
{
    GestureIter start = inProgressGestures_.begin(),
                end = inProgressGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        gestures::LineGesture * gesture = *iter;

        if( gesture->id() == id ) {
            return true;
        }
    }
    return false;
}

void GestureDetector::updateGesture( int id, double x, double y )
{
    GestureIter start = inProgressGestures_.begin(),
                end = inProgressGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        gestures::LineGesture * gesture = *iter;

        if( gesture->id() == id ) {
            gesture->update( x, y );
            break;
        }
    }
}

bool GestureDetector::finishGestureAndTestForPlusSign( int id, double x, double y )
{
    bool isPlusSign = false;
    clearExpiredCompletedGestures();
    GestureIter start = inProgressGestures_.begin(),
                end = inProgressGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        gestures::LineGesture * gesture = *iter;

        if( gesture->id() == id ) {
            gesture->finish( x, y );
            
            if( gesture->isValidLine() ) {
                isPlusSign = searchForPlusSign( gesture );

                if( isPlusSign ) {
                    clear();
                    break;
                }
                completedGestures_.push_back( gesture );
            }
            else { // Not a horizontal or vertical line.
                delete gesture;
            }
            inProgressGestures_.erase( iter );
            break;
        }
    }
    return isPlusSign;
}

gestures::GestureResult GestureDetector::finishGestureAndCalculateResult( int id, double x, double y )
{
    gestures::GestureResult gestureResult;
    clearExpiredCompletedGestures();
    GestureIter start = inProgressGestures_.begin(),
                end = inProgressGestures_.end();

    for( GestureIter iter = start; iter != end; ++iter ) {
        gestures::LineGesture * gesture = *iter;

        if( gesture->id() == id ) {
            gesture->finish( x, y );
            gestureResult = searchCompletedGestures( gesture );

            if( gestureResult.isPlusSign() ) {
                clear();
                break;
            }
            else if( gestureResult.isHorizontalOrVerticalLine() ) {
                completedGestures_.push_back( gesture );
            }
            else { // Not a valid line.
                delete gesture;
            }
            inProgressGestures_.erase( iter );
            break;
        }
    }
    return gestureResult;
}

void GestureDetector::clearExpiredCompletedGestures()
{
    GestureIter iter = completedGestures_.begin(),
                end = completedGestures_.end();

    while( iter != end ) {
        gestures::LineGesture * gesture = *iter;

        if( gesture->isExpired() ) {
            GestureIter temp = iter;
            ++iter;
            completedGestures_.erase( temp );
            delete gesture;
        }
        else {
            ++iter;
        }
    }
}

gestures::GestureResult GestureDetector::searchCompletedGestures( gestures::LineGesture * gesture )
{
    gestures::GestureResult gestureResult = gesture->calculateResult( NULL );

    if( gesture->isValidLine() && completedGestures_.size() > 0 ) {
        GestureIter start = completedGestures_.begin(),
                    end = completedGestures_.end();

        for( GestureIter iter = start; iter != end; ++iter ) {
            gestures::LineGesture * completedGesture = *iter;
            gestureResult = gesture->calculateResult( completedGesture );

            if( gestureResult.isPlusSign() ) { 
                break; 
            }
        }
    }
    return gestureResult;
}

bool GestureDetector::searchForPlusSign( gestures::LineGesture * gesture )
{
    bool isPlusSign = false;

    if( gesture->isValidLine() && completedGestures_.size() > 0 ) {
        GestureIter start = completedGestures_.begin(),
                    end = completedGestures_.end();

        for( GestureIter iter = start; iter != end; ++iter ) {
            gestures::LineGesture * completedGesture = *iter;
            isPlusSign = gesture->isPlusSign( completedGesture );

            if( isPlusSign ) { 
                break; 
            }
        }
    }
    return isPlusSign;
}
