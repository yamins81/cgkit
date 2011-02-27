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
# $Id: gnuplotter.py,v 1.3 2005/03/31 17:11:56 mbaas Exp $

## \file gnuplotter.py
## Contains the GnuPlotter class.

import sys
import component
import eventmanager, events
from scene import getScene
from slots import *
from cgtypes import *
import _core
try:
    import Gnuplot
    gnuplot_installed = True
except:
    gnuplot_installed = False


class _PlotDesc:
    def __init__(self, slot, title):
        self.slot = slot
        self.title = title
        self.data = []

# GnuPlotter
class GnuPlotter(component.Component):
    """Graph plotter using gnuplot.
    
    """

    def __init__(self,
                 name = "GnuPlotter",
                 title = None,
                 xlabel = None,
                 ylabel = None,
                 xrange = None,
                 yrange = None,
                 inputs = 1,
                 plottitles = [],
                 starttime = 0.0,
                 endtime = 99999.0,
                 enabled = True,
                 auto_insert = True):
        """Constructor.
        """
        global gnuplot_installed
        
        component.Component.__init__(self, name, auto_insert)

        self.enabled = enabled
        self.inputs = inputs
        self.plot_data = []
        self.starttime = starttime
        self.endtime = endtime

        for i in range(inputs):
            s = DoubleSlot()
            exec "self.input%d_slot = s"%(i+1)
            if i<len(plottitles):
                t = plottitles[i]
            else:
                t = None
            pd = _PlotDesc(slot=s, title=t)
            self.plot_data.append(pd)

        if not enabled:
            return

        if not gnuplot_installed:
            print >>sys.stderr, "Warning: PyGnuplot is not installed"
            return

        self.gp = Gnuplot.Gnuplot()
        self.gp('set data style lines')
        self.gp("set grid")
        self.gp("set xzeroaxis")
        if title!=None:
            self.gp.title(title)
        if xlabel!=None:
            self.gp.xlabel(xlabel)
        if ylabel!=None:
            self.gp.ylabel(ylabel)
        if yrange!=None:
            self.gp.set_range("yrange",yrange)
#            self.gp("set yrange [%f:%f]"%tuple(yrange))
        if xrange!=None:
            self.gp.set_range("xrange",xrange)

        self._delay = 0

        eventmanager.eventManager().connect(events.STEP_FRAME, self)
        
    def onStepFrame(self):
        if not self.enabled:
            return
        
        t = getScene().timer().time
        if t<self.starttime or t>self.endtime:
            return

        for pd in self.plot_data:
            pd.data.append((t, pd.slot.getValue()))
        
        if self._delay==0:
            a = []
            for pd in self.plot_data:
                data = pd.data
                # There seems to be a bug in Gnuplot.py 1.7 that prevents
                # data arrays with only one item to be displayed. So the
                # following is a workaround (the point is just duplicated).
                if len(data)==1:
                    data = 2*data
                data = Gnuplot.Data(data, title=pd.title)
                a.append(data)
            self.gp.plot(*a)
            self._delay = 0
        else:
            self._delay -= 1
        

