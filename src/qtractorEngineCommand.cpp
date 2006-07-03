// qtractorEngineCommand.cpp
//
/****************************************************************************
   Copyright (C) 2005-2006, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*****************************************************************************/

#include "qtractorAbout.h"
#include "qtractorEngineCommand.h"

#include "qtractorMainForm.h"

#include "qtractorMidiEngine.h"
#include "qtractorMonitor.h"
#include "qtractorMixer.h"
#include "qtractorMeter.h"


//----------------------------------------------------------------------
// class qtractorBusCommand - implementation
//

// Constructor.
qtractorBusCommand::qtractorBusCommand ( qtractorMainForm *pMainForm,
	const QString& sName, qtractorBus *pBus, qtractorBus::BusMode busMode )
	: qtractorCommand(pMainForm, sName), m_pBus(pBus), m_busMode(busMode)
{
}


//----------------------------------------------------------------------
// class qtractorBusGainCommand - implementation.
//

// Constructor.
qtractorBusGainCommand::qtractorBusGainCommand (
	qtractorMainForm *pMainForm, qtractorBus *pBus,
	qtractorBus::BusMode busMode, float fGain )
	: qtractorBusCommand(pMainForm, "bus gain", pBus, busMode)
{
	m_fGain     = fGain;
	m_fPrevGain = 1.0f;
	m_bPrevGain = false;

	setRefresh(false);

	// Try replacing an previously equivalent command...
	static qtractorBusGainCommand *s_pPrevGainCommand = NULL;
	if (s_pPrevGainCommand) {
		qtractorCommand *pLastCommand
			= mainForm()->commands()->lastCommand();
		qtractorCommand *pPrevCommand
			= static_cast<qtractorCommand *> (s_pPrevGainCommand);
		if (pPrevCommand == pLastCommand
			&& s_pPrevGainCommand->bus() == pBus
			&& s_pPrevGainCommand->busMode() == busMode) {
			qtractorBusGainCommand *pLastGainCommand
				= static_cast<qtractorBusGainCommand *> (pLastCommand);
			if (pLastGainCommand) {
				// Equivalence means same (sign) direction too...
				float fPrevGain = pLastGainCommand->prevGain();
				float fLastGain = pLastGainCommand->gain();
				int   iPrevSign = (fPrevGain > fLastGain ? +1 : -1);
				int   iCurrSign = (fPrevGain < m_fGain   ? +1 : -1); 
				if (iPrevSign == iCurrSign) {
					m_fPrevGain = fLastGain;
					m_bPrevGain = true;
					mainForm()->commands()->removeLastCommand();
				}
			}
		}
	}
	s_pPrevGainCommand = this;
}


// Bus-gain command method.
bool qtractorBusGainCommand::redo (void)
{
	qtractorBus *pBus = bus();
	if (pBus == NULL)
		return false;

	// Set Bus gain (repective monitor gets set too...)
	float fGain = m_fPrevGain;
	if ((busMode() & qtractorBus::Input) && pBus->monitor_in()) {
		if (!m_bPrevGain)
			fGain = pBus->monitor_in()->gain();	
		pBus->monitor_in()->setGain(m_fGain);
	}
	if ((busMode() & qtractorBus::Output) && pBus->monitor_out()) {
		if (!m_bPrevGain)
			fGain = pBus->monitor_out()->gain();	
		pBus->monitor_out()->setGain(m_fGain);
	}
	// MIDI busses are special...
	if (pBus->busType() == qtractorTrack::Midi) {
		// Now we gotta make sure of proper MIDI bus...
		qtractorMidiBus *pMidiBus
			= static_cast<qtractorMidiBus *> (pBus);
		if (pMidiBus)
			pMidiBus->setMasterVolume(m_fGain);
	}

	// Set undo value...
	m_bPrevGain = false;
	m_fPrevGain = m_fGain;
	m_fGain     = fGain;

	// Mixer/Meter turn...
	qtractorMixer *pMixer = mainForm()->mixer();
	if (pMixer) {
		qtractorMixerStrip *pStrip;
		if ((busMode() & qtractorBus::Input) && pBus->monitor_in()) {
			pStrip = pMixer->inputRack()->findStrip(pBus->monitor_in());
			if (pStrip && pStrip->meter())
				pStrip->meter()->updateGain();
		}
		if ((busMode() & qtractorBus::Output) && pBus->monitor_out()) {
			pStrip = pMixer->outputRack()->findStrip(pBus->monitor_out());
			if (pStrip && pStrip->meter())
				pStrip->meter()->updateGain();
		}
	}

	return true;
}


