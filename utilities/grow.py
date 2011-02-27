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

######################################################################
# grow v1.0.1
#
# "Grow" objects on the surface of another object.
# This tool generates uniformly distributed points on the surface
# of a mesh and calls a user defined function for each such point.
#
# http://cgkit.sourceforge.net
######################################################################

"""
This tool generates uniformly distributed point locations on the surface
of one or more objects and calls a user defined function for each such
point.

Usage: grow.py [Options] <Inputfiles>

The script does the following steps:

1) Load all input files in the given order (you may use any 3D file format
   that cgkit can read).
2) Initialize the RenderMan API using the "RIBname" parameter.
3) Load the geometry procedure file. The name of this file is determined by
   the "proc" parameter.
4) Generate the surface points on all objects that match the string
   "inputpattern" and call "proc" for each point. The number of points
   is specified with the "numpoints" parameter.

Parameters and their default values (the parameters are stored in the
cgkit Globals section):

Globals(

    # Name of the placement procedure to use. The procedure must be a
    # function with the specified name that is defined in a file with
    # the same name plus suffix ".py". The name may contain path
    # information if the procedure file is not in the current directory.
    # (Command line option: -p/--proc)
    
    proc = "grassproc",

    # Only objects whose name matches this pattern will receive surface points.
    # By default, all objects are considered.
    # (Command line option: -i/--inputpattern)
    
    inputpattern = "*",

    # The total number of points to generate.
    # (Command line option: -n/--numpoints)
    
    numpoints = 1000,

    # RIB output file name. If none is given, the output is written to stdout.
    # (Command line option: -r/--rib)
    
    RIBname = None,

    # Declare the result values of the placement procedure (only required when
    # the procedure wants the main program to call RiCurve()).
    # If the placement procedure returns None, then the procedure takes care
    # for the RIB output itself.
    
    resultdesc = ["P"]
    
)
"""

import sys, os.path, copy, time, optparse, fnmatch, random
import profile
from cgkit.all import *
from cgkit.ri import *

# VaryingVar
class VaryingVar:
    """Access a varying primitive variable.
    """
    
    def __init__(self, name, type, mult=1):
        """Constructor.

        name is the name of the primitive variable, type its type
        (FLOAT, NORMAL, ...) and mult its multiplicity.
        """

        self.name = name
        self.type = type
        self.mult = mult
        self.obj = None
        # The corresponding slot (this is already type checked)
        self.slot = None

        # The current three values
        self.values = None

    # init1
    def init1(self, obj):
        """Initialization part I.

        obj is the WorldObject whose variable should be accessed
        (actually the geom's variable).
        This method might change the geom and convert the variable
        to varying.
        """

        self.slot = None
        self.obj = None

        info = obj.geom.findVariable(self.name)
        # Var not found?
        if info==None:
            return False

        # Check the type
        if info[2]!=self.type:
            print >>sys.stderr, 'Variable "%s": Invalid type'%self.name
            return False

        # Check the multiplicity
        if info[3]!=self.mult:
            print >>sys.stderr, 'Variable "%s": Invalid multiplicty (required %d, got %d)'%(self.name, self.mult, info[3])
            return False

        # Is the variable stored as var/varfaces? Then convert to varying...
        finfo = obj.geom.findVariable("%sfaces"%self.name)
        if finfo!=None:
            obj.geom = convMeshAttr(obj.geom, self.name)
            # Update the var info...
            info = obj.geom.findVariable(self.name)

        # Facevarying? then convert to varying
        if info[1]==FACEVARYING:
            obj.geom = convFaceVarToVar(obj.geom)
            # Update the var info...
            info = obj.geom.findVariable(self.name)

        # Is it really varying?
        if info[1]!=VARYING:
            print >>sys.stderr, 'Variable "%s": Invalid storage class'%self.name
            return False

        self.obj = obj
        return True

    # init2
    def init2(self):
        """Initialization part II.

        Retrieve the slot that holds the variable values. Once this
        method is called, no init1() method may be called anymore
        (no matter if it's this or another variable) because this
        could change the geom so that the obtained slot is no longer valid.
        """
        if self.obj!=None:
            # Store the slot
            self.slot = self.obj.geom.slot(self.name)

    # retrieveValues
    def retrieveValues(self, i, j, k):
        """Get the three values at index i, j, k.
        """
        slot = self.slot
        self.values = (slot[i], slot[j], slot[k])

    # interpolateValue
    def interpolateValue(self, u, v, w):
        """
        """
        v1,v2,v3 = self.values
        return w*v1 + u*v2 + v*v3
        

