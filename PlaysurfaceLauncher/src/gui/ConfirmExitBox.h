/*******************************************************************************
ConfirmExitBox

PURPOSE: Draws the Confirm Exit buttons seen in ConfirmExitBox.ui file and
         provides a means of listening for a touch on a button.

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
#ifndef GUI_CONFIRMEXITBOX_H
#define GUI_CONFIRMEXITBOX_H

#include "ui_ConfirmExitBox.h"
#include <QWidget>

namespace gui 
{
    class ConfirmExitBox : public QWidget
    {
        Q_OBJECT

    public:
        ConfirmExitBox( QWidget * parent = 0 );
        virtual ~ConfirmExitBox();

        bool isPointOnYesButton( double x, double y );
        bool isPointOnNoButton( double x, double y );

    public slots:
        void show();
        void close();

    private:
        Ui::ConfirmExitBoxUi ui_;
    };
}

#endif
