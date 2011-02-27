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
# $Id: viewer.py,v 1.13 2006/03/03 09:13:59 mbaas Exp $

"""Viewer tool.

Global options:

background     Background color (must be anything that can be passed to the
               vec4 constructor).
fullscreen     Boolean indicating if a full screen view should be used
stereo         Stereo setting (None, "vsplit", "glstereo")
eyedistance    Eye distance for stereo display
polygonmode    Drawing mode ("fill", "line", "point")
navigationmode Selects the navigation mode ("maya", "max", "softimage")
resolution     (width, height)  (aspect is ignored)
fps            Framerate
camera         Specify camera to be used
"""

import sys, os, os.path
#sys.path = [r"D:\tmp\pygame_src\pygame-1.6.2\build\lib.win32-2.3"]+sys.path
import pygame
from pygame.locals import *
from cgkit._OpenGL.GL import *
#from cgkit._OpenGL.GLU import *
import cgkit._Image as Image

from math import *
from cgkit.all import *
from cgkit.scene import getScene
import cgkit
from cgkit.cmds import *
from cgkit.tool import Tool

import cgkit.wintab
from cgkit.wintab.constants import *
import cgkit.spacedevice


# Viewer
class Viewer(Tool):
    """Viewer tool."""
    
    def __init__(self):
        Tool.__init__(self, defaultoptionvar="VIEWER_DEFAULT_OPTIONS")

        self.pygame_joysticks = []
        self.cgkit_joysticks = []

        self.spacedevice = None
        self.enable_spacedevice = False  # Disable SpaceMouse for now

        self.wintabcontext = None
        self.enable_wintab = True

        if sys.platform!="win32":
            self.enable_spacedevice = False
            self.enable_wintab = False

        scene = getScene()
        scene.setGlobal("stereo", self.options.stereo)
        scene.setGlobal("polygonmode", self.options.polygon_mode)
        scene.setGlobal("navigationmode", self.options.navigation_mode)

        self.separate_specular_color = False
        self.draw_orientation = True

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
            308 : KEY_ALT_LEFT,
            307 : KEY_ALT_RIGHT,
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

    def setOptions(self, optparser):
        """Add options specific to this tool."""
        
        Tool.setOptions(self, optparser)
        optparser.add_option("-F", "--full-screen", action="store_true", default=False,
                             help="Full screen display")
        optparser.add_option("-S", "--stereo", metavar="MODE",
                             help="Activate stereo display (vsplit, glstereo)")
        optparser.add_option("-D", "--eye-distance", type="float", default=0.07,
                             help="Default eye distance for stereo display. Default: 0.07")
        optparser.add_option("-B", "--bounding-box", action="store_true", default=False,
                             help="Show bounding boxes")
        optparser.add_option("-P", "--polygon-mode", metavar="MODE",
                             help="Polygon mode (fill, line, point). Default: fill")
        optparser.add_option("-s", "--save", metavar="NAME",
                             help="Save screenshots as images.")
        optparser.add_option("-N", "--navigation-mode", metavar="MODE",
                             help="Navigation mode (MAX, Maya, Softimage). Default: Maya")
        optparser.add_option("-X", "--disable-spacedevice", action="store_true", default=False,
                             help="Disable SpaceMouse/SpaceBall.")
        optparser.add_option("-T", "--disable-wintab", action="store_true", default=False,
                             help="Disable tablet support.")
        

    # init
    def init(self):

        # Initialize pygame
        passed, failed = pygame.init()
        if failed>0:
            print "Warning: %d pygame modules couldn't be initialized"%failed

        # Initialize joysticks...
        numjoy = pygame.joystick.get_count()
        if self.options.verbose:
            print numjoy, "joysticks available"

        self.pygame_joysticks = []
        self.cgkit_joysticks = []
        for id in range(numjoy):
            j = pygame.joystick.Joystick(id)
            j.init()
            self.pygame_joysticks.append(j)
            # Create a cgkit joystick object...
            id = j.get_id()
            name = j.get_name()
            numaxes = j.get_numaxes()
            numballs = j.get_numballs()
            numhats = j.get_numhats()
            numbuttons = j.get_numbuttons()
            cgj = Joystick(id=id, name=name, numaxes=numaxes, numhats=numhats,
                           numballs=numballs, numbuttons=numbuttons)
            getScene().setJoystick(cgj)
            # Initialize the values
    #        for i in range(numaxes):
    #            cgj.setAxis(i, j.get_axis(i))
            self.cgkit_joysticks.append(cgj)
            if self.options.verbose:
                print "Joystick #%d (%s):"%(id, name)
                print "  %d axes, %d balls, %d buttons, %d hats"%(numaxes, numballs, numbuttons, numhats)

        if self.options.disable_spacedevice:
            self.enable_spacedevice = True  # temporarily invert the meaning of -X
