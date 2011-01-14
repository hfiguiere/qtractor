// qtractorTrackButton.h
//
/****************************************************************************
   Copyright (C) 2005-2011, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __qtractorTrackButton_h
#define __qtractorTrackButton_h

#include "qtractorTrack.h"

#include <QPushButton>

#include "qtractorObserverWidget.h"


//----------------------------------------------------------------------------
// qtractorTrackButton -- Track observer tool button.

class qtractorTrackButton : public qtractorObserverWidget<QPushButton>
{
	Q_OBJECT

public:

	// Constructor.
	qtractorTrackButton(qtractorTrack *pTrack,
		qtractorTrack::ToolType toolType,
		const QSize& fixedSize, QWidget *pParent = 0);

	// Specific accessors.
	void setTrack(qtractorTrack *pTrack);
	qtractorTrack *track() const;

	qtractorTrack::ToolType toolType() const;

protected slots:

	// Special toggle slot.
	void toggledSlot(bool bOn);

	// MIDI controller/observer attachment (context menu) slot.
	void midiControlActionSlot();

protected:

	// MIDI controller/observer attachment (context menu) activator.
	void addMidiControlAction(qtractorMidiControlObserver *pMidiObserver);

	// Visitor setup.
	void updateTrack();

	// Visitors overload.
	void updateValue(float fValue);

private:

	// Instance variables.
	qtractorTrack *m_pTrack;
	qtractorTrack::ToolType m_toolType;

	int m_iUpdate;

	// Special background colors.
	QColor m_rgbText;
	QColor m_rgbOn;
	QColor m_rgbOff;
};


#endif  // __qtractorTrackButton_h


// end of qtractorTrackButton.h
