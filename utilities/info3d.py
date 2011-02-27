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
# Display info about a 3D scene.
#
# Usage: info3d [-v/--values] <filename>
######################################################################

import optparse
from cgkit.all import *
from cgkit import _core
from cgkit.sl import *


# objInfo
def objInfo(obj):
    """Print infos about obj.
    """
    print 70*"-"
    if obj.geom!=None:
        g = obj.geom.__class__.__name__
        numgeomslots = obj.geom.numSlots()
    else:
        g = "-"
        numgeomslots = 0

    if obj.parent==None:
        parentname = "<none>"
    else:
        parentname = '"'+obj.parent.name+'"'

    materialnames = []
    for i in range(obj.getNumMaterials()):
        mat = obj.getMaterial(i)
        if mat==None:
            materialnames.append("None")
        else:
            materialnames.append('"'+mat.name+'"')
        
    print 'Name       : "%s"'%obj.name
    print "Type       : %s"%obj.__class__.__name__
    print "Geom       : %s"%g
    print 'Parent     : %s'%parentname
    print "Pos        : (%1.3f, %1.3f, %1.3f)"%tuple(obj.pos)
    print "Rot        : (%1.2f, %1.2f, %1.2f)"%tuple(map(lambda x: degrees(x), obj.rot.toEulerXYZ()))
    print "Scale      : (%1.3f, %1.3f, %1.3f)"%tuple(obj.scale)
    print "Visible    :",obj.visible
    print "Mass       :",obj.mass
    print "Total mass :",obj.totalmass
    print "COG        :",obj.cog
    print "#Materials : %d (%s)"%(obj.getNumMaterials(), ", ".join(materialnames))
    print "#Childs    :",len(list(obj.iterChilds()))
    print "#Slots     :",obj.numSlots()
    print "#Geom slots:",numgeomslots

    print "\nSlots:"
    printSlots(obj)

    if obj.geom!=None:
        print "\nPrimitive variables:"
        for v in obj.geom.iterVariables():
            name = v[0]
            storage = str(v[1]).lower()
            typ = str(v[2]).lower()
            mult = v[3]
            slot = obj.geom.slot(name)
            size = slot.size()
            print "  %-11s %-6s %-20s %5s %8d elements"%(storage, typ, name, "["+str(mult)+"]", size)

        print "\nGeom slots:"
        printSlots(obj.geom)

def printSlots(obj):
    """Display the slots of component obj.
    """
    for s in obj.iterSlots():
        slot = obj.slot(s)
        if hasattr(slot, "size"):
            info = "%s, %d items"%(slot.__class__.__name__, slot.size())
            print "  %s (%s)"%(s, info)
        else:
            try:
                v = getattr(obj, s)
            except:
                v = "?"
            if isinstance(v, _core.mat3) or isinstance(v, _core.mat4):
                v = list(v)
            info = "%s"%(slot.__class__.__name__)
            print "  %s = %s (%s)"%(s, v, info)
    
                
def variableValues(obj):
    """Show the values of all primitive variables.
    """
    if obj.geom==None:
        return

    for var in obj.geom.iterVariables():
        slot = obj.geom.slot(var[0])
        print '\nVariable: "%s"'%var[0]
        for i,v in enumerate(slot):
            print " %5d %s"%(i,v)

def slotValues(obj):
    """Show the values of all slots.

    obj can be any Component object.
    """
    if obj==None:
        return
    
    for slotname in obj.iterSlots():
        slot = obj.slot(slotname)
        # Array slot?
        if hasattr(slot, "size"):
            print '\nSlot: "%s"'%slotname
            for i,v in enumerate(slot):
                print " %5d %s"%(i,v)
        else:
            pass
#            print slot.getValue()
            
    

######################################################################
parser = optparse.OptionParser()
parser.add_option("-v", "--values", action="store_true", default=False,
                  help="Show primitive variable values")
parser.add_option("-n", "--name", metavar="NAME", default=None,
                  help="Only show infos of object with the specified name")
options, args = parser.parse_args()

load(args[0])
listWorld()

for obj in getScene().walkWorld():
    if options.name!=None and obj.name!=options.name:
        continue
    objInfo(obj)
    if options.values:
        slotValues(obj)
        slotValues(obj.geom)
#        variableValues(obj)