#            self.enable_spacedevice = False
        if self.options.disable_wintab:
            self.enable_wintab = False

        # Initialize SpaceMouse/SpaceBall
        if self.enable_spacedevice and cgkit.spacedevice.available():
            try:
                self.spacedevice = cgkit.spacedevice.SpaceDevice()
            except RuntimeError, e:
                print "3DxWare exception:",e
        else:
            if self.options.verbose and self.enable_spacedevice:
                print "SpaceMouse/SpaceBall module (spacedevice) not available"

        # Initialize Wintab context
        if self.enable_wintab and cgkit.wintab.available():
            if self.options.verbose:
                info = cgkit.wintab.info(WTI_INTERFACE)
                print 'Wintab identification: "%s"'%info["WINTABID"]
                info = cgkit.wintab.info(WTI_DEVICES)
                print "Initializing %s..."%info["NAME"]

            self.wintabcontext = cgkit.wintab.Context()
            ctx = self.wintabcontext
            ctx.name = os.path.basename(sys.argv[0])
            ctx.options = 0

            # Compute the pktdata attribute...
            ncursors = cgkit.wintab.info(WTI_INTERFACE)["NCURSORS"]
            pktdata = 0
            for i in range(ncursors):
                info = cgkit.wintab.info(WTI_CURSORS+i)
                pd = info["PKTDATA"]
                if pd!=None:
                    pktdata |= pd
            ctx.pktdata = pktdata
        else:
            if self.options.verbose and self.enable_wintab:
                print "No Wintab driver available"

        # Create renderer
        self.renderer = GLRenderInstance()

    # setOptionsFromGlobals
    def setOptionsFromGlobals(self):
        Tool.setOptionsFromGlobals(self)

        scene = getScene()
        self.options.full_screen = scene.getGlobal("fullscreen", self.options.full_screen)

        self.options.eye_distance = float(scene.getGlobal("eyedistance", self.options.eye_distance))

        # Check the stereo option and initialize the variable "stereo_mode"
        Sopt = scene.getGlobal("stereo", None)
        self.stereo_mode = self.translateKeyWordOpt(Sopt,
                                { None:0, "vsplit":1, "glstereo":2 },
                                "Unknown stereo mode: '%s'")

        # Check the polygon mode option
        Popt = scene.getGlobal("polygonmode", "fill")
        self.polygon_mode = self.translateKeyWordOpt(Popt,
                                { None:2, "point":0, "line":1, "fill":2 },
                                "Unknown polygon mode: '%s'")

        # Check the navigationmode option
        Nopt = scene.getGlobal("navigationmode", "maya")
        self.navigation_mode = self.translateKeyWordOpt(Nopt,
                            { None:1, "max":0, "maya":1, "softimage":2 },
                            "Unknown navigation mode: '%s'")

    # action
    def action(self):
        # Display the scene using pygame...
        scene = getScene()
        timer = scene.timer()
        
        # Create a camera control component
        CameraControl(cam=self.cam, mode=self.navigation_mode)

        # Get options...
        fps = timer.fps
        width = self.options.width
        height = self.options.height

        # Open a window...
        pygame.display.set_caption("OpenGL viewer")
        if self.stereo_mode==2:
            pygame.display.gl_set_attribute(pygame.GL_STEREO, 1)
        flags = OPENGL | DOUBLEBUF
        if self.options.full_screen:
            flags |= FULLSCREEN
        srf = pygame.display.set_mode((width,height), flags)

        # Output OpenGL infos...
        if self.options.verbose:
            print "OpenGL information:"
            print "  Vendor  :",glGetString(GL_VENDOR)
            print "  Renderer:", glGetString(GL_RENDERER)
            print "  Version :",glGetString(GL_VERSION)
            
        # Check if stereo output is active...
        if self.stereo_mode==2:
            if pygame.display.gl_get_attribute(pygame.GL_STEREO)==0:
                raise RuntimeError, "Stereo buffers not supported by this hardware."

        # Try to get the native window handle
        # (this only works with pygame 1.6.2 and later)
        try:
            info = pygame.display.get_wm_info()
            hwnd = info["window"]
        except:
            hwnd = None

        # Open SpaceMouse/SpaceBall device...
        if self.spacedevice!=None:
            if hwnd==None:
                if sys.platform=="win32" and self.options.verbose:
                    print "SpaceMouse/SpaceBall support is not available with the installed version"
                    print "of pygame. Please upgrade to v1.6.2 or later."
            else:
                try:
                    self.spacedevice.open("viewer.py", hwnd)
                    self.spacedevice.setUIMode(True)
                    if self.options.verbose:
                        typ,btns,degs,beep,firmware = self.spacedevice.getDeviceInfo()
                        print "3D input device:",firmware
                        print "%s: %d buttons, %d degrees of freedom"%(typ,btns,degs)
                    pygame.event.set_allowed(SYSWMEVENT)
                except RuntimeError, e:
                    print "SpaceDevice:",e
                    print "SpaceMouse/SpaceBall support is disabled."
                    self.spacedevice = None

        # Open Wintab context
        if self.wintabcontext!=None:
            if hwnd==None:
                self.wintabcontext = None
                if sys.platform=="win32" and self.options.verbose:
                    print "Tablet support is not available with the installed version of pygame."
                    print "Please upgrade to v1.6.2 or later."
            else:
                self.wintabcontext.open(hwnd, True)
                pygame.event.set_allowed(SYSWMEVENT)

        # Event loop...
        self.running = True
        active = True
        clk = pygame.time.Clock()
        cnt = 0
        timer.startClock()
        while self.running:
            # Display the scene
            self.draw(self.cam, width, height)
            pygame.display.flip()

            # Handle events
            events = pygame.event.get()
            self.handleEvents(events)

            if self.wintabcontext!=None:
                self.handleWintabEvents()

            if self.time_end!=None and timer.time>self.time_end+1E-10:
                active = False

            if active:
                if self.options.save!=None:
                    self.saveScreenshot(srf)

                # Step time
                timer.step()

            # Sync
            clk.tick(fps)
            cnt+=1
