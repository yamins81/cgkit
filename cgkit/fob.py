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
# $Id: fob.py,v 1.2 2005/04/29 15:10:22 mbaas Exp $


import sys, string, time, struct
from cgtypes import *
from math import pi
try:
    import serial
    serial_installed = True
except:
    serial_installed = False

# Commands
ANGLES = 'W'
ANGLE_ALIGN1 = 'J'
ANGLE_ALIGN2 = 'q'
BORESIGHT = 'u'
BORESIGHT_REMOVE = 'v'
BUTTON_MODE = 'M'
BUTTON_READ = 'N'
CHANGE_VALUE = 'P'
EXAMINE_VALUE = 'O'
FBB_RESET = '/'
HEMISPHERE = 'L'
MATRIX = 'X'
METAL = 's'
METAL_ERROR = 't'
NEXT_TRANSMITTER = '0'
OFFSET = 'K'
POINT = 'B'
POSITION = 'V'
POSITION_ANGLES = 'Y'
POSITION_MATRIX = 'Z'
POSITION_QUATERNION = ']'
QUATERNION = '\\'
REFERENCE_FRAME1 = 'H'
REFERENCE_FRAME2 = 'r'
REPORT_RATE1 = 'Q'
REPORT_RATE2 = 'R'
REPORT_RATE8 = 'S'
REPORT_RATE32 = 'T'
RUN = 'F'
SLEEP = 'G'
STREAM = '@'
STREAM_STOP = '?'
SYNC = 'A'
XOFF = 0x13
XON = 0x11

# Change value/Examine Value parameters
BIRD_STATUS = chr(0x00)
SOFTWARE_REVISION_NUMBER = chr(0x01)
BIRD_COMPUTER_CRYSTAL_SPEED = chr(0x02)
POSITION_SCALING = chr(0x03)
BIRD_MEASUREMENT_RATE = chr(0x07)
SYSTEM_MODEL_IDENTIFICATION = chr(0x0f)
FLOCK_SYSTEM_STATUS = chr(0x24)
FBB_AUTO_CONFIGURATION = chr(0x32)

# Hemispheres
FORWARD = chr(0) + chr(0)
AFT =  chr(0) + chr(1)
UPPER = chr(0xc) + chr(1)
LOWER = chr(0xc) + chr(0)
LEFT = chr(6) + chr(1)
RIGHT = chr(6) + chr(0)

class DataError(Exception):
    """Exception.

    This exception is thrown when the number of bytes returned from the bird
    doesn't match what is expected."""
    pass

def hexdump(data):
    """data als Hex-Dump ausgeben."""
    if len(data)>16:
        hexdump(data[:16])
        hexdump(data[16:])
    else:
        t = ""
        for c in data:
            s = hex(ord(c))[2:]
            if len(s)==1:
                s="0"+s
            print s,
            if c in string.printable and c not in string.whitespace:
                t+=c
            else:
                t+="."
        print (48-3*len(data))*" ",
        print t

class FOBRecord:
    def __init__(self):
        self.pos = (0.0, 0.0, 0.0)
        self.angles = (0,0,0)
        self.matrix = (0,0,0,0,0,0,0,0,0)
        self.quat = (0,0,0,0)
        self.metal = None

