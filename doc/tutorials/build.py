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
# Build tutorials

import sys, os, os.path, stat

# isNewer
def isNewer(file1, file2):
    """Check if file1 is newer than file2.

    Returns True if file1 is newer than file2 or if file2 doesn't exist.
    file1 must be an existing file.
    """
    if not os.path.exists(file2):
        return True
    return os.stat(file1)[stat.ST_MTIME]>os.stat(file2)[stat.ST_MTIME]

# build
def build(name, stylesheet="../default.css"):
    """Build a tutorial by converting RST into HTML."""
    prevdir = os.getcwd()
    
    dir,filename = os.path.split(name)
    os.chdir(dir)

    n,ext = os.path.splitext(filename)
    dest = n+".html"
    if isNewer(filename, dest):
        cmd = "python ../rst2html.py %s %s --stylesheet=%s --link-stylesheet"%(filename, dest, stylesheet)
#        cmd = "rst2html %s %s --stylesheet=%s"%(name, dest, stylesheet)
        print cmd
        os.system(cmd)

    os.chdir(prevdir)

######################################################################

#build("index.txt", stylesheet="default.css")
build("first_steps/first_steps.txt")
build("materials/materials.txt")
build("animation/animation.txt")
build("renderman/renderman.txt")
build("custom_vars/custom_vars.txt")
build("ogre/ogre.txt")
build("baking/baking.txt")
build("grow_utility/grow_utility.txt")
build("code_examples/demo1.txt")
build("code_examples/demo2.txt")
build("code_examples/demo3.txt")
build("code_examples/shownormals.txt")
build("code_examples/spacedevicedemo.txt")
