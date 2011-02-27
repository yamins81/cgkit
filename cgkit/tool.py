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
# $Id: tool.py,v 1.6 2005/03/31 17:20:01 mbaas Exp $

## \file tool.py
## Contains the Tool base class.

import os, os.path, sys, optparse, re
import pluginmanager
from eventmanager import eventManager
from scene import getScene
from targetcamera import TargetCamera
from Interfaces import *
from cmds import *
import application


# Tool
class Tool:
    """Base class for command tools that process a cgkit scene.

    Derived classes have to implement the init() and action() method.
    The options are parsed in the constructor. To start the tool, the
    run() method has to be called. This method does the following:

    - Call init()
    - Check if input files are given
    - Load plugins
    - Load input files
    - Get or create a camera
    - Call action()

    Attributes:

    - time_start
    - time_end
    - frame_start
    - frame_end

    The global timer will be initialized with the frame rate and the
    start time.
    """

    def __init__(self, defaultoptionvar=None):
        """Constructor.

        defaultoptionvar is the name of an environment variable that
        contains the default options.
        """

        # Create the option parser, ...
        self.optparser = self.createOptionParser()
        # set the options...
        self.setOptions(self.optparser)

        scene = getScene()
        timer = scene.timer()

        # Parse the default options
        defaultopts = None
        if defaultoptionvar!=None:
            if defaultoptionvar in os.environ:
                optstring = os.environ[defaultoptionvar]
                args = optstring.split()
                defaultopts, dummy = self.optparser.parse_args(args)
                
        # ...and parse the command line. The options are in "options"
        self.options, self.args = self.optparser.parse_args(values=defaultopts)

        # Print default options
        if self.options.verbose:
            print "Python",sys.version
            if defaultoptionvar!=None:
                if defaultoptionvar in os.environ:
                    print "Default options in %s: %s"%(defaultoptionvar, os.environ[defaultoptionvar])
                else:
                    print "Environment variable %s not set."%defaultoptionvar

        # Determine screen resolution...
        self._no_resolution_specified = False
        if self.options.width==None:
            self.options.width = 640
            self._no_resolution_specified = True
        if self.options.height==None:
            self.options.height = int(self.options.width*3.0/4)

        # Set the global application object to this tool class
        application._app = self

    # init
    def init(self):
        pass

    # action
    def action(self):
        pass


    # createOptionParser
    def createOptionParser(self):
        """Create and OptionParser instance.

        \return Option parser.
        """
        usage = "usage: %prog [options] inputfiles"
        return optparse.OptionParser(usage)

    # setOptions
    def setOptions(self, optparser):
        """Set the options.

        \param optparser (\c OptionParser) Option parser
        """
        optparser.add_option("-f", "--fps", type="float", default=30,
                             help="Frame rate")
        optparser.add_option("-W", "--width", type="int", default=None,
                             help="Screen width")
        optparser.add_option("-H", "--height", type="int", default=None,
                             help="Screen height")
        optparser.add_option("-p", "--plugin", action="append", default=[],
                             help="Load a plugin file or directory")
        optparser.add_option("-v", "--verbose", action="store_true", default=False,
                             help="Output info messages")
        optparser.add_option("-c", "--camera", metavar="NAME",
                             help="Select a camera")
        optparser.add_option("-b", "--begin", metavar="TIME",
                             help="Specify time or frame where to begin")
        optparser.add_option("-e", "--end", metavar="TIME",
                             help="Specify time or frame where to end")
        optparser.add_option("-U", "--up", metavar="AXIS",
                             help="Specify the up axis ('y' or 'z')")
        optparser.add_option("-O", "--option", action="append", default=[],
                             help="Add a global scene option")
        optparser.add_option("-t", "--set-time", action="store_true", default=False,
                             help="Set starting time directly instead of stepping there")

    # run
    def run(self):
        """Run the tool.

        This method calls the init() and action() method.
        """

        # Custom initialization
        self.init()
             
        # No input file given? Then print the help page and exit
        if len(self.args)==0:
            self.optparser.print_help()
            return

        # Load plugins
        self.loadPlugins()

        # Load the input files...
        for filename in self.args:
            if self.options.verbose:
                print 'Loading "%s"...'%filename
            load(filename)

        # Convert global settings into command line options
        self.setOptionsFromGlobals()

        self.cam = self.getCamera()

        self.action()

        getScene().clear()

    # setOptionsFromGlobals
    def setOptionsFromGlobals(self):
        """Convert global settings into command line options.

        This method is called by the run() method after the input
        files have been read. The method inspects the global settings
        and modifies the command line options as appropriate.
        """
        
        scene = getScene()
        timer = scene.timer()

        # Set additional global options
        for s in self.options.option:
            f = s.split("=")
            name = f[0]
            val = "=".join(f[1:])
            scene.setGlobal(name, val)

        # Set framerate
        self.options.fps = scene.getGlobal("fps", self.options.fps)

        # Set resolution
        if self._no_resolution_specified:
            w,h,a = self.getResolution()
            self.options.width = w
            self.options.height = h
            self.options.aspect = a

        # Camera
        self.options.camera = scene.getGlobal("camera", self.options.camera)

        # Set up direction
        if self.options.up!=None:
            up = self.options.up.lower()
            if up=="y":
                scene.up = (0,1,0)
            elif up=="z":
                scene.up = (0,0,1)
            else:
                raise ValueError, "Invalid 'up' direction: '%s' (should be 'y' or 'z')"%self.options.up

        # Set the frame rate
        timer.fps = self.options.fps

        # Determine end time
        self.time_end = None
        self.frame_end = None
        if self.options.end!=None:
            val, unit = self._parseTimeStr(self.options.end)
            if unit=="f":
                scene.timer().frame = val
            else:
                scene.timer().time = val
            self.time_end = timer.time
            self.frame_end = timer.frame

        # Determine start time (which will be set on the timer)
        self.time_start = 0.0
        self.frame_start = 0.0
        if self.options.begin!=None:
            val, unit = self._parseTimeStr(self.options.begin)
            if unit=="f":
                scene.timer().frame = val
            else:
                scene.timer().time = val
            self.time_start = timer.time
            self.frame_start = timer.frame

        # Set the timer to the start time...
        # (either by stepping there or by setting it directly)
        if self.options.set_time:
            # Set the time directly
            timer.time = self.time_start
        else:
            # Step to the start time (so that any simulation, etc. is properly
            # done)
            timer.time = 0
            while self.time_start-timer.time>1E-5:
                timer.step()

        if self.time_end==None:
            te = "<inf>"
            fe = "<inf>"
        else:
            te = "%1.2fs"%self.time_end
            fe = "%d"%self.frame_end
        if self.options.verbose:
            print "Time range: %1.2fs - %s (frames %d - %s)"%(self.time_start, te, self.frame_start, fe)


    # loadPlugins
    def loadPlugins(self):
        """Load plugins.

        The plugins specified in the environment variable CGKIT_PLUGIN_PATH
        and in the command line are loaded.
        """
        
        s = os.environ.get("CGKIT_PLUGIN_PATH", "")
        pluginsvar = splitPaths(s)
        
        lst = getattr(self.options, "plugin", [])
        s = ";".join(lst)
        s = s.replace(",", ";")
        pluginsopt = splitPaths(s)

        plugins = pluginsvar + pluginsopt

        descs = pluginmanager.importPlugins(plugins)
        for desc in descs:
            if desc.status!=pluginmanager.STATUS_OK:
                sys.stderr.write(70*"-"+"\n")
                sys.stderr.write('ERROR: Loading plugin "%s" failed:\n'%os.path.basename(desc.filename))
                sys.stderr.write("\n"+desc.traceback)
                sys.stderr.write(70*"-"+"\n")

    # getCamera
    def getCamera(self):
        """Get or create a camera object.
        """
        
        scene = getScene()

        cname = self.options.camera
        
        # Search for a camera...
        cam = None
        for obj in scene.walkWorld():
            prots = obj.protocols()
            if ICamera in prots:
                if obj.name==cname or cname==None :
                    cam = obj
                    break

        if cname!=None and cam==None:
            raise ValueError, 'Camera "%s" not found.'%cname

        # No camera? Then create a default camera...
        if cam==None:
            if self.options.verbose:
                print "No camera set, using a default camera."
            bbmin, bbmax = scene.boundingBox().getBounds()
            dif = bbmax-bbmin
            b1 = scene.up.ortho()
            b2 = scene.up.cross(b1)
            pos = dif.length()*(0.5*b1+b2) + (bbmax.z+0.5*dif.z)*scene.up
            if abs(dif.z)<0.0001:
                pos += 0.8*dif.length()*scene.up
            cam = TargetCamera(pos = pos,
                               target = 0.5*(bbmin+bbmax)-0.2*(dif.z*scene.up),
                               fov = 50)
        else:
            if self.options.verbose:
                print "Camera:",cam.name

        return cam

    # getResolution
    def getResolution(self):
        """Read the global resolution setting.

        Returns a tuple (width, height, pixel aspect)
        """
        res = getScene().getGlobal("resolution", (640,480))
        # If the resolution is a string, try to evaluate it
        if isinstance(res, basestring):
            try:
                res = eval(res)
            except:
                pass
        try:
            if len(res)==2:
                w,h = res
                aspect = 1
            elif len(res)==3:
                w,h,aspect = res
            else:
                raise Exception
        except:
            print >>sys.stderr, "Error: Invalid resolution setting:",res
            w,h,aspect = 640,480,1
        return w,h,aspect

    # translateKeyWordOpt
    def translateKeyWordOpt(self, opt, dic, msg):
        """Translates an option value from keyword to internal value.

        opt is the value of the option as specified by the user.
        dic is a dictionary that translates keywords to internal values.
        msg is an error message that will be passed to the ValueError
        exception. msg must contain the sub string "%s" that will be
        replaced with the invalid option value.
        """

        if opt==None:
            opt2 = None
        else:
            opt2 = opt.lower()
            
        if dic.has_key(opt2):
            return dic[opt2]
        else:
            raise ValueError, msg%opt

        
    ## protected:

    # _parseTimeStr
    def _parseTimeStr(self, s):
        """Split a string containing a time value into the value and unit.

        Returns a tuple (value, unit).
        """
        m = re.search("[a-z]*$", s)
        unit = s[m.start():]
        val = float(s[:m.start()])
        if unit=="":
            unit="f"
        if unit not in ["f", "s"]:
            raise ValueError, "Unknown time unit: '%s'"%unit
        return val,unit
    