#            if cnt==25:
#                print "fps: %d"%clk.get_fps()
#                print "Virtual time:",timer.time, "Real time:",timer.clock
#                cnt=0
        


    # handleEvents
    def handleEvents(self, events):
        eventmanager = eventManager()
        for e in events:
            if e.type==QUIT:
                self.running=False
            # KEYDOWN?
            elif e.type==KEYDOWN:
                if e.key==27:
                    self.running=False
                key = e.unicode
                code = self.keydict.get(e.key, e.key)
                mods = self.convertMods(e.mod)
                eventmanager.event(KEY_PRESS, KeyEvent(key, code, mods))
#                keyboard.setKeyValue(e.key, True)
            # KEYUP
            elif e.type==KEYUP:
                code = self.keydict.get(e.key, e.key)
                try:
                    key = unicode(chr(e.key))
                except:
                    key = u""
                mods = self.convertMods(e.mod)
                eventmanager.event(KEY_RELEASE, KeyEvent(key, code, mods))
#                keyboard.setKeyValue(e.key, False)
            # MOUSEBUTTONDOWN
            elif e.type==MOUSEBUTTONDOWN:
                x,y = e.pos
                x0 = float(x)/self.options.width
                y0 = float(y)/self.options.height
                if e.button==1:
                    eventname = LEFT_DOWN
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==2:
                    eventname = MIDDLE_DOWN
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==3:
                    eventname = RIGHT_DOWN
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==4:
                    eventname = MOUSE_WHEEL
                    evt = MouseWheelEvent(120, x, y, x0, y0)
                elif e.button==5:
                    eventname = MOUSE_WHEEL
                    evt = MouseWheelEvent(-120, x, y, x0, y0)
                else:
                    eventname = MOUSE_BUTTON_DOWN
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                eventmanager.event(eventname, evt)
            # MOUSEBUTTONUP
            elif e.type==MOUSEBUTTONUP:
                x,y = e.pos
                x0 = float(x)/self.options.width
                y0 = float(y)/self.options.height
                if e.button==1:
                    eventname = LEFT_UP
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==2:
                    eventname = MIDDLE_UP
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==3:
                    eventname = RIGHT_UP
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                elif e.button==4:
                    eventname = MOUSE_WHEEL
                    evt = MouseWheelEvent(120, x, y, x0, y0)
                elif e.button==5:
                    eventname = MOUSE_WHEEL
                    evt = MouseWheelEvent(-120, x, y, x0, y0)
                else:
                    eventname = MOUSE_BUTTON_UP
                    evt = MouseButtonEvent(e.button, x, y, x0, y0)
                eventmanager.event(eventname, evt)
            # MOUSEMOTION
            elif e.type==MOUSEMOTION:
                btns = 0
                b1,b2,b3 = e.buttons
                if b1:
                    btns |= 0x1
                if b2:
                    btns |= 0x2
                if b3:
                    btns |= 0x4                
                x,y = e.pos
                dx, dy = e.rel
                width = self.options.width
                height = self.options.height
                x0 = float(x)/width
                y0 = float(y)/height
                dx0 = float(dx)/width
                dy0 = float(dy)/height
                evt = MouseMoveEvent(x, y, dx, dy, x0, y0, dx0, dy0, btns)
                eventmanager.event(MOUSE_MOVE, evt)
            # JOYAXISMOTION
            elif e.type==JOYAXISMOTION:
                self.cgkit_joysticks[e.joy].setAxis(e.axis, e.value)