//----------------------------------------------------------------------
// class qtractorBusPanningCommand - implementation.
//

// Constructor.
qtractorBusPanningCommand::qtractorBusPanningCommand (
	qtractorMainForm *pMainForm, qtractorBus *pBus,
	qtractorBus::BusMode busMode, float fPanning )
	: qtractorBusCommand(pMainForm, "bus pan", pBus, busMode)
{
	m_fPanning = fPanning;
	m_fPrevPanning = 0.0f;
	m_bPrevPanning = false;

	setRefresh(false);

	// Try replacing an previously equivalent command...
	static qtractorBusPanningCommand *s_pPrevPanningCommand = NULL;
	if (s_pPrevPanningCommand) {
		qtractorCommand *pLastCommand
			= mainForm()->commands()->lastCommand();
		qtractorCommand *pPrevCommand
			= static_cast<qtractorCommand *> (s_pPrevPanningCommand);
		if (pPrevCommand == pLastCommand
			&& s_pPrevPanningCommand->bus() == pBus
			&& s_pPrevPanningCommand->busMode() == busMode) {
			qtractorBusPanningCommand *pLastPanningCommand
				= static_cast<qtractorBusPanningCommand *> (pLastCommand);
			if (pLastPanningCommand) {
				// Equivalence means same (sign) direction too...
				float fPrevPanning = pLastPanningCommand->prevPanning();
				float fLastPanning = pLastPanningCommand->panning();
				int   iPrevSign    = (fPrevPanning > fLastPanning ? +1 : -1);
				int   iCurrSign    = (fPrevPanning < m_fPanning   ? +1 : -1); 
				if (iPrevSign == iCurrSign) {
					m_fPrevPanning = fLastPanning;
					m_bPrevPanning = true;
					mainForm()->commands()->removeLastCommand();
				}
			}
		}
	}
	s_pPrevPanningCommand = this;
}


// Bus-panning command method.
bool qtractorBusPanningCommand::redo (void)
{
	qtractorBus *pBus = bus();
	if (pBus == NULL)
		return false;

	// Set bus panning (repective monitor gets set too...)
	float fPanning = m_fPrevPanning;	
	if ((busMode() & qtractorBus::Input) && pBus->monitor_in()) {
		if (!m_bPrevPanning)
			fPanning = pBus->monitor_in()->panning();	
		pBus->monitor_in()->setPanning(m_fPanning);
	}
	if ((busMode() & qtractorBus::Output) && pBus->monitor_out()) {
		if (!m_bPrevPanning)
			fPanning = pBus->monitor_out()->panning();	
		pBus->monitor_out()->setPanning(m_fPanning);
	}

	// Set undo value...
	m_bPrevPanning = false;
	m_fPrevPanning = m_fPanning;
	m_fPanning     = fPanning;

	// Mixer/Meter turn...
	qtractorMixer *pMixer = mainForm()->mixer();
	if (pMixer) {
		qtractorMixerStrip *pStrip;
		if ((busMode() & qtractorBus::Input) && pBus->monitor_in()) {
			pStrip = pMixer->inputRack()->findStrip(pBus->monitor_in());
			if (pStrip && pStrip->meter())
				pStrip->meter()->updatePanning();
		}
		if ((busMode() & qtractorBus::Output) && pBus->monitor_out()) {
			pStrip = pMixer->outputRack()->findStrip(pBus->monitor_out());
			if (pStrip && pStrip->meter())
				pStrip->meter()->updatePanning();
		}
	}

	return true;
}


// end of qtractorEngineCommand.cpp
