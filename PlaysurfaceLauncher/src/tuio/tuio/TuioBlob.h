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
#ifndef INCLUDED_TUIOBLOB_H
#define INCLUDED_TUIOBLOB_H

#include <math.h>
#include "TuioContainer.h"

#define TUIO_ROTATING 5

namespace TUIO 
{
	/**
	 * The TuioBlob class encapsulates /tuio/2Dblb TUIO objects.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 1.4
	 *
	 * This class was created by duplicating Martin Kaltenbrunner's TuioObject.h
	 * file (for /tuio/2Dobj profile) and added 4 fields:
	 *
	 * blobID - replaced symbol_id.
	 * width  - bounding box width.
	 * height - bounding box height.
	 * area   - blob area (can be used for pressure).
	 *
	 * Modified on 9/9/2014 by JRW.
	 */ 
	class TuioBlob : public TuioContainer 
	{
	protected:
		int blobID_;
		float angle_;
		float width_;
		float height_;
		float area_;
		float rotationSpeed_;
		float rotationAccel_;
		
	public:
		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided 
		 * Session ID, Symbol ID, X and Y coordinate and angle to the newly created TuioBlob.
		 *
		 * @param	tuioTime     the TuioTime
		 * @param	s_sessionID  marker ID created on server side for tracking
		 * @param   blobID       marker ID created on client side for tracking
		 * @param	x_pos        the x-coordinate of the position
		 * @param	y_pos        the y-coordinate of the position
		 * @param	a_angle      the angle of the blob bounding box
		 * @param   w_width      the width of the bounding box
		 * @param   h_height     the height of the bounding box
		 * @param   f_area       the blob area
		 */
		TuioBlob( TuioTime tuioTime, long s_sessionID, int blobID, 
				  float x_pos, float y_pos, float a_angle, 
				  float w_width, float h_height, float f_area ) :
		  TuioContainer( tuioTime, s_sessionID, x_pos, y_pos ) ,
		  blobID_( blobID ),
		  angle_( a_angle ),
		  width_( w_width),
		  height_( h_height ),
		  area_( f_area ),
		  rotationSpeed_( 0.0f ),
		  rotationAccel_( 0.0f )
		{
		};

		/**
		 * This constructor takes the provided Session ID, Blob ID, X and Y coordinate 
		 * and angle, and assigns these values to the newly created TuioBlob.
		 *
		 * @param	s_sessionID  marker ID created on server side for tracking
		 * @param   blobID       marker ID created on client side for tracking
		 * @param	x_pos        the x-coordinate of the position
		 * @param	y_pos        the y-coordinate of the position
		 * @param	a_angle      the angle of the blob bounding box
		 * @param   w_width      the width of the bounding box
		 * @param   h_height     the height of the bounding box
		 * @param   f_area       the blob area
		 */	
		TuioBlob( long s_sessionID, int blobID, 
				  float x_pos, float y_pos, float a_angle,
				  float w_width, float h_height, float f_area ) :
		  TuioContainer( s_sessionID, x_pos, y_pos ),
		  blobID_( blobID ),
		  angle_( a_angle ),
		  width_( w_width ),
		  height_( h_height ),
		  area_( f_area ),
		  rotationSpeed_( 0.0f ),
		  rotationAccel_( 0.0f )
		{
		};
		
		/**
		 * This constructor takes the attributes of the provided TuioBlob 
		 * and assigns these values to the newly created TuioBlob.
		 *
		 * @param  tblb  the TuioBlob to assign
		 */
		TuioBlob( TuioBlob * tblb ) : 
		  TuioContainer( tblb ),
		  blobID_( tblb->blobID_ ),
		  angle_( tblb->angle_ ),
		  width_( tblb->width_ ),
		  height_( tblb->height_ ),
		  area_( tblb->area_ ),
		  rotationSpeed_( tblb->rotationSpeed_ ),
		  rotationAccel_( tblb->rotationAccel_ )
		{
		};
		
