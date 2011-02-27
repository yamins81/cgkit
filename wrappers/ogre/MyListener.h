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

#ifndef __MyFrameListener_H__
#define __MyFrameListener_H__
 
#include "OgreEventListeners.h"
#include "OgreKeyEvent.h"
#include "Ogre.h"

#define MAX_KEYDOWNS 10
#define MAX_KEYUPS 10

using namespace Ogre;
//
//
// **********************************************************************
// * class MyListener							*
// *									*
// * Temporary class until ogre 1.0 is released				*
// **********************************************************************
//
class MyListener: public KeyListener
{
protected:

public:
	char *keyCharList;
	int *keyIntList;
	char *charRoot;
	int *intRoot;
	int *keyModifiersList;
	int *modifiersRoot;

	int pressedCount;

	char *keyCharListUp;
	int *keyIntListUp;
	char *charRootUp;
	int *intRootUp;
	int *keyModifiersListUp;
	int *modifiersRootUp;

	int releasedCount;

	MyListener() 
	{
		keyCharList = new char[MAX_KEYDOWNS];
		keyIntList = new int[MAX_KEYDOWNS];
		keyModifiersList = new int[MAX_KEYDOWNS];
		charRoot = keyCharList;
		intRoot = keyIntList;
		modifiersRoot = keyModifiersList;
		pressedCount = 0;

		keyCharListUp = new char[MAX_KEYUPS];
		keyIntListUp = new int[MAX_KEYUPS];
		keyModifiersListUp = new int[MAX_KEYUPS];
		charRootUp = keyCharListUp;
		intRootUp = keyIntListUp;
		modifiersRootUp = keyModifiersListUp;
		releasedCount = 0;
	};

	~MyListener() 
	{
		delete keyCharList;
		delete keyIntList;
		delete keyCharListUp;
		delete keyIntListUp;

		delete keyModifiersList;
		delete keyModifiersListUp;
	};

	void keyClicked(KeyEvent* e) 
	{

	}; 

	void keyPressed(KeyEvent* e) 
	{
		pressedCount++;

		//cout << "Key number: " << pressedCount << endl;

		if( pressedCount > MAX_KEYDOWNS )
		{
			//cout << "temporary queue full" << endl;
			return;
		}

		/*
		*keyCharList = e->getKeyChar();
		keyCharList++;
		*keyIntList = e->getKey();
		keyIntList++;
		*/

		keyCharList[ pressedCount-1 ] = e->getKeyChar();
		keyIntList[ pressedCount-1 ] = e->getKey();
		keyModifiersList[ pressedCount-1 ] = e->getModifiers();
		
	};

	void flush() 
	{
		keyCharList = charRoot;
		keyIntList = intRoot;	
		keyModifiersList = modifiersRoot;
		
		pressedCount = 0;
	};

	void flushUp() 
	{
		keyCharListUp = charRootUp;
		keyIntListUp = intRootUp;	
		keyModifiersListUp = modifiersRootUp;
		
		releasedCount = 0;
	};

	void keyReleased(KeyEvent* e) 
	{
		//cout << "Key pressed!" << endl;

		releasedCount++;

		//cout << "Key number: " << pressedCount << endl;

		if( releasedCount > MAX_KEYUPS )
		{
			//cout << "temporary queue full" << endl;
			return;
		}

		keyCharListUp[ releasedCount-1 ] = e->getKeyChar();
		keyIntListUp[ releasedCount-1 ] = e->getKey();
		keyModifiersListUp[ releasedCount-1 ] = e->getModifiers();
	};

protected:

};

#endif