# SurfacePointGenerator
class SurfacePointGenerator:
    """Generate random points on a TriMesh.

    The points are uniformly distributed over the entire mesh.

    Usage:

    srf = SurfacePointGenerator(obj, num)
    for t in srf.triangles():
        for P,N,s,t in srf.surfacePoints(t):
            ...do something with P and N, for example:...
            drawMarker(P, size=0.01)
            drawLine(P, P+0.1*N)
            ...this is called num times...
    
    """
    
    def __init__(self, obj):
        """Constructor.
        
        obj: A list of TriMeshes
        """
        
        self.obj = worldObject(obj)
        self._area = 0
        self._triangles = []
        
        self._preprocess()

    # getArea
    def getArea(self):
        """Return the surface area of the object."""
        return self._area

    # triangles
    def triangles(self, num):
        """Iterate over all triangles.

        This method yields a tuple that describes one triangle.
        This tuple can be passed to surfacePoints() to iterate over
        a random number of points on that surface.

        num: The total number of surface points to be generated
        """
        A = self._area
        facecount = 0
        # "Error" made in assigning points to triangles (if one triangle
        # didn't receive any points, the chances for the next triangle
        # will be increased. This prevents clustering at the end of the
        # triangle list when only a few total points should be created)
        err = 0.0
        for a,b,c,Na,Nb,Nc,sta,stb,stc,Af in self._triangles:
            facecount+=1
#            print >>sys.stderr, "\015%d/%d"%(facecount, len(obj.faces)),
        
            # Calculate the number of points n for this triangle...
            n = int(round(num*Af/A + err))
            err += (num*Af/A)-n
            A -= Af
            num -= n

            if n==0:
                continue

            yield facecount,n,a,b,c,Na,Nb,Nc,sta,stb,stc

    # surfacePoints
    def surfacePoints(self, trispec):
        """Iterate over a random number of points on a triangle.

        Yields a tuple (P, N) where P is the position on the surface
        and N the (interpolated) surface normal.
        """
        facecount,n,a,b,c,Na,Nb,Nc,sta,stb,stc = trispec
        sa,ta = sta
        sb,tb = stb
        sc,tc = stc

        rnd = random.random
        ab = b-a
        ac = c-a
        for i in range(n):
            # Select a random position on the triangle
            while 1:
                u = rnd()
                v = rnd()
                if u+v<1.0:
                    break
            w = 1.0-u-v

            # P: Grass blade position   N: Interpolated normal
            P = a + u*ab + v*ac
            N = (w*Na + u*Nb + v*Nc).normalize()
            s = w*sa + u*sb + v*sc
            t = w*ta + u*tb + v*tc
            yield P,N,s,t
        

    # _preprocess
    def _preprocess(self):
        """Preprocessing the object.

        Computes self._triangles, self._area
        """

        varN = VaryingVar("N", NORMAL)
        varN.init1(self.obj)
                 
        varst = VaryingVar("st", FLOAT, 2)
        varst.init1(self.obj)

        varN.init2()
        varst.init2()
        N_slot = varN.slot
        st_slot = varst.slot

        # Extract triangles and determine surface area A of obj    
        triangles = []
        A = 0.0
        obj = self.obj
        L = obj.worldtransform
        L3 = L.getMat3()
        sta = [0.0, 0.0]
        stb = [0.0, 0.0]
        stc = [0.0, 0.0]
        for i,j,k in obj.faces:
            # Get vertices
            a = L*obj.verts[i]
            b = L*obj.verts[j]
            c = L*obj.verts[k]
            # Get the normals
            if N_slot!=None:
                Na = (L3*N_slot[i]).normalize()
                Nb = (L3*N_slot[j]).normalize()
                Nc = (L3*N_slot[k]).normalize()
            else:
                ab = b-a
                ac = c-a
                Na = (ab.cross(ac)).normalize()
                Nb = Na
                Nc = Na
            # Get the tex coords
            if st_slot!=None:
                sta = st_slot[i]
                stb = st_slot[j]
                stc = st_slot[k]
            # Surface area of this triangle
            Af = 0.5*((b-a).cross(c-a)).length()
            # Add to total surface area
            A += Af
            # Store vertices and triangle surface area for later use
            triangles.append((a,b,c, Na,Nb,Nc, sta,stb,stc, Af))

        self._triangles = triangles
        self._area = A
        print >>sys.stderr, "Surface area:",A
        
######################################################################  

# time2str
def time2str(t):
    """Convert a time value (in seconds) into a string.
    """
    if t<60:
        return "%1.0fs"%t
    else:
        min = int(t/60)
        sec = int(t%60)
        if min<60:
            return "%dmin %ds"%(min,sec)
        else:
            h = int(min/60)
            min = int(min%60)
            return "%dh %dmin %ds"%(h,min,sec)

