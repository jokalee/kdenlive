/***************************************************************************
                          kmmtrackbase.h  -  description
                             -------------------
    begin                : Sat Feb 16 2002
    copyright            : (C) 2002 by Jason Wood
    email                : jasonwood@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMMTRACKBASE_H
#define KMMTRACKBASE_H

#include <qhbox.h>

/**Base class for all track types.
  *@author Jason Wood
  */

class KMMTrackBase : public QHBox  {
   Q_OBJECT
public: 
	KMMTrackBase(QWidget *parent=0, const char *name=0);
	~KMMTrackBase();
};

#endif
