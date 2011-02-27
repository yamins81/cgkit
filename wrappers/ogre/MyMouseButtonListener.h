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

#ifndef __MyMouseBListener_H__ 
#define __MyMouseBListener_H__

#include "OgreEventListeners.h"
#include "OgreKeyEvent.h"
#include "Ogre.h"
#include "OgreCore.h"

#define MAX_MOUSEBUTTONEVENTS 10



// bis jetzt unbuffered: immer nur letzter klick wird beachtet
// TODO :: PUFFERN
// TODO :: An CGKIT übergeben (wrapper funktion schreiben)

using namespace Ogre;
//
//
// **********************************************************************
// * class MyMouseButtonListener					*
// *									*
// * Temporary class until ogre 1.0 is released				*
// **********************************************************************
//
class MyMouseButtonListener: public MouseListener
{
protected:

public:
	int *mouseButtonList; // 0 LMB, 1 MMB, 3 RMB
	float *mouseXList;
	float *mouseYList;
	float *mousedXList;
	float *mousedYList;
	int *eventTypeList; // 0 PRESS, 1 RELEASE

	int *mouseButtonRoot;
	float *mouseYRoot;
	float *mousedXRoot;
	float *mousedYRoot;
	int *eventTypeRoot;

	int mouseCount;

	MyMouseButtonListener() 
	{
		mouseButtonList = new int[MAX_MOUSEBUTTONEVENTS];
		mouseXList = new float[MAX_MOUSEBUTTONEVENTS];
		mouseYList = new float[MAX_MOUSEBUTTONEVENTS];
		mousedXList = new float[MAX_MOUSEBUTTONEVENTS];
		mousedYList = new float[MAX_MOUSEBUTTONEVENTS];	
		eventTypeList = new int[MAX_MOUSEBUTTONEVENTS];	
		
		mouseButtonRoot = mouseButtonList;
		mouseXList = mouseXList;
		mouseYRoot = mouseYList;
		mousedXRoot = mousedXList;
		mousedYRoot = mousedYList;
		eventTypeRoot = eventTypeList;
	
		mouseCount = 0;
	};

	~MyMouseButtonListener() 
	{
		delete mouseButtonList;
		delete mouseXList;
		delete mouseYList;
		delete mousedXList;
		delete mousedYList;		
		delete eventTypeList;
	};

	void mouseReleased(MouseEvent *e) // Type = 1
	{
	
		//cout << "Mouse released!" << endl;

		mouseCount++;

		if( mouseCount > MAX_MOUSEBUTTONEVENTS )
		{
			//cout << "temporary queue full" << endl;
			return;
		}

		mouseButtonList[ mouseCount-1 ] = e->getButtonID();
		mouseXList[ mouseCount-1 ] = e->getX();
		mouseYList[ mouseCount-1 ] = e->getY();
		mousedXList[ mouseCount-1 ] = e->getRelX();
		mousedYList[ mouseCount-1 ] = e->getRelY();
		eventTypeList[ mouseCount-1 ] = 1;

	};

	void mousePressed(MouseEvent *e) // Type = 0
	{


		//cout << "Mouse preleased!" << endl;

		mouseCount++;

		if( mouseCount > MAX_MOUSEBUTTONEVENTS )
		{
			//cout << "temporary queue full" << endl;
			return;
		}

		mouseButtonList[ mouseCount-1 ] = e->getButtonID();

		//cout << "###################" << e->getButtonID() << endl;

		mouseXList[ mouseCount-1 ] = e->getX();
		mouseYList[ mouseCount-1 ] = e->getY();
		mousedXList[ mouseCount-1 ] = e->getRelX();
		mousedYList[ mouseCount-1 ] = e->getRelY();
		eventTypeList[ mouseCount-1 ] = 0;

	};

	void mouseClicked(MouseEvent *e)  
	{

	};

	void mouseExited(MouseEvent *e)
	{

	};

	void mouseEntered(MouseEvent *e)
	{

	};

	void flush() 
	{
		mouseCount = 0;

		mouseButtonList = mouseButtonRoot;
		mouseXList = mouseXList;
		mouseYList = mouseYRoot;
		mousedXList = mousedXRoot;
		mousedYList = mousedYRoot;
		eventTypeList = eventTypeRoot;
	};
};
	
#endif
