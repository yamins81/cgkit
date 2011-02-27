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

import sys, optparse, Image

# shift
def shift(img, dx, dy):
    """Return an image shifted by dx,dy pixels.
    """
    w,h = img.size
    if dx<0:
        x1 = -dx
        x2 = w
        xd = 0
    else:
        x1 = 0
        x2 = w-dx
        xd = dx
        
    if dy<0:
        y1 = -dy
        y2 = h
        yd = 0
    else:
        y1 = 0
        y2 = h-dy
        yd = dy
    clp = img.crop((x1,y1,x2,y2))
    res = Image.new(img.mode, img.size)
    res.paste(clp, (xd,yd))
    return res

# composite
def composite(img1, img2):
    """Composite two images.

    img2 is on top of img1.
    """
    r,g,b,a = img2.split()
    img1 = img1.convert("RGB")
    img2 = img2.convert("RGB")
    res = Image.composite(img2, img1, a)
    return res

    
######################################################################

usage = "usage: %prog [options] inputimage outputimage"
optparser = optparse.OptionParser(usage)
opts, args = optparser.parse_args()

if len(args)<2:
    optparser.print_help()
    sys.exit()

print 'Reading "%s"...'%args[0]
img = Image.open(args[0])

if img.mode!="RGBA":
    print 'Invalid image mode %s (must be RGBA)'%img.mode
    sys.exit(1)

r = 1
bg = Image.new("RGB", img.size)
bg = composite(bg, shift(img, -r, -r))
bg = composite(bg, shift(img, r,-r))
bg = composite(bg, shift(img, -r,r))
bg = composite(bg, shift(img, r,r))

outimg = composite(bg, img)
print 'Writing "%s"...'%args[1]
outimg.save(args[1])