# grow
def grow(objs, proc, num=10, resultdesc=["P"]):
    """Grow objects on the given object (TriMesh).
    
    """

    srfs = []
    totalarea = 0.0
    for obj in objs:
        print >>sys.stderr, 'Preprocessing object "%s"...'%obj.name
        srf = SurfacePointGenerator(obj)
        srfs.append(srf)
        totalarea += srf.getArea()

    print >>sys.stderr, "Total area:",totalarea

    # Determine the number of points for each object...
    nums = []
    err = 0.0
    for srf in srfs:
        Aobj = srf.getArea()
        # Calculate the number of points n for this object...
        n = int(round(num*Aobj/totalarea + err))
        nums.append(n)
        err += (num*Aobj/totalarea)-n
        totalarea -= Aobj
        num -= n
        
    total_n = 0
    for obj,srf,num in zip(objs, srfs, nums):
        print >>sys.stderr, 'Generating %d points on "%s"...'%(num,obj.name)
        for tri in srf.triangles(num):
            n = tri[1]
            total_n += n
            print >>sys.stderr, "\015%d..."%total_n,

            # Create a list with len(resultdesc) lists (one list per variable)
            valuelsts = []
            for i in range(len(resultdesc)):
                valuelsts.append([])

            # Iterate over the surface points...
            for P,N,s,t in srf.surfacePoints(tri):
                F = mat4(1).lookAt(P, P+N, up=vec3(0,0,1))
                res = proc(P=P, N=N, F=F, s=s, t=t)
                if res!=None:
                    for Lst, lst in zip(valuelsts, res):
                        Lst += lst

            if res!=None:
                params = {}
                for name, values in zip(resultdesc, valuelsts):
                    params[name] = values
                RiCurves(RI_CUBIC, n*[6], RI_NONPERIODIC, params)
                
        print >>sys.stderr, ""


######################################################################

# Default values
Globals(
    proc = "grassproc",
    inputpattern = "*",
    numpoints = 1000,
    RIBname = None,
    resultdesc = ["P"]
)

# Parse the command line
usage = "usage: %prog [options] inputfiles"
parser = optparse.OptionParser(usage)
parser.add_option("-p", "--proc", metavar="NAME", default=None,
                  help="Select the procedure")
parser.add_option("-n", "--numpoints", type="int", default=None,
                  help="Number of points to generate")
parser.add_option("-r", "--rib", metavar="NAME", default=None,
                  help="Output RIB name")
parser.add_option("-i", "--inputpattern", metavar="PATTERN", default=None,
                  help="Input pattern used for matching object names")
opts, args = parser.parse_args()

# No scene files? then exit
if len(args)==0:
    parser.print_help()
    sys.exit(0)

scene = getScene()

# Load the geometry...
for filename in args:
    print >>sys.stderr, 'Loading "%s"...'%filename
    load(filename)

# Initialize ri
RIBname = scene.getGlobal("RIBname")
if opts.rib!=None:
    RIBname = opts.rib

RiBegin(RIBname)
RiOption(RI_RIBOUTPUT, RI_VERSION, 0)

# Load the procedure file
procname = scene.getGlobal("proc")
if opts.proc!=None:
    procname = opts.proc
procfilename = "%s.py"%procname
print >>sys.stderr, 'Loading procedure file "%s"...'%procfilename
execfile(procfilename)

# Get a reference to the procedure
procname = os.path.basename(procname)
proc = eval("%s"%procname)

inputpattern = scene.getGlobal("inputpattern")
if opts.inputpattern!=None:
    inputpattern = opts.inputpattern
print >>sys.stderr, 'Input pattern: "%s"'%inputpattern

# Collect all geometry (convert to TriMesh if possible)...
meshes = []
for obj in scene.walkWorld():
    if obj.geom==None:
        continue
    if not fnmatch.fnmatch(obj.name, inputpattern):
        continue
    if not isinstance(obj.geom, TriMeshGeom):
        try:
            convertToTriMesh(obj)
#        except NotImplementedError:
        except:
            continue
    meshes.append(obj)

# No meshes? then exit
if len(meshes)==0:
    print >>sys.stderr, "No meshes available"
    sys.exit(1)

# Generate the surface points...

numpoints = scene.getGlobal("numpoints")
if opts.numpoints!=None:
    numpoints = opts.numpoints

desc = scene.getGlobal("resultdesc")

print >>sys.stderr, 'Output RIB file: "%s"'%RIBname
print >>sys.stderr, '#Surface points:',numpoints

t1 = time.time()
grow(objs=meshes, proc=proc, num=numpoints, resultdesc=desc)
#profile.run("grow(objs=meshes, proc=proc, num=numpoints, resultdesc=desc)", "profile.log")
t2 = time.time()
dt = t2-t1
print >>sys.stderr, "Generation time: %s"%time2str(dt)

RiEnd()



    
