/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Ole Ciliox <ole@ira.uka.de>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*======================================================================
 Fast Ogre Rendering For CGKIT (FORC)

 Author: Ole Ciliox (ole@ira.uka.de)
======================================================================*/
 
#ifndef __MyMouseListener_H__
#define __MyMouseListener_H__

#include "OgreEventListeners.h"
#include "OgreKeyEvent.h"
#include "Ogre.h"
#include "OgreCore.h"

#define MAX_MOTIONS 10

using namespace Ogre;
using std::cout;
using std::endl;
//
//
// **********************************************************************
// * class MyMouseListener						*
// *									*
// * Temporary class until ogre 1.0 is released				*
// **********************************************************************
//
class MyMouseListener: public MouseMotionListener
{
protected:

public:
	float moveX;
	float moveY;
	float moveRelX;
	float moveRelY;

	bool motion;
	bool lMB;
	bool mMB;
	bool rMB;

	MyMouseListener() 
	{
		moveX = 0.0; moveY = 0.0; moveRelX = 0.0; moveRelY = 0.0;
		motion = false;
		lMB = false;
		mMB = false;
		rMB = false;
	};

	~MyMouseListener()
	{

	};

	// MODIFIERS LEGENDE ;;;;;;;;;;;;;;;;;;;;
	// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	// 16 : linker Mausknopf
	// 32 : rechter Mausknopf
	// 64 : mittlerer Mausknopf
	// 1  : shift zusätzlich
	// 2  : strg zuätzlich
	// 4  : meta zuätzlich (?)
	// 8  : alt zusätzlich
	// MODIFIERS : gesamtwert
	// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	void mouseMoved(MouseEvent *e)
	{
		motion = true;
		//snapshot = e;
		//cout << "Mouse Moved!" << endl;

		//cout << "true values: " << e->getX() << " " << e->getY() << " " << e->getRelX() << " " << e->getRelY() << endl;

		moveX = (float)(e->getX());
		moveY = (float)(e->getY());
		moveRelX = (float)(e->getRelX());
		moveRelY = (float)(e->getRelY());


		lMB = (e->getModifiers() & InputEvent::BUTTON0_MASK) != 0;
		mMB = (e->getModifiers() & InputEvent::BUTTON2_MASK) != 0;
		rMB = (e->getModifiers() & InputEvent::BUTTON1_MASK) != 0;

		//e->consume();

	};

	void mouseDragged(MouseEvent *e)
	{
		motion = true;
		//snapshot = e;
		//cout << "Mouse Dragged!" << endl;

		//cout << "MODIFIERS: " << e->getModifiers() << endl;

		moveX = (float)(e->getX());
		moveY = (float)(e->getY());
		moveRelX = (float)(e->getRelX());
		moveRelY = (float)(e->getRelY());
		
		lMB = (e->getModifiers() & InputEvent::BUTTON0_MASK) != 0;
		mMB = (e->getModifiers() & InputEvent::BUTTON2_MASK) != 0;
		rMB = (e->getModifiers() & InputEvent::BUTTON1_MASK) != 0;
	};

	void mouseDragMoved(MouseEvent *e)
	{
		motion = true;
		//snapshot = e;
		//cout << "Mouse DragMoved!" << endl;

		//cout << "MODIFIERS: " << e->getModifiers() << endl;

		moveX = (float)(e->getX());
		moveY = (float)(e->getY());
		moveRelX = (float)(e->getRelX());
		moveRelY = (float)(e->getRelY());
		
		lMB = (e->getModifiers() & InputEvent::BUTTON0_MASK) != 0;
		mMB = (e->getModifiers() & InputEvent::BUTTON2_MASK) != 0;
		rMB = (e->getModifiers() & InputEvent::BUTTON1_MASK) != 0;
	};

	

	void flush() 
	{

	};
};
	
#endif