		/**
		 * The destructor is doing nothing in particular. 
		 */
		~TuioBlob() {};
		
		/**
		 * Takes a TuioTime argument and assigns it along with the provided 
		 * X and Y coordinate, angle, X and Y velocity, motion acceleration,
		 * rotation speed and rotation acceleration to the private TuioBlob attributes.
		 *
		 * @param	tuioTime  the TuioTime to assign
		 * @param	x_pos     the x-coordinate of the position
		 * @param	y_pos     the y-coordinate of the position
		 * @param	a_angle   the angle of the blob bounding box
		 * @param   w_width   the width of the bounding box
		 * @param   h_height  the height of the bounding box
		 * @param   f_area    the blob area
		 * @param   X_speed   velocity vector in the x direction
		 * @param   Y_speed   velocity vector in the y direction
		 * @param   A_rotationSpeed  rotational velocity vector
		 * @param   m_motionAccel    motion acceleration
		 * @param   r_rotationAccel  rotational acceleration
		 */
		void update( TuioTime tuioTime, 
					 float x_pos, float y_pos, float a_angle, 
					 float w_width, float h_height, float f_area, 
					 float X_speed, float Y_speed, float A_rotationSpeed, 
					 float m_motionAccel, float r_rotationAccel )
		{
			TuioContainer::update( tuioTime, x_pos, y_pos, X_speed, Y_speed, m_motionAccel );
			angle_ = a_angle;
			width_ = w_width;
			height_ = h_height;
			area_ = f_area;
			rotationSpeed_ = A_rotationSpeed;
			rotationAccel_ = r_rotationAccel;

			if( (rotationAccel_ != 0) && (state == TUIO_STOPPED) ) {
				state = TUIO_ROTATING;
			}
		};

		/**
		 * The one letter code at the beginning of each parameter 
		 * name is from http://tuio.org/?specification.
		 *
		 * The TuioTime time stamp remains unchanged.
		 *
		 * @param	x_pos     the x-coordinate of the position
		 * @param	y_pos     the y-coordinate of the position
		 * @param	a_angle   the angle of the blob bounding box
		 * @param   w_width   the width of the bounding box
		 * @param   h_height  the height of the bounding box
		 * @param   f_area    the blob area
		 * @param   X_speed   velocity vector in the x direction
		 * @param   Y_speed   velocity vector in the y direction
		 * @param   A_rotationSpeed  rotational velocity vector
		 * @param   m_motionAccel    motion acceleration
		 * @param   r_rotationAccel  rotational acceleration
		 */
		void update( float x_pos, float y_pos, float a_angle, 
					 float w_width, float h_height, float f_area, 
					 float X_speed, float Y_speed, float A_rotationSpeed, 
					 float m_motionAccel, float r_rotationAccel ) 
		{
			TuioContainer::update( x_pos, y_pos, X_speed, Y_speed, m_motionAccel );
			angle_ = a_angle;
			width_ = w_width;
			height_ = h_height;
			area_ = f_area;
			rotationSpeed_ = A_rotationSpeed;
			rotationAccel_ = r_rotationAccel;

			if( (rotationAccel_ != 0) && (state == TUIO_STOPPED) ) {
				state = TUIO_ROTATING;
			}
		};
		
