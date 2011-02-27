######################################################################
# Analyse debug output
######################################################################
# Create the debug output by setting the debug_flag to True (see __init__.py)
# and calling "viewer.py <inputfile> 2>err.log".
#
# checkdebuginfo alloc - Checks allocations/deallocations
#
# checkdebuginfo deps - Check dependency cleanup
#
# checkdebuginfo dot >dot.txt - Create a dot file (dot -Tpng dot.txt -odot.png)
# 
######################################################################

import sys, os, optparse, random, colorsys, fnmatch

# InfoReader
class InfoReader:
    def __init__(self, filename):
        self.filename = filename
        self.fhandle = file(filename)
        # A dictionary with class "names". Key is the address as string.
        self.objectname = {}
        self.slotname = {}

    def read(self):
        self.onBegin()
        for s in self.fhandle:
            n = s.find(":")
            if n!=-1:
                addr = s[:n]
                info = s[n+1:].strip()
                n = info.find("::")
                classname = info[:n]
                info = info[n+2:]
                n = info.find("(")
                methodname = info[:n]
                args = self.getArgs(info[n:])
                self.findInstanceName(addr, classname, methodname, args)
                self.line = s.strip()
                self.onMethodCall(addr, classname, methodname, args)
        self.onEnd()

    def getArgs(self, s):
        res = []
        bracket = 0
        arg = ""
        for c in s:
            if c=='(':
                bracket+=1
                arg=""
            elif c==')':
                bracket-=1
                if bracket==0:
                    res.append(arg.strip())
                    break
            elif c==',' and bracket==1:
                res.append(arg.strip())
                arg=""
            else:
                arg+=c

        return res

    # instanceName
    def instanceName(self, addr):
        """Return the name of an instance.

        addr is the address of the object.
        """
        if addr in self.slotname:
            return self.slotname[addr]
        elif addr in self.objectname:
            return self.objectname[addr]
        else:
            return "?_"+addr

    def findInstanceName(self, addr, classname, methodname, args):
        rawclassname = classname.split("<")[0]
        # Constructor?
        if rawclassname==methodname:
            if addr not in self.objectname:
                name = "%s(%s)"%(methodname, ", ".join(args))
                # Check if the component name already exists and add the
                # address if it does
                if classname=="Component":
                    for k in self.objectname:
                        if self.objectname[k]==name:
                            name = name+addr
                            break
                self.objectname[addr] = name
        # addSlot()?
        elif methodname=="addSlot":
            slotname, slotaddr = args
            slotname = slotname[1:-1]
            compname = self.objectname[addr]
            self.slotname[slotaddr] = compname+"."+slotname
        

    def onMethodCall(self, addr, classname, methodname, args):
        """Method callback.

        This callback is called for every method call.

        addr is the address of the object whose method was called.
        classname is the name of the class and methodname the name
        of the method that was called. args is a list of arguments.

        The current line is in self.line
        """
        pass

    def onBegin(self):
        """Initial callback.

        This callback is called before reading starts.
        """
        pass

    def onEnd(self):
        """Final callback.

        This callback is called after the file was read.
        """
        pass

######################################################################

# AllocDeallocChecker
class AllocDeallocChecker(InfoReader):
    """Checks if allocs/deallocs balances.

    This class checks constructor/destructor calls to see if every
    object created is also deleted in the end.
    """
    
    def __init__(self, filename):
        InfoReader.__init__(self, filename)

        self.numobjects = 0
        self.instances = {}

    def onMethodCall(self, addr, classname, methodname, args):
        classname = classname.split("<")[0]
        if methodname==classname:
            if addr not in self.instances:
                self.instances[addr] = 0
                self.numobjects += 1
            self.instances[addr]+=1
        elif methodname=="~"+classname:
            if addr not in self.instances:
                print "%s: %s: Destructor without constructor!"%(addr, classname)
            else:
                self.instances[addr]-=1
                if self.instances[addr]==0:
                    del self.instances[addr]

    def onEnd(self):
        for k in self.instances:
            print "Object %s at %s not deleted"%(self.instanceName(k), k)
        print self.numobjects, "objects created,",len(self.instances), "objects not deleted!"

######################################################################

class CheckDependencies(InfoReader):
    """Checks dependencies.

    This class checks if established dependencies are removed in the end.
    """
    
    def __init__(self, filename):
        InfoReader.__init__(self, filename)
        # Key: Addr  Value:List of dependents
        self.deps = {}

    def onMethodCall(self, addr, classname, methodname, args):
        if classname=="Slot<T>":
            if methodname=="addDependent":
                if addr not in self.deps:
                    self.deps[addr] = []
                if args[0] in self.deps[addr]:
                    print "Object %s is added to %s twice!"%(args[0], addr)
                self.deps[addr].append(args[0])
            elif methodname=="removeDependent":
                if addr not in self.deps:
                    print addr, "removeDependent() on empty list!"
                try:
                    self.deps[addr].remove(args[0])
                except ValueError:
                    print "%s: removeDependent(%s) without addDependent()!"%(addr, args[0])
                    print "  %s: %s"%(addr, self.instanceName(addr))
                    print "  %s: %s"%(args[0], self.instanceName(args[0]))

    def onEnd(self):
        for k in self.deps:
            if self.deps[k]!=[]:
                print "%s: %d dependencies not removed:"%(k, len(self.deps[k])),
                print self.deps[k]
                print "  %s: %s"%(k, self.instanceName(k))
                for a in self.deps[k]:
                    print "  %s: %s"%(a, self.instanceName(a))
                    