# BirdContext
class BirdContext:
    """Context class for one individual bird (sensor).

    This class stores the output mode in which a particular bird is
    currently in.  This information is used to decode the data record
    sent by the bird.
    """
    def __init__(self):

        # 144 for extended range transmitter (see p. 89 of the FOB manual)
        # 2.54 to convert inches into cm
        self.pos_scale = 144*2.54

        # Mode (ANGLE, POSITION, ...)
        self.mode = POSITION_ANGLES
        # Size of data records (default is POSITION/ANGLES)
        self.record_size = 12
        # Number of words in one record (record_size = 2*num_words [+ metal])
        self.num_words = 6
        self.scales = [self.pos_scale, self.pos_scale, self.pos_scale, 180, 180, 180]
        self.metal_flag = False

    def angles(self, addr=None):
        self.mode = ANGLES
        self.num_words = 3
        self.scales = [180,180,180]
        self._calc_record_size()

    def position(self):
        self.mode = POSITION
        self.num_words = 3
        self.scales = [self.pos_scale, self.pos_scale, self.pos_scale]
        self._calc_record_size()

    def position_angles(self, addr=None):
        """POSITION/ANGLES command."""
        self.mode = POSITION_ANGLES
        self.num_words = 6
        self.scales = [self.pos_scale, self.pos_scale, self.pos_scale,180,180,180]
        self._calc_record_size()

    def matrix(self, addr=None):
        self.mode = MATRIX
        self.num_words = 9
        self.scales = [1,1,1,1,1,1,1,1,1]
        self._calc_record_size()

    def position_matrix(self, addr=None):
        """POSITION/MATRIX command."""
        self.mode = POSITION_MATRIX
        self.num_words = 12
        self.scales = [self.pos_scale, self.pos_scale, self.pos_scale,1,1,1,1,1,1,1,1,1]
        self._calc_record_size()

    def quaternion(self, addr=None):
        """QUATERNION command."""
        self.mode = QUATERNION
        self.num_words = 4
        self.scales = [1,1,1,1]
        self._calc_record_size()

    def position_quaternion(self, addr=None):
        """POSITION/QUATERNION command."""
        self.mode = POSITION_QUATERNION
        self.num_words = 7
        self.scales = [self.pos_scale, self.pos_scale, self.pos_scale,1,1,1,1]
        self._calc_record_size()

    def metal(self, flag):
        self.metal_flag = flag
        self._calc_record_size()

    def decode(self, s):
        values = []
        for i in range(self.num_words):
            v = self.decodeWord(s[2*i:2*i+2])
            v*=self.scales[i]
            v=v/32768.0
            values.append(v)

#        if self.metal_flag:
#            print "Metal:",ord(s[-1])

        return values
        

    def decodeWord(self, s):
        """Decode the word in string s.

        s must contain exactly 2 bytes.
        """

        ls = ord(s[0]) & 0x7f
        ms = ord(s[1])
        if ms&0x80==0x80:
            print "MSB bit7 nicht 0!"
        v = (ms<<9) | (ls<<2)
        if v<0x8000:
            return v
        else:
            return v-0x10000

    def _calc_record_size(self):
        self.record_size = 2*self.num_words
#        if self.metal_flag:
#            self.record_size+=1
        
        

# FOB
class FOB:
    """Interface to the Ascension Flock Of Birds.

    This class can be used to communicate with the Ascension Flock
    of Birds motion tracker (http://www.ascension-tech.com/).
    The class directly accesses the serial port and has methods
    that correspond to the commands described in the \em Flock \em of
    \em Birds \em Installation \em and \em Operation \em Guide.

    Example usage:

    \code
    fob = FOB()
    # Open a connection on the serial port
    fob.open()
    # Initialize
    fob.fbbAutoConfig(numbirds=4)
    fob.fbbReset()
    ...

    # Set output mode for bird 1 to "position"
    fob.position(1)
    
    # Obtain a sensor value from bird 1
    pos = fob.point(1)
    ...
    
    fob.close()
    \endcode
    """
    def __init__(self):
        """Constructor."""
        # Serial() instance
        self.ser = None

        # Default-Context (Master)
        self.defaultctx = BirdContext()
        # A list of bird context classes (one per sensor)        
        self.birds = []

    def __str__(self):
        if self.ser==None:
            serstr = "not connected"
        else:
            serstr = "port:%s, baudrate:%d"%(self.ser.portstr, self.ser.baudrate)
        s = "Flock of Birds (%s):\n"%serstr
        s += '  Device Description: "%s"\n'%self.examineValue(SYSTEM_MODEL_IDENTIFICATION)
        status = self.examineValue(FLOCK_SYSTEM_STATUS)
        for i in range(14):
            s += "  Bird %2d: "%i
            b = status[i]
            if b&0x01:
                s += "ERT#0, "
            if b&0x02:
                s += "ERT#1, "
            if b&0x04:
                s += "ERT#2, "
            if b&0x08:
                s += "ERT#3, "
            if b&0x10:
                s += "Extended Range Controller, "
            if b&0x80==0:
                s += "not "
            s += "accessible, "
            if b&0x40==0:
                s += "not "
            s += "running, "
            if b&0x20:
                s += "has a sensor"
            else:
                s += "has no sensor"
            if i<14:
                s += "\n"
            