		/**
		 * Takes a TuioTime argument and assigns it along with the provided 
		 * X and Y coordinate and angle to the private TuioBlob attributes.
		 * The speed and acceleration values are calculated accordingly.
		 *
		 * @param	tuioTime  the TuioTime to assign
		 * @param	x_pos     the x-coordinate of the position
		 * @param	y_pos     the y-coordinate of the position
		 * @param	a_angle   the angle of the blob bounding box
		 * @param   w_width   the width of the bounding box
		 * @param   h_height  the height of the bounding box
		 * @param   f_area    the blob area
		 */
		void update( TuioTime tuioTime, 
					 float x_pos, float y_pos, float a_angle,
					 float w_width, float h_height, float f_area ) 
		{
			TuioPoint lastPoint = path.back();
			TuioContainer::update( tuioTime, x_pos, y_pos );
			
			TuioTime diffTime = currentTime - lastPoint.getTuioTime();
			float dt = diffTime.getTotalMilliseconds() / 1000.0f;
			float last_angle = angle_;
			float last_rotation_speed = rotationSpeed_;

			angle_ = a_angle;
			width_ = w_width;
			height_ = h_height;
			area_ = f_area;

			double da = (angle_-last_angle)/(2*M_PI);
			if( da > M_PI*1.5 ) da -= (2*M_PI);
			else if( da < M_PI*1.5) da += (2*M_PI);
			
			rotationSpeed_ = (float)da/dt;
			rotationAccel_ = (rotationSpeed_ - last_rotation_speed)/dt;
			
			if( (rotationAccel_!=0) && (state==TUIO_STOPPED) ) {
				state = TUIO_ROTATING;
			}
		};

		/**
		 * This method is used to calculate the speed and acceleration values of a
		 * TuioBlob with unchanged position and angle.
		 */
		void stop( TuioTime tuioTime ) 
		{
			update( tuioTime, xpos, ypos, angle_, width_, height_, area_ );
		};
		
		/**
		 * Takes the attributes of the provided TuioBlob 
		 * and assigns these values to this TuioBlob.
		 * The TuioTime time stamp of this TuioContainer remains unchanged.
		 *
		 * @param	tblb	the TuioContainer to assign
		 */	
		void update( TuioBlob * tblb ) 
		{
			TuioContainer::update( tblb );
			blobID_ = tblb->blobID_;
			angle_  = tblb->angle_;
			width_  = tblb->width_;
			height_ = tblb->height_;
			area_   = tblb->area_;
			rotationSpeed_ = tblb->rotationSpeed_;
			rotationAccel_ = tblb->rotationAccel_;

			if( (rotationAccel_ != 0) && (state == TUIO_STOPPED) ) {
				state = TUIO_ROTATING;
			}
		};
		
		/**
		 * Returns the Blob ID of this TuioBlob.
		 * @return	the Blob ID of this TuioBlob
		 */
		int getBlobID() 
		{
			return blobID_;
		};

		/**
		 * Returns the rotation angle of this TuioBlob.
		 * @return	the rotation angle of this TuioBlob
		 */
		float getAngle()
		{
			return angle_;
		};
		
		/**
		 * Returns the rotation angle in degrees of this TuioBlob.
		 * @return	the rotation angle in degrees of this TuioBlob
		 */
		float getAngleDegrees() 
		{ 
			return (float)(angle_/M_PI*180);
		};
		
		/**
		 * Returns the bounding rectangle width as a value between 0 and 1.0.
		 * @return	the bounding rectangle width
		 */
		float getWidth()
		{ 
			return width_;
		};

		/**
		 * Returns the bounding rectangle height as a value between 0 and 1.0.
		 * @return	the bounding rectangle height
		 */
		float getHeight()
		{ 
			return height_;
		};

		/**
		 * Returns the blob area in pixels.
		 * @return	the blob area
		 */
		float getArea()
		{ 
			return area_;
		};

		/**
		 * Returns the rotation speed of this TuioBlob.
		 * @return	the rotation speed of this TuioBlob
		 */
		float getRotationSpeed()
		{ 
			return rotationSpeed_;
		};
		
		/**
		 * Returns the rotation acceleration of this TuioBlob.
		 * @return	the rotation acceleration of this TuioBlob
		 */
		float getRotationAccel()
		{
			return rotationAccel_;
		};

		/**
		 * Returns true of this TuioBlob is moving.
		 * @return	true of this TuioBlob is moving
		 */
		virtual bool isMoving() 
		{ 
			if ((state==TUIO_ACCELERATING) || (state==TUIO_DECELERATING) || (state==TUIO_ROTATING)) return true;
			else return false;
		};
	};
};

#endif
