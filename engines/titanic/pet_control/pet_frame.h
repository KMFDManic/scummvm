/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_PET_FRAME_H
#define TITANIC_PET_FRAME_H

#include "titanic/pet_control/pet_section.h"
#include "titanic/pet_control/pet_gfx_element.h"

namespace Titanic {

/**
 * This implements the frame and background for the PET display.
 * This includes the area buttons and title
 */
class CPetFrame : public CPetSection {
private:
	CPetGfxElement _modeButtons[6];
	CPetGfxElement _titles[6];
	CPetGfxElement _modeBackground;
	CPetGfxElement _val2;
	CPetGfxElement _val3;
	CPetGfxElement _background;
	CPetGfxElement _squares[7];
private:
	/**
	 * Called to set the owning PET instance and set some initial state
	 */
	bool setPetControl(CPetControl *petControl);
public:
	CPetFrame();

	/**
	 * Sets up the section
	 */
	virtual bool setup(CPetControl *petControl);
	
	/**
	 * Sets up the section
	 */
	virtual bool reset();
	
	/**
	 * Handles mouse down messages
	 */
	virtual bool MouseButtonDownMsg(CMouseButtonDownMsg *msg);
	virtual bool MouseButtonUpMsg(CMouseButtonUpMsg *msg) { return false; }

	/**
	 * Returns true if the object is in a valid state
	 */
	virtual bool isValid(CPetControl *petControl);

	/**
	 * Called after a game has been loaded
	 */
	virtual void postLoad();

	/**
	 * Called when the current PET area changes
	 */
	void setArea(PetArea newArea);

	/**
	 * Reset the currently selected area
	 */
	void resetArea();

	/**
	 * Draws the PET frame
	 */
	void drawFrame(CScreenManager *screenManager);

	/**
	 * Draws the indent
	 */
	void drawSquares(CScreenManager *screenManager, int count);
};

} // End of namespace Titanic

#endif /* TITANIC_PET_FRAME_H */
