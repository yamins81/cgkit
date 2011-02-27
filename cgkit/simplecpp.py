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
# $Id: simplecpp.py,v 1.2 2006/01/27 22:11:09 mbaas Exp $

import sys, types, os.path

class Context:
    """This class carries information about a particular source file.
    
    Each #include command produces a new context that gets pushed
    onto the stack.
    """
    def __init__(self, filehandle):
        # The file handle (file-like object)
        self.filehandle = filehandle
        # The current line number (i.e. the line number of the next line read)
        self.linenr = 1
        # The line number where the current (continued) line started.
        # This is only different from self.linenr when lines were continued
        # with a backslash at the end of a line. In this case start_linenr
        # is the line number where the long line began and linenr is the
        # last line number of the long line.
        self.start_linenr = 1
        # The file name
        self.filename = getattr(filehandle, "name", "<?>")
        
        # This is basically a stack that contains the evaluated conditions
        # of the #if, #ifdef, etc. commands
        self.condition_list = []
        # Flag that specifies whether a line should be ignored or not
        # (because it is inside an #ifdef ... #endif block, for example).
        # If this flag is False there is still be output generated, but
        # it's just an empty string)
        self.output_line = True
        

# PreProcessor
class PreProcessor:
    """A simple 'preprocessor'.

    This class implements a subset of the functionality of the C preprocessor.
    """

    def __init__(self, defines=None, includedirs=None):
        """Constructor.
        
        defines is a list of tuples (name, value) that specify the initial set
        of defined macros.
        includedirs is a list of strings that is used to find include files.
        """

        # The error stream
        self.errstream = sys.stderr
        
        # Flag that specifies if the starting position of the next line is
        # already inside a comment (i.e. if there was a '/*' without a '*/'
        self.inside_comment = False
        
        # A 'stack' of Context objects
        self.context_stack = []
        # Current context (this is the topmost element from context_stack)
        self.context = None
        
        # Contains the defined macros.
        self.defined = {}
        # This is a sorted list containing the macros (longest names first)
        # The items are tuples (name, value).
        self.substitution_list = []
        
        self.output_buffer = []

        # Set the predefined macros
        if defines!=None:
            for name,val in defines:
                if val==None:
                    val = ""
                self.onDefine("define", "%s %s"%(name, str(val)))

        # Set the include directories
        self.includedirs = []
        if includedirs!=None:
            self.includedirs.extend(includedirs)
           
        # This flag can be used to abort the preprocessor
        self.abort_flag = False
        
    def __call__(self, source, errstream=None):
        """Preprocess a file or a file-like object.

        source may either be the name of a file or a file-like object.
        """
        
        self.errstream = errstream
        
        if isinstance(source, types.StringTypes):
            fhandle = file(source, "rt")
        else:
            fhandle = source

        self.output_buffer = []
        self.preprocess(fhandle)
        return "\n".join(self.output_buffer)

    def preprocess(self, fhandle):
        """Preprocess a source file.
        """
        
        self.context_stack = []
        self.context = None
        ctx = Context(fhandle)
        self.readFile(ctx)
        
    def readFile(self, ctx):
        """Read another file.
        
        ctx is a initialized Context object.
        """
        
        self.pushContext(ctx)
        linebuffer = ""
        defined = self.defined
        
        for line in ctx.filehandle:
            if line[-1:]=='\n':
                line = line[:-1]
            # Append the line to the line buffer...
            if line[-1:]=='\\':
                linebuffer += line[:-1]
                # Read another line before processing the line
                self.context.linenr += 1
                continue
            else:
                linebuffer += line
                
            intervals = list(self.iterStringIntervals(linebuffer))
            out = "".join(intervals)
                 
            stripped = out.strip()
            # Preprocessor directive?
            if stripped[:1]=="#":
                # Invoke the appropriate directive handler method...
                # (i.e. onInclude(), onIfdef(), ...) 
                s = stripped[1:].strip()
                a = s.split()
                directive = a[0].lower()
                handlername = "on%s"%directive.capitalize()
                arg = s[len(directive)+1:].strip()
                handler = getattr(self, handlername, None)
                if handler!=None:
                    handler(directive, arg)
            # no preprocessor directive but regular code...
            else:
                out = ""
                for s in intervals:
                    # Do macro substitutions if it's not a string
                    if s[0:]!='"':
                        for name,value in self.substitution_list:
                            s = s.replace(name, value)
                    out += s

                if ctx.output_line:
                    self.output(out)
                else:
                    self.output("")
                    
            # Should preprocessing be aborted?
            if self.abort_flag:
                break
            
            self.context.linenr += 1
            self.context.start_linenr = self.context.linenr
            linebuffer = ""
            
        self.popContext()

    def output(self, s):
        """This method is called for every (extended) output line.
        
        Lines that were split using '\' at the end of the line are
        reported as one single line.
        """
        self.output_buffer.append(s)
            
    def abort(self):
        """Tell the preprocessor to abort operation.
        
        This method can be called by a derived class in the output()
        handler method.
        """
        self.abort_flag = True
        
    # Directive handler methods:
    # Each method takes the lowercase directive name as input and
    # the argument string (which is everything following the directive)
    # Example: #include <spam.h> -> directive: "include"  arg: "<spam.h>"
            
    def onInclude(self, directive, arg):
        """Handle #include directives.
        """
        filename = arg[1:-1]
        fullfilename = self.findFile(filename)
        if fullfilename==None:
            ctx = self.context
            print >>self.errstream, "%s:%d: %s: No such file or directory"%(ctx.filename, ctx.linenr, filename)
            return

        f = file(fullfilename, "rt")
        ctx = Context(f)
        self.readFile(ctx)
        
    def onDefine(self, directive, arg):
        """Handle #define directives.
        """
        a = arg.split()
        if len(a)==0:
            ctx = self.context
            print >>self.errstream, "%s:%d: Invalid macro definition"%(ctx.filename, ctx.linenr)
            return
        name = a[0]
        value = " ".join(a[1:])
        self.defined[name] = value
        self.substitution_list.append((name, value))
        self.substitution_list.sort(lambda a,b: cmp(len(b[0]), len(a[0])))

    def onUndef(self, directive, arg):
        """Handle #undef directives.
        """
        if arg=="":
            ctx = self.context
            print >>self.errstream, "%s:%d: Invalid macro name"%(ctx.filename, ctx.linenr)
            return
        if arg not in self.defined:
            return
        
        del self.defined[arg]
        # Remove the macro from the substitution list
        lst = []
        for name,value in self.substitution_list:
            if name!=arg:
                lst.append((name,value))
        self.substitution_list = lst
        
    def onIf(self, directive, arg):
        """Handle #if directives.
        """
        try:
            cond = bool(int(arg))
        except:
            cond = False
        self.context.condition_list.append(cond)
        self.context.output_line = cond
        
    def onIfdef(self, directive, arg):
        """Handle #ifdef directives.
        """
        a = arg.split()
        if len(a)==0:
            ctx = self.context
            print >>self.errstream, "%s:%d: '#ifdef' with no argument"%(ctx.filename, ctx.linenr)
            return

        name = a[0]
        cond = self.defined.has_key(name)
        self.context.condition_list.append(cond)
        self.context.output_line = cond

    def onIfndef(self, directive, arg):
        """Handle #ifndef directives.
        """
        a = arg.split()
        if len(a)==0:
            ctx = self.context
            print >>self.errstream, "%s:%d: '#ifndef' with no argument"%(ctx.filename, ctx.linenr)
            return
        
        name = a[0]
        cond = not self.defined.has_key(name)
        self.context.condition_list.append(cond)
        self.context.output_line = cond

    def onElif(self, directive, arg):
        """Handle #elif directives.
        """
        try:
            cond = bool(int(arg))
        except:
            cond = False
        self.context.output_line = cond
        
    def onElse(self, directive, arg):
        """Handle #else directives.
        """
        self.context.output_line = not self.context.output_line
                
    def onEndif(self, directive, arg):
        """Handle #endif directives.
        """
        if len(self.context.condition_list)==0:
            print >>self.errstream, "%s:%d: unbalanced '#endif'"%(self.context.filename, self.context.linenr)
            return
        
        self.context.condition_list.pop()
        self.context.output_line = True
        
    def onPragma(self, directive, arg):
        """Handle #pragma directives.
        """
        pass

    def pushContext(self, ctx):
        """Push a context object onto the context stack.
        """
        self.context_stack.append(ctx)
        self.context = ctx
        self.output('# %d "%s"'%(ctx.linenr, ctx.filename))
        
    def popContext(self):
        """Pop the topmost context object from the context stack.
        """
        ctx = self.context_stack.pop()
        if len(self.context_stack)==0:
            self.context = None
        else:
            self.context = self.context_stack[-1]
            self.output('# %d "%s"'%(self.context.linenr, self.context.filename))
            self.output("")
        return ctx

    def findFile(self, filename):
        """Search for an include file and return its name.

        The returned file name is guaranteed to refer to an existing file.
        If the file is not found None is returned.
        """
        
        if os.path.exists(filename):
            return filename

        for incdir in self.includedirs:
            name = os.path.join(incdir, filename)
            if os.path.exists(name):
                return name

        return None
        
    def iterStringIntervals(self, s):
        """Iterate over the separate 'intervals' in the given string.
        
        This iterator splits the string into several intervals and yields
        each interval. The intervals can be one of the following:
        
        - Content: Anything outside a comment and outside a string
        - String: Always begins with '"'
        - Comment: Is replaced by blanks (or not yielded at all)
        
        Example string: 
        
          'result = myfunc(5, "text" /* comment */, 6) // Comment'
          
        Intervals:
        
        - 'result = myfunc(5, '
        - '"text"'
        - ' '
        - '             '
        - ', 6) '
        """

        slength = len(s)

        start = 0
        while start<slength:
        
            if self.inside_comment:
                # Search for the end of the comment
                n = s.find('*/', start)
                if n==-1:
                    start = slength
                else:
                    self.inside_comment = False
                    start = n+2
            else:
                n1 = s.find('//', start)
                n2 = s.find('/*', start)
                n3 = s.find('"', start)

                # Determine which of the above strings is encountered first...
                if n1==-1:
                    if n2==-1:
                        if n3==-1:
                            # n1==-1 and n2==-1 and n3==-1
                            first = 0
                        else:
                            # n1==-1 and n2==-1 and n3!=-1
                            first = 3
                    else:
                        if n3==-1:
                            # n1==-1 and n2!=-1 and n3==-1
                            first = 2
                        else:
                            # n1==-1 and n2!=-1 and n3!=-1
                            if n2<n3:
                                first = 2
                            else:
                                first = 3
                else:
                    if n2==-1:
                        if n3==-1:
                            # n1!=-1 and n2==-1 and n3==-1
                            first = 1
                        else:
                            # n1!=-1 and n2==-1 and n3!=-1
                            if n1<n3:
                                first = 1
                            else:
                                first = 3
                    else:
                        if n3==-1:
                            # n1!=-1 and n2!=-1 and n3==-1
                            if n1<n2:
                                first = 1
                            else:
                                first = 2
                        else:
                            # n1!=-1 and n2!=-1 and n3!=-1
                            if n1<n2 and n1<n3:
                                first = 1
                            else:
                                if n2<n3:
                                    first = 2
                                else:
                                    first = 3

                # Neither a comment, nor quotes
                if first==0:
                    # The entire remaining string is content
                    yield s[start:]
                    start = slength
                # '//' comment
                elif first==1:
                    if n1>start:
                        yield s[start:n1]
                    start = slength
                # '/*' comment
                elif first==2:
                    if n2>start:
                        yield s[start:n2]
                    # Search for the closing comment
                    n = s.find('*/', n2+2)
                    if n==-1:
                        self.inside_comment = True
                        start = slength
                    else:
                        yield (n-n2+2)*" "
                        start = n+2
                # '"'
                else:
                    if n3>start:
                        yield s[start:n3]
                    # Search for the closing apostrophes and yield the string
                    n = s.find('"', n3+1)
                    if n==-1:
                        # No closing apostrophes in the same line? This is
                        # actually an error
                        yield s[n3:]
                        start = slength
                    else:
                        n += 1
                        yield s[n3:n]
                        start = n
                
                        
                    
                    
    

######################################################################

if __name__=="__main__":
    import optparse
    
    op = optparse.OptionParser()
    op.add_option("-D", "--define", action="append", default=[],
                 help="Define a symbol")    
    op.add_option("-I", "--includedir", action="append", default=[],
                 help="Specify include paths")    
    
    opts, args = op.parse_args()
    p = PreProcessor(includedirs=opts.includedir, defines=map(lambda x: (x,None), opts.define))
    if len(args)==0:
        print p(sys.stdin)
    else:
        try:
            print p(file(args[0]))
        except IOError, e:
            print e
