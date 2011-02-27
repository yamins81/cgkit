#!/usr/bin/env python
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is the Python Computer Graphics Kit.
#
# The Initial Developer of the Original Code is Matthias Baas.
# Portions created by the Initial Developer are Copyright (C) 2004
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Ole Ciliox <ole@ira.uka.de>
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****
# -*- coding: cp1252 -*-
import sys, os, optparse

from cgkit.all import *
from cgkit.scene import getScene
from math import *
from cgkit._core import OsgCore 
from cgkit.tool import Tool


class OsgViewer(Tool):
    """OsgViewer tool."""
    
    def __init__(self):
        Tool.__init__(self, defaultoptionvar="VIEWEROGRE_DEFAULT_OPTIONS")

        # Search path for OGRE material scripts
        self.materialpath = r".\materials"
        
        Nopt = self.options.navigation_mode
        if Nopt==None or Nopt.lower()=="maya":
            self.navigation_mode = 1
        elif Nopt.lower()=="max":
            self.navigation_mode = 0
        elif Nopt.lower()=="softimage":
            self.navigation_mode = 2
        else:
            raise ValueError, "Unknown navigation mode. '%s'"%Nopt

        Ropt = self.options.render_system
        if Ropt==None or Ropt.lower()=="gl":
            self.use_opengl = 1
        elif Ropt.lower()=="d3d":
            self.use_opengl = 0
        else:
            raise ValueError, "Unknown render system: '%s'"%Ropt

        Topt = self.options.shadow_mode
        if Topt==None or Topt.lower()=="stencil_add":
            self.shadowmode = 0
        elif Topt.lower()=="stencil_mod":
            self.shadowmode = 1
        elif Topt.lower()=="texture":
            self.shadowmode = 2	    
        else:
            raise ValueError, "Unknown shadow mode: '%s'"%Topt   

	print "init done"


    def setOptions(self, optparser):
        """Add options specific to this tool."""
        Tool.setOptions(self, optparser)
        optparser.add_option("-F", "--full-screen", action="store_true", default=False,
                             help="Full screen display")
        optparser.add_option("-N", "--navigation-mode", metavar="MODE",
                             help="Navigation mode (MAX, Maya, Softimage). Default: Maya")
        optparser.add_option("-d", "--no-shadow", action="store_true", default=False,
                             help="Disable shadows")
        optparser.add_option("-R", "--render-system", metavar="NAME",
                             help="Render system (gl, d3d). Default: gl")
        optparser.add_option("-S", "--stereo", action="store_true", default=False,
                             help="Activate stereo display")
        optparser.add_option("-m", "--shadow-mode", metavar="NAME",
                             help="Shadow Mode  (STENCIL_ADD, STENCIL_MOD, TEXTURE")

	print "options set"

    # init
    def init(self):
        pass
        self.keydict = {
              8  : KEY_BACK,
              9  : KEY_TAB,
             13 : KEY_RETURN,
             27 : KEY_ESCAPE,
             32 : KEY_SPACE,
            276 : KEY_LEFT,
            273 : KEY_UP,
            275 : KEY_RIGHT,
            274 : KEY_DOWN,
            301 : KEY_CAPSLOCK,
            304 : KEY_SHIFT_LEFT,
            303 : KEY_SHIFT_RIGHT,
            306 : KEY_CONTROL_LEFT,
            305 : KEY_CONTROL_RIGHT,
            233 : KEY_ALT_LEFT,
            234 : KEY_ALT_RIGHT,
            310 : KEY_WINDOWS_LEFT,
            309 : KEY_WINDOWS_RIGHT,
            319 : KEY_WINDOWS_MENU,
            317 : KEY_PRINT,
            302 : KEY_SCROLL,
             19 : KEY_PAUSE,
            277 : KEY_INSERT,
            127 : KEY_DELETE,
            278 : KEY_HOME,
            279 : KEY_END,
            280 : KEY_PRIOR,
            281 : KEY_NEXT,
            282 : KEY_F1,
            283 : KEY_F2,
            284 : KEY_F3,
            285 : KEY_F4,
            286 : KEY_F5,
            287 : KEY_F6,
            288 : KEY_F7,
            289 : KEY_F8,
            290 : KEY_F9,
            291 : KEY_F10,
            292 : KEY_F11,
            293 : KEY_F12,
            300 : KEY_NUMLOCK,
            256 : KEY_NUMPAD0,
            257 : KEY_NUMPAD1,
            258 : KEY_NUMPAD2,
            259 : KEY_NUMPAD3,
            260 : KEY_NUMPAD4,
            261 : KEY_NUMPAD5,
            262 : KEY_NUMPAD6,
            263 : KEY_NUMPAD7,
            264 : KEY_NUMPAD8,
            265 : KEY_NUMPAD9,
            266 : KEY_NUMPAD_DECIMAL,
            267 : KEY_NUMPAD_DIVIDE,
            268 : KEY_NUMPAD_MULTIPLY,
            269 : KEY_NUMPAD_SUBTRACT,
            270 : KEY_NUMPAD_ADD,
            271 : KEY_NUMPAD_ENTER
            }
            
    def convertMods(self, mods):
        """Convert pygame key modifier flags to cgkit modifier flags.
        """
        res = 0
        if mods & 0x0001 or mods & 0x0002:
            res |= KEYMOD_SHIFT
        if mods & 0x0040 or mods & 0x0080:
            res |= KEYMOD_CONTROL
        if mods & 0x0100 or mods & 0x0200:
            res |= KEYMOD_ALT
            print "ALT"
        return res

    # action
    def action(self):
        # Create a camera control component
        CameraControl(cam=self.cam, mode=self.navigation_mode)

        print "NAV MODE:       ", self.navigation_mode
        
        scene = getScene()
        timer = scene.timer()
        worldroot = scene.worldRoot()
        myOsg = OsgCore()
        self.osgcore = myOsg
        myOsg.initStuff(worldroot)
        myOsg.setupCamera(worldroot, self.cam, 0.1, 1000.0, self.cam.fov, self.options.width, self.options.height)
        running = True
        #abc = 600
	
        while(running):
            eventmanager = eventManager()
            # testing the time limit
            #abc -= 1
            #if (abc == 0):
            #    running = False            
            timer.step()
            myOsg.renderFrame()

            # KEYDOWN EVENTS #
            ##################
            
            kd = myOsg.pumpKeyDowns()
            i = 0
            while(i<len(kd)):
		#print kd # format char / int / int
                # quit when ESC is pressed
                code = self.keydict.get(kd[i+1], kd[i+1])
                if (code == KEY_ALT_LEFT):
                    print "alt left pressed"
                try:
                    key = unicode(chr(kd[i+1]))
                except:
                    key = ""                
                mods = self.convertMods(kd[i+2])
                eventmanager.event( KEY_PRESS, KeyEvent(key, code, mods) )
                if (kd[i+1] == 27):
                    print "bye bye"
                    #running = False
                i = i+3

            # KEYUP EVENTS #
            ##################
            
            kd = myOsg.pumpKeyUps()
            i = 0
            while(i<len(kd)):
		#print kd # format char / int / int
                # quit when ESC is pressed
                code = self.keydict.get(kd[i+1], kd[i+1])
                if (code == KEY_ALT_LEFT):
                    print "alt left released"                
                try:
                    key = unicode(chr(kd[i+1]))
                except:
                    key = ""                
                mods = self.convertMods(kd[i+2])
                eventmanager.event( KEY_RELEASE, KeyEvent(key, code, mods) )
                if (kd[i+1] == 27):
                    print "bye bye"
                    #running = False
                i = i+3                

            # MOUSE MOTION EVENTS #
            #######################

            mb = myOsg.pumpMotions()
            i = 0
            if ( len(mb) > 5 ): 
                #print mb
                #print self.cam.transform
                #print self.cam.transform.toList()
                x = mb[i]
                y = mb[i+1]
                dx = mb[i+2]
                dy = mb[i+3]
                btns = 0
                b1 = mb[i+4]
                b2 = mb[i+5]
                b3 = mb[i+6]
                if b1:
                    btns |= 0x1
                    #print "left mouse!"
                if b2:
                    btns |= 0x2
                    #print "middle mouse!"
                if b3:
                    btns |= 0x4
                    #print "right mouse!"
                #print "Motion X Y (ABS): ",x*((float)(self.options.width)), y*((float)(self.options.height))
                #print "Motion X Y (REEEEEL): ",dx*((float)(self.options.width)), dy*((float)(self.options.height))
                evt = MouseMoveEvent(x*((float)(self.options.width)), y*((float)(self.options.height)), dx*((float)(self.options.width)), dy*((float)(self.options.height)), x, y, dx, dy, btns)
                #evt = MouseMoveEvent(10, 10, 3, 3, 0.1, 0.1, 0.1, 0.1, 0x2)
                #print evt
                #print "MOUSE MOVE"
                eventmanager.event(MOUSE_MOVE, evt)

            # MOUSE BUTTON EVENTS #
            #######################

            mb = myOsg.pumpMouseDowns()
            i = 0
            while ( i<len(mb) ):
                print "mousebuttonevent"
                x = mb[i+2]*self.options.width
                y = mb[i+3]*self.options.height
                #print "X and Y: ", x, y
                x0 = mb[i+2]
                y0 = mb[i+3]
                # relative coords mb[i+4], mb[i+5] not used yet
                if ( mb[i+1] == 0 ): # MOUSE PRESSED
                    #print "mouse pressed"
                    if ( mb[i] == 1 ): # LEFT_MOUSE
                        print "left mouse down"
                        eventname = LEFT_DOWN
                        evt = MouseButtonEvent(1, x, y, x0, y0)
                    elif ( mb[i] == 2 ): # MIDDLE_MOUSE
                        eventname = MIDDLE_DOWN
                        evt = MouseButtonEvent(2, x, y, x0, y0)
                    elif ( mb[i] == 4 ): # RIGHT_MOUSE
                        eventname = RIGHT_DOWN
                        evt = MouseButtonEvent(3, x, y, x0, y0)
                    else: 
                        eventname = MOUSE_BUTTON_DOWN
                        evt = MouseButtonEvent(mb[i], x, y, x0, y0) # ?<---
                    eventmanager.event(eventname, evt)
                elif ( mb[i+1] == 1 ): # MOUSE RELEASED
                    #print "mouse released"
                    if ( mb[i] == 1 ): # LEFT_MOUSE
                        eventname = LEFT_UP
                        print "leftup"
                        evt = MouseButtonEvent(1, x, y, x0, y0)
                    elif ( mb[i] == 2 ): # MIDDLE_MOUSE
                        eventname = MIDDLE_UP
                        print "middleup"
                        evt = MouseButtonEvent(2, x, y, x0, y0)
                    elif ( mb[i] == 4 ): # RIGHT_MOUSE
                        eventname = RIGHT_UP
                        print "rightup"
                        evt = MouseButtonEvent(3, x, y, x0, y0)
                    else: 
                        eventname = MOUSE_BUTTON_UP
                        print "up"
                        evt = MouseButtonEvent(mb[i], x, y, x0, y0) # ?<---
                    eventmanager.event(eventname, evt)
                i = i+7                

	
viewer = OsgViewer()
viewer.run()

