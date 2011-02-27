# Render using Blender

import os, os.path, subprocess, sys, traceback
import cgkit.jobqueue

class blender(cgkit.jobqueue.JobProc):
    """Render a Blender file.
    """
    
    def __init__(self, blendFile=None, output=None, frames=None, format=None,
                 addExtension=None, sceneName=None, threads=None,
                 blenderCmd = "blender",
                 label=None):
        """Constructor.
        
        blendFile: The file to render (option -b)
        output: The output path and name (option -o).
        frames: This is either a single int to render a single frame, a tuple
                (start,end) to render a frame range or None to render everything.
                The start or the end may be None to indicate the start or end
                of the entire animation.
        format: A string describing the output format (option -F).
        addExtension: A boolean indicating whether a file extension should be added
                or not (option -x).
        sceneName: The scene name (option -S)
        threads: The number of threads that should be used for rendering (option -t).
                 0=system processor count.
        blenderCmd: The name of the Blender executable
        label: The GUI label string
        """
        # Check the parameter types...
        if not isinstance(blendFile, basestring):
            raise TypeError("blendFile argument must be a string, got a %s instead"%getattr(type(blendFile), "__name__", "?"))
        if not (output is None or isinstance(output, basestring)):
            raise TypeError("output argument must be a string or None, got a %s instead"%getattr(type(output), "__name__", "?"))
        if not (format is None or isinstance(format, basestring)):
            raise TypeError("format argument must be a string or None, got a %s instead"%getattr(type(format), "__name__", "?"))
        if not (addExtension is None or type(addExtension) is bool):
            raise TypeError("format argument must be a string or None, got a %s instead"%getattr(type(addExtension), "__name__", "?"))
        if not (sceneName is None or isinstance(sceneName, basestring)):
            raise TypeError("sceneName argument must be a string or None, got a %s instead"%getattr(type(sceneName), "__name__", "?"))
        if not (threads is None or type(threads) is int):
            raise TypeError("threads argument must be an int or None, got a %s instead"%getattr(type(threads), "__name__", "?"))
        if not isinstance(blenderCmd, basestring):
            raise TypeError("blenderCmd argument must be a string, got a %s instead"%getattr(type(blenderCmd), "__name__", "?"))
        if frames is None:
            startFrame = None
            endFrame = None
        elif type(frames) is int:
            startFrame = frames
            endFrame = frames
        else:
            try:
                s,e = frames
            except:
                raise TypeError("frames argument must be an int, two ints or None")
            if not (s is None or type(s) is int):
                raise TypeError("The start frame must be an int or None, got a %s instead"%getattr(type(s), "__name__", "?"))
            if not (e is None or type(e) is int):
                raise TypeError("The end frame must be an int or None, got a %s instead"%getattr(type(e), "__name__", "?"))
            startFrame = s
            endFrame = e
        
        # Set the default label if no label was given
        if label is None:
            label = 'Blender "%s"'%os.path.basename(blendFile)
            
        cgkit.jobqueue.JobProc.__init__(self, label=label)
        
        # Store the parameters
        self._blenderExec = blenderCmd
        self._blendFile = blendFile
        self._output = output
        self._startFrame = startFrame
        self._endFrame = endFrame
        self._format = format
        self._addExtension = addExtension
        self._sceneName = sceneName
        self._threads = threads
        
        self._currentFrame = None
        
        self._hasError = False
        self._errorLine = ""
        
    def run(self):
        """Run Blender.
        """
        cmd = self.createCommandLine()
        print >>self.stdout, cmd
        ret = self.execCmd(cmd)
        if ret!=0:
            print >>self.stderr, "\nReturn code:",ret
            self._hasError = True
            self._errorLine = "Return code: %s"%ret
        
        if self._hasError:
            self.setError()
            self.setStatusLine(self._errorLine)
        else:
            self.setStatusLine("Done")
            
            
    def stdoutCallback(self, lineNr, line):
        """Scan stdout for errors.
        """
        self.setStatusLine(line.strip())
        if line.startswith("ERROR:") and not self._hasError:
            print >>self.stderr, "stdout, line %s:\n%s"%(lineNr+1, line)
            print >>self.stderr, "(see stdout for more details)"
            self._hasError = True
            self._errorLine = line
        elif line.startswith("Fra:"):
            try:
                frame = int(line[4:].split()[0])
            except:
                frame = self._currentFrame
                
            if frame!=self._currentFrame:
                self._currentFrame = frame
                if self._startFrame is not None and self._endFrame is not None:
                    pc = 100*(frame-self._startFrame)/(self._endFrame-self._startFrame+1)
                    self.setProgress(pc)
    
    def stderrCallback(self, lineNr, line):
        """Scan stderr for errors.
        """
        if not self._hasError and line.strip()!="":
            self._hasError = True
        
    def createCommandLine(self):
        """Return the command line for invoking Blender.
        """
        toks = [self._blenderExec]
        toks.append('-b "%s"'%self._blendFile)
        if self._sceneName is not None:
            toks.append('-S "%s"'%self._sceneName)
        if self._output is not None:
            toks.append('-o "%s"'%self._output)
        if self._addExtension is not None:
            toks.append("-x %d"%int(self._addExtension))
        if self._format is not None:
            toks.append("-F %s"%self._format)
        if self._threads is not None:
            toks.append("-t %s"%self._threads)
        if self._startFrame is not None and self._startFrame==self._endFrame:
            toks.append("-f %d"%self._startFrame)
        else:
            if self._startFrame is not None:
                toks.append("-s %s"%self._startFrame)
            if self._endFrame is not None:
                toks.append("-e %s"%self._endFrame)
            toks.append("-a")
            
        return " ".join(toks)