#                e = JoystickAxisEvent(e.joy, e.axis, e.value)
#                print e
#                eventmanager.event(JOYSTICK_AXIS, e)
            # JOYBALLMOTION
            elif e.type==JOYBALLMOTION:
                self.cgkit_joysticks[e.joy].setBall(e.ball, e.value)
#                e = JoystickBallEvent(e.joy, e.ball, e.value)
#                print e
#                eventmanager.event(JOYSTICK_BALL, e)
            # JOYHATMOTION
            elif e.type==JOYHATMOTION:
                x,y = e.value
                self.cgkit_joysticks[e.joy].setBall(e.hat, x, y)
#                e = JoystickHatEvent(e.joy, e.hat, x, y)
#                print e
#                eventmanager.event(JOYSTICK_HAT, e)
            # JOYBUTTONUP
            elif e.type==JOYBUTTONUP:
                 self.cgkit_joysticks[e.joy].setButton(e.button, False)
#                e = JoystickButtonEvent(e.joy, e.button)
#                print "up",e
#                eventmanager.event(JOYSTICK_BUTTON_UP, e)                
            # JOYBUTTONDOWN
            elif e.type==JOYBUTTONDOWN:
                 self.cgkit_joysticks[e.joy].setButton(e.button, True)
#                e = JoystickButtonEvent(e.joy, e.button)
#                print "down",e
#                eventmanager.event(JOYSTICK_BUTTON_DOWN, e)                
            # SYSWMEVENT
            elif e.type==SYSWMEVENT:
                if sys.platform=="win32" and not hasattr(e, "msg") and not pygame.event.get_blocked(SYSWMEVENT):
                    pygame.event.set_blocked(SYSWMEVENT)
                    print "Warning: This version of pygame does not allow processing system events."
                    if self.spacedevice!=None:
                        self.spacedevice=None
                        print " -> SpaceMouse/SpaceBall support is disabled."
                    if self.wintabcontext!=None:
                        print " -> Tablet support is only partially available."

                if self.spacedevice!=None:
                    self.handleSpaceEvents(e)
                
    # handleSpaceEvents
    def handleSpaceEvents(self, e):
        """Handle SpaceMouse/SpaceBall events.

        Check if the event e is a SpaceMouse/SpaceBall event and process it.
        The return value is True if the event was a SpaceMouse/SpaceBall event.

        This method may currently only be calld on Windows.
        """
        # Check if the event was a SpaceMouse event
        res, evttype, data = self.spacedevice.translateWin32Event(e.msg, e.wparam, e.lparam)
        if res!=cgkit.spacedevice.RetVal.IS_EVENT:
            return False

        eventmanager = eventManager()
        # Motion?
        if evttype==cgkit.spacedevice.EventType.MOTION_EVENT:
            t,r,period = data
            eventmanager.event(SPACE_MOTION, SpaceMotionEvent(vec3(t), vec3(r) ,period))
        # Button?
        elif evttype==cgkit.spacedevice.EventType.BUTTON_EVENT:
            pressed, released = data
            for b in pressed:
                eventmanager.event(SPACE_BUTTON_DOWN, SpaceButtonEvent(b))
            for b in released:
                eventmanager.event(SPACE_BUTTON_UP, SpaceButtonEvent(b))
        # Zero?
        elif evttype==cgkit.spacedevice.EventType.ZERO_EVENT:
            eventmanager.event(SPACE_ZERO)

        return True
        

    # handleWintabEvents
    def handleWintabEvents(self):
        """Poll and process Wintab events.

        This method may only be called if self.wintabcontext is not None!
        """
        eventmanager = eventManager()       
        pkts = self.wintabcontext.packetsGet(20)
        for p in pkts:
            eventmanager.event(TABLET, p)
        

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
        return res

    def draw(self, cam, width, height):
        scene = getScene()
        renderer = self.renderer

        # Set handedness
        renderer.left_handed = scene.handedness=="l"
        renderer.setViewport(0,0,width,height)

        renderer.draw_solid = True
        renderer.draw_bboxes = self.options.bounding_box
        renderer.draw_coordsys = False
        renderer.draw_orientation = self.draw_orientation
        renderer.smooth_model = True
        renderer.backface_culling = False
        renderer.separate_specular_color = self.separate_specular_color
        renderer.polygon_mode = self.polygon_mode  # 0=Point 1=Line 2=Fill
        renderer.stereo_mode = self.stereo_mode
        renderer.clearcol = vec4(scene.getGlobal("background", vec4(0.5,0.5,0.6,0)))

        V = cam.viewTransformation()
        V2 = None
        if renderer.stereo_mode!=0:
            d = getattr(cam, "eye_distance", self.options.eye_distance)
            T1 = mat4().translation(vec3(-d,0,0))
            T2 = mat4().translation(vec3(d,0,0))
            V2 = T2*V
            V = T1*V
            if renderer.stereo_mode==1:
                width /= 2

        # Set projection matrix
        near, far = cam.getNearFar()
        P = cam.projection(width,height,near,far)
        renderer.setProjection(P)

        # Set view matrix
        renderer.setViewTransformation(V, 0)
        if V2!=None:
            renderer.setViewTransformation(V2, 1)

        # Draw scene
        root = scene.worldRoot()
        renderer.paint(root)

    # saveScreenshot
    def saveScreenshot(self, srf):
        """Save the current window content.

        srf is the pygame Surface object.
        """
        name,ext = os.path.splitext(self.options.save)
        f = int(round(getScene().timer().frame))
        fname = "%s%04d%s"%(name, f, ext)
        print 'Saving "%s"...'%fname
        data = pygame.image.tostring(srf, "RGB")
        img = Image.fromstring("RGB", (srf.get_width(), srf.get_height()), data)
        img.save(fname)
        
        
        

######################################################################

if __name__=="__main__":
    viewer = Viewer()
    viewer.run()