######################################################################
                    
class MakeDot(InfoReader):
    """Creates a dot description file of the dependency graph.

    checkdebuginfo.py dot >dot.txt
    dot -Tpng dot.txt -odot.png
    """
    def __init__(self, filename):
        InfoReader.__init__(self, filename)
        # Key: Addr  Value:List of dependents
        self.deps = {}
        
    def onMethodCall(self, addr, classname, methodname, args):
        if methodname=="addDependent":
            if addr not in self.deps:
                self.deps[addr] = []
            if args[0] in self.deps[addr]:
                print "Object %s is added to %s twice!"%(args[0], addr)
            self.deps[addr].append(args[0])

    def onEnd(self):

        # The colors for the components
        colors = ["oldlace","lightblue", "lightpink", "palegreen",
                  "lightgray", "plum", "lightgoldenrod", "tan", "bisque"]

#        ignored = ["WorldRoot_inertiatensor", "WorldRoot_cog",
#                   "WorldRoot_totalmass", "WorldRoot_worldtransform"]
        ignored = ["WorldRoot_*", "TargetCamera_*", "_lookat_*"]
#        ignored = []

        # Remove dependencies from/to ignored slots
        remove = []
        for addr in self.deps:
            # Is addr an ignored slot?
            name = self.instanceName(addr)
            name = self.instanceNameToNodeName(name)
            removed = False
            for pattern in ignored:
                if fnmatch.fnmatch(name, pattern):
                    remove.append(addr)
                    removed = True
                    break
            if removed:
                continue

            # Does the dependencies go to ignored slots?
            newdeps = []
            for da in self.deps[addr]:
                name = self.instanceName(da)
                name = self.instanceNameToNodeName(name)
                ok = True
                for pattern in ignored:
                    if fnmatch.fnmatch(name, pattern):
                        ok = False
                        break
                if ok:
                    newdeps.append(da)
            self.deps[addr] = newdeps
            if newdeps==[]:
                remove.append(addr)
        for a in remove:
            del self.deps[a]

        # Make a dictionary that contains all objects present in this graph...
        # -> nodes
        nodes = {}
        for addr in self.deps:
            nodes[addr] = 1
            for da in self.deps[addr]:
                nodes[da] = 1

        # Output the graph...
        
        print "digraph G {"

        # "Declare" all nodes and set their attributes...
        cols = {}
        for a in nodes:
            name = self.instanceName(a)
            name = self.instanceNameToNodeName(name)
            comp = "_".join(name.split("_")[:-1])
            if comp not in cols:
                if len(colors)>0:
                    col = colors[0]
                    del colors[0]
                else:
                    col = "white"
                cols[comp] = col
            else:
                col = cols[comp]
                
            n = name.rfind("_")
            if n!=-1:
                label = name[:n]+"."+name[n+1:]
            else:
                label = name
            
            print '  %s [style=filled, fillcolor=%s, label="%s\\n(%s)"]'%(name, col, label, a)

        print ""

        # Output the edges...
        for addr in self.deps:
            for da in self.deps[addr]:
                name1 = self.instanceName(da)
                name2 = self.instanceName(addr)
                name1 = self.instanceNameToNodeName(name1)
                name2 = self.instanceNameToNodeName(name2)
                print "  %s -> %s;"%(name2, name1)
        print "}"

    def instanceNameToNodeName(self, name):
        """Convert an instance name to a valid dot node name."""
        name = name.replace(" ", "")
        name = name.replace("Component", "")
        name = name.replace("(", "").replace(")", "")
        name = name.replace("\"", "")
        name = name.replace(".", "_")
        name = name.replace("?", "_")
        name = name.replace(",", "_")
        name = name.replace("=", "_")
        return name
        
######################################################################

# AddrTranslator
class AddrTranslator(InfoReader):
    """

    """
    
    def __init__(self, filename):
        InfoReader.__init__(self, filename)
        
        self.prev_instance = ""

        self.addrs = {}

    def onMethodCall(self, addr, classname, methodname, args):
        inst = self.instanceName(addr)
        self.addrs[addr]=inst
        if inst!=self.prev_instance:
            print 70*"-"
            print "INSTANCE:",self.instanceName(addr)
            print ""
            self.prev_instance = inst
        print self.line

    def onEnd(self):
        print "\nWriting file err_names.log..."
        f = file("err_names.log", "w")
        for a in self.addrs:
            print >>f, a, "->", self.addrs[a]
        f.close()


######################################################################

cmd = ""

usage = """usage: %prog [options] command

commands:  alloc, deps, dot"""

parser = optparse.OptionParser(usage)
parser.add_option("-l", "--logfile", metavar="NAME", help="The name of the debug info file (default: err.log)", default="err.log")
opts, args = parser.parse_args()

if len(args)>0:
    cmd = args[0]
    
if cmd=="alloc":
    reader = AllocDeallocChecker(opts.logfile)
elif cmd=="deps":
    reader = CheckDependencies(opts.logfile)
elif cmd=="dot":
    reader = MakeDot(opts.logfile)
elif cmd=="translate":
    reader = AddrTranslator(opts.logfile)
else:
    parser.print_help()
    sys.exit(1)
    
reader.read()