#        c,dev,deps = self.examineValue(FBB_AUTO_CONFIGURATION)
#        for i in range(14):
#            s += "  Bird %2d: "%i
#            if dev&(1<<i):
#                s += "running, "
#            else:
#                s += "not running, "
#            if deps&(1<<i):
#                s += "dependent\n"
#            else:
#                s += "not dependent\n"
        return s
    
    # open
    def open(self, port=0, baudrate=115200, timeout=3):
        """Open a connection to the flock.

        \param port (\c int) Port number (0,1,...)
        \param baudrate (\c int) Baud rate
        \param timeout (\c float) Time out value for RS232 operations
        """
        if not serial_installed:
            raise RuntimeError, 'Cannot import the serial module (http://pyserial.sourceforge.net).'
        
        self.ser = serial.Serial(port=port,
                                 baudrate=baudrate,
                                 bytesize=serial.EIGHTBITS,
                                 parity=serial.PARITY_NONE,
                                 stopbits=serial.STOPBITS_ONE,
                                 xonxoff=0,
                                 rtscts=0,
                                 timeout=timeout)
        self.ser.setRTS(0)
        ser = self.ser
#        print "Port:",ser.portstr
#        print "Baudrates:",ser.BAUDRATES
#        print "Bytesizes:",ser.BYTESIZES
#        print "Parities:",ser.PARITIES
#        print "Stopbits:",ser.STOPBITS
#        print "CTS:",ser.getCTS()
#        print "DSR:",ser.getDSR()
#        print "RI:",ser.getRI()
#        print "CD:",ser.getCD()
#        self.ser.setRTS(1)
#        time.sleep(1.0)
#        self.ser.setRTS(0)

    # close
    def close(self):
        if self.ser!=None:
            self.ser.close()
            self.ser = None

    def examineValue(self, param, addr=None):
        """EXAMINE VALUE command.

        Here are the possible parameters and their return values

        - BIRD_STATUS: Integer (actually a word)
        - SOFTWARE_REVISION_NUMBER: Tuple (int,fra). The version number is int.fra
        - BIRD_COMPUTER_CRYSTAL_SPEED: Frequeny in MHz as an integer
        - BIRD_MEASUREMENT_RATE: Measurement rate in cycles/sec (as float)
        - SYSTEM_MODEL_IDENTIFICATION: Device description string
        - FLOCK_SYSTEM_STATUS: 14-tuple with the status byte for each bird
        - FBB_AUTO_CONFIGURATION: Tuple (configmode, devices, dependents)

        \param param Parameter number (there are symbolic constants)
        \return Value
        """
        if addr!=None:
            self.rs232ToFbb(addr)            
        self._write(EXAMINE_VALUE+param)
        # BIRD_STATUS
        if param==BIRD_STATUS:
            v = self._read(2, exc=True)
            res = struct.unpack("<H", v)[0]
        # SOFTWARE_REVISION_NUMBER
        elif param==SOFTWARE_REVISION_NUMBER:
            v = self._read(2, exc=True)
            int = struct.unpack("B", v[1])[0]
            fra = struct.unpack("B", v[0])[0]
            res = (int, fra)
        # BIRD_COMPUTER_CRYSTAL_SPEED
        elif param==BIRD_COMPUTER_CRYSTAL_SPEED:
            v = self._read(2, exc=True)
            res = struct.unpack("B", v[0])[0]
        # BIRD_MEASUREMENT_RATE
        elif param==BIRD_MEASUREMENT_RATE:
            v = self._read(2, exc=True)
            rate = struct.unpack("<H", v)[0]
            res = float(rate)/256.0
        # SYSTEM_MODEL_IDENTIFICATION
        elif param==SYSTEM_MODEL_IDENTIFICATION:
            v = self._read(10, exc=True)
            res = v.strip()
        # FLOCK_SYSTEM_STATUS
        elif param==FLOCK_SYSTEM_STATUS:
            v = self._read(14, exc=True)
            res = struct.unpack("BBBBBBBBBBBBBB", v)
        # FBB AUTO-CONFIGURATION
        elif param==FBB_AUTO_CONFIGURATION:
            v = self._read(5, exc=True)
            configmode = struct.unpack("B", v[0])[0]
            dev = struct.unpack("<H", v[1:3])[0]
            deps = struct.unpack("<H", v[3:])[0]
            res = (configmode, dev, deps)
        else:
            raise ValueError, 'Unknown parameter: %s'%param

        return res


    def fbbReset(self):
        """FBB RESET command.

        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        self._write(FBB_RESET)

    def angles(self, addr=None):
        """ANGLES command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(ANGLES, addr)
        ctx.angles()

    def position(self, addr=None):
        """POSITION command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(POSITION, addr)
        ctx.position()

    def position_angles(self, addr=None):
        """POSITION/ANGLES command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(POSITION_ANGLES, addr)
        ctx.position_angles()

    def matrix(self, addr=None):
        """MATRIX command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(MATRIX, addr)
        ctx.matrix()

    def position_matrix(self, addr=None):
        """POSITION/MATRIX command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(POSITION_MATRIX, addr)
        ctx.position_matrix()

    def quaternion(self, addr=None):
        """QUATERNION command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(QUATERNION, addr)
        ctx.quaternion()

    def position_quaternion(self, addr=None):
        """POSITION/QUATERNION command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        ctx = self._switch_mode(POSITION_QUATERNION, addr)
        ctx.position_quaternion()        

    def point(self, addr=None):
        """POINT command.

        \param addr Bird address (0,1,2...).
        \return A list of sensor values (the number of values depends on the output mode).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        if addr==None:
            ctx = self.defaultctx
        else:
            self.rs232ToFbb(addr)
            ctx = self.birds[addr]

        self._write(POINT)
        while 1:
            record = self._read()
            if record=="":
                print "FOB: No feedback from the ERC"
                return
            if ord(record[0])&0x80:
                break
            print "FOB: Bit 7 not set"
            
        record += self._read(ctx.record_size-1)
        if len(record)!=ctx.record_size:
            print "Wrong number of bytes read (%d instead of %d)"%(len(record), ctx.record_size)
            return None
#        hexdump(res)

        res = ctx.decode(record)
#        print res
        return res

    def hemisphere(self, hemisphere, addr=None):
        """HEMISPHERE command.

        \param hemisphere The hemisphere to use (one of FORWARD, AFT, UPPER, LOWER, LEFT, RIGHT)
        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        if addr!=None:
            self.rs232ToFbb(addr)

        self._write(HEMISPHERE+hemisphere)            

    def run(self):
        """RUN command.

        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        self._write(RUN)

    def sleep(self):
        """SLEEP command.

        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        self._write(SLEEP)

    def metal(self, flag, data, addr=None):
        """METAL command.

        \param flag (\c int) Flag (see Flock of Birds manual)
        \param data (\c int) Data (see Flock of Birds manual)
        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        if addr==None:
            ctx = self.defaultctx
        else:
            self.rs232ToFbb(addr)
            ctx = self.birds[addr]
        self._write(METAL+chr(flag)+chr(data))
        if flag==0 and data==0:
            ctx.metal(False)
        else:
            ctx.metal(True)

    def metal_error(self, addr=None):
        """METAL ERROR command.

        \param addr Bird address (0,1,2...).
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        """
        if addr==None:
            ctx = self.defaultctx
        else:
            self.rs232ToFbb(addr)
            ctx = self.birds[addr]

        self._write(METAL_ERROR)               
        met = self._read(1)
        if len(met)!=1:
            print "Metal error byte nicht erhalten"
            return
        print "Metal error:",ord(met[0])
        
    def fbbAutoConfig(self, numbirds):
        """FBB AUTO-CONFIGURATION command.

        Send the FBB AUTO-CONFIGURATION command (via CHANGE VALUE).

        \param numbirds (\c int) The number of birds to use (this is the parameter byte that the FBB AUTO-CONFIGURATION command expects)
        \see Flock of Birds Installation and Operation Guide, chapter 7: \em RS232 \em Commands
        \see Flock of Birds Installation and Operation Guide, chapter 8: \em Change \em Value / \em Examine \em Value
        """
        time.sleep(1.0)
        self._write(CHANGE_VALUE+FBB_AUTO_CONFIGURATION+chr(numbirds))
        time.sleep(1.0)
        # First element is not used (addr 0 does not exist)
        self.birds = [None]
        for i in range(numbirds):
            self.birds.append(BirdContext())
#        res = self._read(2)
#        print len(res)

    def rs232ToFbb(self, addr):
        """Address a particular bird.

        Only for normal addressing mode!
        This method has to be called before the actual command is issued.

        \param addr (\c int) Bird address (1-14)
        """

        self._write(chr(0xf0+addr))

    def _switch_mode(self, mode, addr=None):
        """Generic mode switch command.

        mode is one of ANGLES, POSITION, MATRIX, QUATERNION, POSITION_ANGLES,
        POSITION_MATRIX, POSITION_QUATERNION.
        addr is the bird addr or None.
        The return value is the corresponding bird context. The calling
        function must still call the appropriate mode switch method on the
        context.

        \param mode Output mode
        \param addr Bird address (0,1,2...)
        \return A bird context class (BirdContext).
        """
        if addr==None:
            ctx = self.defaultctx
        else:
            self.rs232ToFbb(addr)
            ctx = self.birds[addr]
        self._write(mode)
        return ctx
        
    def _write(self, s):
        """Send string s to the Bird."""
#        print 70*"-"
#        print "Host->Bird:"
#        hexdump(s)
        self.ser.write(s)

    def _read(self, size=1, exc=False):
        """Receive size bytes of data from the bird.

        If the timeout value is exceeded an empty string is returned.

        \param size (\c int) Number of bytes to read.
        \param exc (\c bool) If True an exception is thrown when the number of read bytes does not match
        \return String containing the received bytes (or empty string).
        """
        v = self.ser.read(size)
        if exc and len(v)!=size:
            raise DataError, 'Expected %d bytes from the bird, but got %d'%(size, len(v))
        return v

#    def _decode(self, s):
#        values = []
#        for i in range(self.num_words):
#            v = self._decodeWord(s[2*i:2*i+2])
#            v*=self.scales[i]
#            v=v/32768.0
#            values.append(v)

#        return values
        

#    def _decodeWord(self, s):
#        """Decode the word in string s.

#        s must contain exactly 2 bytes.
#        """

#        ls = ord(s[0]) & 0x7f
#        ms = ord(s[1])
#        if ms&0x80==0x80:
#            print "MSB bit7 nicht 0!"
#        v = (ms<<9) | (ls<<2)
#        if v<0x8000:
#            return v
#        else:
#            return v-0x10000 
        

#    def _readAll(self):
#        buf = ""
#        while 1:
#            s = self._read()
#            if s=="":
#                break
#            buf+=s
#        print 70*"-"
#        print "Bird->Host:"
#        hexdump(buf)

    

