#!/usr/bin/env python
# Create PyDoc documentation
# (pydoc.py must be somewhere on PATH)

import sys, os, os.path
from stat import *


# isNewer
def isNewer(file1,file2):
    """Check if file1 is newer than file2.
    """
    if not os.path.exists(file1):
        return True
    if not os.path.exists(file2):
        return True
    return os.stat(file1)[ST_MTIME]>os.stat(file2)[ST_MTIME]

######################################################################

# The files or directories that have to be passed to pydoc
pydoc_args = ["cgkit._core"]


for dirpath, dirnames, filenames in os.walk("cgkit"):
    # Is the current directory a package?
    if os.path.exists(os.path.join(dirpath, "__init__.py")):
        pydoc_args.append(dirpath)
    for name in filenames:
        n,ext = os.path.splitext(name)
        if ext==".py" and n!="__init__":
            mod = os.path.join(dirpath, n)
            pydoc_args.append(mod)

# Call pydoc
os.environ["PYTHONPATH"]=os.getcwd()
if not os.path.exists("html_pydoc"):
    os.mkdir("html_pydoc")
os.chdir("html_pydoc")
pydoc = os.path.join(sys.prefix, "lib", "pydoc.py")
print pydoc                     
for name in pydoc_args:
    name = name.replace(os.path.sep, ".")
    cmd = "%s -w %s"%(pydoc, name)
    htmlname = name+".html"
    srcname = os.path.join("..", name.replace(".", os.path.sep)+".py")
    if isNewer(srcname, htmlname):
        print cmd
        os.system(cmd)
