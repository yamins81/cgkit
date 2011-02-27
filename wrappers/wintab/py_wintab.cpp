/*
  Wintab wrapper
 */

#include <boost/python.hpp>
#include <iostream>
#include <exception>

#include <windows.h>
#include "wintab.h"

// Set all bits to get the maximum size of the PACKET structure
#define PACKETDATA 0xffffffff
#include "pktdef.h"

using namespace boost::python;

// Exception class
class WintabException : public std::exception
{
  public:
  std::string msg;

  WintabException(std::string amsg="") : msg(amsg) {}
  ~WintabException() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

// Exception class
class OutOfMemory : public std::exception
{
  public:
  OutOfMemory() {}
  ~OutOfMemory() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return "Out of memory";
  }
};


/**
  Packet class.
 */
class Packet
{
  public:
  WTPKT pktdata;

  HCTX context;
  UINT status;
  LONG time;
  WTPKT changed;
  UINT serial;
  UINT cursor;
  DWORD buttons;
  DWORD x;
  DWORD y;
  DWORD z;
  UINT normalpressure;
  UINT tangentpressure;
  int orient_azimuth;
  int orient_altitude;
  int orient_twist;
  int rot_pitch;
  int rot_roll;
  int rot_yaw;

  public:
  Packet() 
   : pktdata(0), context(0), status(0), time(0), changed(0), serial(0), 
     cursor(0),
     buttons(0), x(0), y(0), z(0), normalpressure(0), tangentpressure(0),
     orient_azimuth(0), orient_altitude(0), orient_twist(0),
     rot_pitch(0), rot_roll(0), rot_yaw(0)
   {}

  boost::python::str __repr__()
  {
    boost::python::str res = "<Packet";
    if (pktdata & PK_CONTEXT)
      res += " ctx:"+boost::python::str(long(context));
    if (pktdata & PK_STATUS)
      res += " status:"+boost::python::str(status);
    if (pktdata & PK_TIME)
      res += " time:"+boost::python::str(time);
    if (pktdata & PK_CHANGED)
      res += " chg:"+boost::python::str(changed);
    if (pktdata & PK_SERIAL_NUMBER)
      res += " serial:"+boost::python::str(serial);
    if (pktdata & PK_CURSOR)
      res += " csr:"+boost::python::str(cursor);
    if (pktdata & PK_BUTTONS)
      res += " btns:"+boost::python::str(buttons);
    if (pktdata & PK_X)
      res += " x:"+boost::python::str(x);
    if (pktdata & PK_Y)
      res += " y:"+boost::python::str(y);
    if (pktdata & PK_Z)
      res += " z:"+boost::python::str(z);
    if (pktdata & PK_NORMAL_PRESSURE)
      res += " npress:"+boost::python::str(normalpressure);
    if (pktdata & PK_TANGENT_PRESSURE)
      res += " tpress:"+boost::python::str(tangentpressure);
    if (pktdata & PK_ORIENTATION)
    {
      res += " azimuth:"+boost::python::str(orient_azimuth);
      res += " alt:"+boost::python::str(orient_altitude);
      res += " twist:"+boost::python::str(orient_twist);
    }
    if (pktdata & PK_ROTATION)
    {
      res += " pitch:"+boost::python::str(rot_pitch);
      res += " roll:"+boost::python::str(rot_roll);
      res += " yaw:"+boost::python::str(rot_yaw);
    }
    res+=">";
    return res;
  }

  /**
     Initialize the packet attributes.

     data contains the pktdata value determining the data that is
     stored in the buffer pointed to by packet. If a bit in data is
     not set, the corresponding attribute is left unchanged.
     The method returns the number of bytes consumed.
   */
  int init(WTPKT data, void* packet)
  {
    long* p = (long*)packet;
    pktdata = data;
    if (pktdata & PK_CONTEXT)
    {
      context = (HCTX)(*p);
      p+=1 ;
    }
    if (pktdata & PK_STATUS)
    {
      status = *p;
      p+=1 ;
    }
    if (pktdata & PK_TIME)
    {
      time = *p;
      p+=1 ;
    }
    if (pktdata & PK_CHANGED)
    {
      changed = *p;
      p+=1 ;
    }
    if (pktdata & PK_SERIAL_NUMBER)
    {
      serial = *p;
      p+=1 ;
    }
    if (pktdata & PK_CURSOR)
    {
      cursor = *p;
      p+=1 ;
    }
    if (pktdata & PK_BUTTONS)
    {
      buttons = *p;
      p+=1 ;
    }
    if (pktdata & PK_X)
    {
      x = *p;
      p+=1 ;
    }
    if (pktdata & PK_Y)
    {
      y = *p;
      p+=1 ;
    }
    if (pktdata & PK_Z)
    {
      z = *p;
      p+=1 ;
    }
    if (pktdata & PK_NORMAL_PRESSURE)
    {
      normalpressure = *p;
      p+=1 ;
    }
    if (pktdata & PK_TANGENT_PRESSURE)
    {
      tangentpressure = *p;
      p+=1 ;
    }
    if (pktdata & PK_ORIENTATION)
    {
      orient_azimuth = p[0];
      orient_altitude = p[1];
      orient_twist = p[2];
      p+=3 ;
    }
    if (pktdata & PK_ROTATION)
    {
      rot_pitch = p[0];
      rot_roll = p[1];
      rot_yaw = p[2];
      p+=3 ;
    }
    return (char*)p-(char*)packet;
  }
};

/**
  Device context class.
 */
class Context
{
  // Context descriptor
  LOGCONTEXT context;

  // Context handle
  HCTX handle;

  // Store the lcPktData attribute at the time open() was called
  long pktdata;
  HWND winhandle;

  // A permanent buffer for packet data, etc.
  void* buffer;
  long bufsize;

  public:
  // Message IDs
  int id_packet;
  int id_csrchange;
  int id_ctxopen;
  int id_ctxclose;
  int id_ctxupdate;
  int id_ctxoverlap;
  int id_proximity;
  int id_infochange;

  public:
  Context()
    : handle(0),
      pktdata(0),
      winhandle(0),
      buffer(0),
      bufsize(0),
      id_packet(WT_PACKET),
      id_csrchange(WT_CSRCHANGE),
      id_ctxopen(WT_CTXOPEN),
      id_ctxclose(WT_CTXCLOSE),
      id_ctxupdate(WT_CTXUPDATE),
      id_ctxoverlap(WT_CTXOVERLAP),
      id_proximity(WT_PROXIMITY),
      id_infochange(WT_INFOCHANGE)      
  {
    // Initialize the context structure with the default values
    WTInfo(WTI_DEFCONTEXT, 0, &context);
  }

  ~Context()
  {
    close();
    freeBuffer();
  }

  /**
     Open a context with the current settings.
   */
  void open(long hwnd, bool enable)
  {
    close();

    winhandle = (HWND)hwnd;

    handle = WTOpen((HWND)hwnd, &context, enable);
    if (handle==0)
    {
      throw WintabException("Could not open Wintab device context");
    }

    storeInternalData();
  }

  /**
     Restore the context from binary save infos.
   */
  void restore(long hwnd, std::string saveinfo, bool enable)
  {
    close();

    winhandle = (HWND)hwnd;

    handle = WTRestore((HWND)hwnd, (void*)(saveinfo.c_str()), enable);
    if (handle==0)
    {
      throw WintabException("Could not restore Wintab device context");
    }

    WTGet(handle, &context);
    storeInternalData();
  }

  /**
     Close the context if it was open.

     Returns false if an error occured.
   */
  bool close()
  {
    bool res = true;
    if (handle!=0)
    {
      res = WTClose(handle);
      handle = 0;
    }
    return res;
  }

  /**
     Retrieve packets from the queue.
   */
  list packetsGet(int maxpkts)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    // Enlarge the buffer if necessary
    assertBufferSize(maxpkts*sizeof(PACKET));
    // Retrieve the packets
    int n = WTPacketsGet(handle, maxpkts, buffer);

    return createPacketList(n, buffer);
  }

  /**
     Retrieve a packet.

     serial is the serial number of the packet.
   */
  Packet packet(int serial)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    assertBufferSize(sizeof(PACKET));

    // Get the packet
    if (!WTPacket(handle, serial, buffer))
    {
      throw WintabException("serial not found");
    }

    Packet res;
    res.init(pktdata, buffer);

    return res;
  }

  /**
     Enable or disable the context.
   */
  bool enable(bool flag)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    return WTEnable(handle, flag);
  }

  /**
     Move the context to the top or bottom.
   */
  bool overlap(bool totop)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    return WTOverlap(handle, totop);
  }

  /**
     Show config dialog.
   */
  bool config(long hwnd=0)
  {
    if (handle==0)
      throw WintabException("Context is not open");
  
    if (hwnd==0)
    {
      hwnd = (long)winhandle;
    }

    return WTConfig(handle, (HWND)hwnd);
  }

  /**
     Retrieve context attributes.
   */
  void get()
  {
    if (handle==0)
      throw WintabException("Context is not open");

    if (!WTGet(handle, &context))
      throw WintabException("Failed to retrieve context attributes");
  }

  /**
     Set new context attributes.
   */
  void set()
  {
    if (handle==0)
      throw WintabException("Context is not open");

    if (!WTSet(handle, &context))
      throw WintabException("Failed to set new context attributes");

    storeInternalData();
  }

  /**
     Retrieve binary save infos.
   */
  boost::python::str save()
  {
    if (handle==0)
      throw WintabException("Context is not open");

    UINT size=0;

    // Determine size of save infos
    WTInfo(WTI_INTERFACE, IFC_CTXSAVESIZE, &size);

    // Make sure the buffer is large enough
    assertBufferSize(size);

    // Retrieve save infos
    if (!WTSave(handle, buffer))
      throw WintabException("failed to retrieve save information");

    return boost::python::str((char*)buffer, size);
  }

  /**
     Retrieve packets from the queue without removing them.
   */
  list packetsPeek(int maxpkts)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    // Enlarge the buffer if necessary
    assertBufferSize(maxpkts*sizeof(PACKET));

    // Retrieve the packets
    int n = WTPacketsPeek(handle, maxpkts, buffer);

    return createPacketList(n, buffer);
  }

  /**
     Retrieve packets from the queue.
     
     Returns a tuple (numpackets, Packet-List).
   */
  tuple dataGet(int begin, int end, int maxpkts)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    // Enlarge the buffer if necessary
    assertBufferSize(maxpkts*sizeof(PACKET));

    // Retrieve the packets
    int n = 0;
    int numpkts = WTDataGet(handle, begin, end, maxpkts, buffer, &n);

    return make_tuple(numpkts, createPacketList(n, buffer));
  }

  /**
     Retrieve packets from the queue without removing them.
     
     Returns a tuple (numpackets, Packet-List).
   */
  tuple dataPeek(int begin, int end, int maxpkts)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    // Enlarge the buffer if necessary
    assertBufferSize(maxpkts*sizeof(PACKET));

    // Retrieve the packets
    int n = 0;
    int numpkts = WTDataPeek(handle, begin, end, maxpkts, buffer, &n);

    return make_tuple(numpkts, createPacketList(n, buffer));
  }

  /**
     Get the serial number of the oldest and newest packet.
     
     Returns a tuple (oldserial, newserial) or (None, None) if there
     is no packet.
   */
  tuple queuePacketsEx()
  {
    if (handle==0)
      throw WintabException("Context is not open");

    UINT olds, news;
    if (WTQueuePacketsEx(handle, &olds, &news))
      return make_tuple(olds, news);
    else
      return make_tuple(object(), object());
  }
  

  //////////////////////////////////////////////////////////////////////
  //// get/set functions for the attributes of the LOGCONTEXT structure
  //////////////////////////////////////////////////////////////////////

  int getQueueSize()
  {
    if (handle==0)
      throw WintabException("Context is not open");

    return WTQueueSizeGet(handle);
  }

  void setQueueSize(int size)
  {
    if (handle==0)
      throw WintabException("Context is not open");

    if (!WTQueueSizeSet(handle, size))
      throw WintabException("Failed to set new queue size");
  }

  std::string getName() { return std::string(context.lcName); }
  void setName(std::string s) 
  { 
    strncpy(context.lcName, s.c_str(), LC_NAMELEN); 
    context.lcName[LC_NAMELEN-1]=0; 
  }

  int getOptions() { return context.lcOptions; }
  void setOptions(int o) { context.lcOptions = o; }

  int getStatus() { return context.lcStatus; }
  void setStatus(int s) { context.lcStatus = s; }

  int getLocks() { return context.lcLocks; }
  void setLocks(int l) { context.lcLocks = l; }

  int getMsgBase() { return context.lcMsgBase; }
  void setMsgBase(int b) { context.lcMsgBase = b; }

  int getDevice() { return context.lcDevice; }
  void setDevice(int dev) { context.lcDevice = dev; }

  int getPktRate() { return context.lcPktRate; }
  void setPktRate(int rate) { context.lcPktRate = rate; }

  long getPktData() { return context.lcPktData; }
  void setPktData(long d) { context.lcPktData = d; }

  long getPktMode() { return context.lcPktMode; }
  void setPktMode(long m) { context.lcPktMode = m; }

  long getMoveMask() { return context.lcMoveMask; }
  void setMoveMask(long m) { context.lcMoveMask = m; }

  long getBtnDnMask() { return context.lcBtnDnMask; }
  void setBtnDnMask(long m) { context.lcBtnDnMask = m; }

  long getBtnUpMask() { return context.lcBtnUpMask; }
  void setBtnUpMask(long m) { context.lcBtnUpMask = m; }

  long getInOrgX() { return context.lcInOrgX; }
  void setInOrgX(long v) { context.lcInOrgX = v; }

  long getInOrgY() { return context.lcInOrgY; }
  void setInOrgY(long v) { context.lcInOrgY = v; }

  long getInOrgZ() { return context.lcInOrgZ; }
  void setInOrgZ(long v) { context.lcInOrgZ = v; }

  long getInExtX() { return context.lcInExtX; }
  void setInExtX(long v) { context.lcInExtX = v; }

  long getInExtY() { return context.lcInExtY; }
  void setInExtY(long v) { context.lcInExtY = v; }

  long getInExtZ() { return context.lcInExtZ; }
  void setInExtZ(long v) { context.lcInExtZ = v; }

  long getOutOrgX() { return context.lcOutOrgX; }
  void setOutOrgX(long v) { context.lcOutOrgX = v; }

  long getOutOrgY() { return context.lcOutOrgY; }
  void setOutOrgY(long v) { context.lcOutOrgY = v; }

  long getOutOrgZ() { return context.lcOutOrgZ; }
  void setOutOrgZ(long v) { context.lcOutOrgZ = v; }

  long getOutExtX() { return context.lcOutExtX; }
  void setOutExtX(long v) { context.lcOutExtX = v; }

  long getOutExtY() { return context.lcOutExtY; }
  void setOutExtY(long v) { context.lcOutExtY = v; }

  long getOutExtZ() { return context.lcOutExtZ; }
  void setOutExtZ(long v) { context.lcOutExtZ = v; }

  long getSensX() { return context.lcSensX; }
  void setSensX(long v) { context.lcSensX = v; }

  long getSensY() { return context.lcSensY; }
  void setSensY(long v) { context.lcSensY = v; }

  long getSensZ() { return context.lcSensZ; }
  void setSensZ(long v) { context.lcSensZ = v; }

  bool getSysMode() { return context.lcSysMode; }
  void setSysMode(bool m) { context.lcSysMode = m; }

  long getSysOrgX() { return context.lcSysOrgX; }
  void setSysOrgX(long v) { context.lcSysOrgX = v; }

  long getSysOrgY() { return context.lcSysOrgY; }
  void setSysOrgY(long v) { context.lcSysOrgY = v; }

  long getSysExtX() { return context.lcSysExtX; }
  void setSysExtX(long v) { context.lcSysExtX = v; }

  long getSysExtY() { return context.lcSysExtY; }
  void setSysExtY(long v) { context.lcSysExtY = v; }

  long getSysSensX() { return context.lcSysSensX; }
  void setSysSensX(long v) { context.lcSysSensX = v; }

  long getSysSensY() { return context.lcSysSensY; }
  void setSysSensY(long v) { context.lcSysSensY = v; }

  //////////////////////////////////////////////////////////////////////

  private:

  /**
     Copy data from the context to internal attributes.

     This method has to be called when the context is opened or when
     context attributes are changed.
   */
  void storeInternalData()
  {
    // Keep the pktdata value
    pktdata = context.lcPktData;

    // Compute the final message IDs
    id_packet = WT_PACKET-WT_DEFBASE+context.lcMsgBase;
    id_csrchange = WT_CSRCHANGE-WT_DEFBASE+context.lcMsgBase;
    id_ctxopen = WT_CTXOPEN-WT_DEFBASE+context.lcMsgBase;
    id_ctxclose = WT_CTXCLOSE-WT_DEFBASE+context.lcMsgBase;
    id_ctxupdate = WT_CTXUPDATE-WT_DEFBASE+context.lcMsgBase;
    id_ctxoverlap = WT_CTXOVERLAP-WT_DEFBASE+context.lcMsgBase;
    id_proximity = WT_PROXIMITY-WT_DEFBASE+context.lcMsgBase;
    id_infochange = WT_INFOCHANGE-WT_DEFBASE+context.lcMsgBase;
  }

  /**
     Create a Python list with Packet objects.

     n is the number of packets and buf a pointer to the first packet.
   */
  list createPacketList(int n, void* buf)
  {
    list res;
    Packet packet;
    // Create the result Python list
    char* p = (char*)buffer;
    for(int i=0; i<n; i++)
    {
      int bytes = packet.init(pktdata, p);
      res.append(packet);
      p += bytes;
    }
    return res;
  }

  /// Assert that the packet buffer has at least size bytes.
  void assertBufferSize(long size)
  {
    if (bufsize<size)
    {
      freeBuffer();
      buffer = malloc(size);
      if (buffer==0)
	throw OutOfMemory();
      bufsize = size;
    }
  }

  /// Free the packet buffer.
  void freeBuffer()
  {
    if (buffer!=0)
    {
      free(buffer);
      buffer = 0;
      bufsize = 0;
    }
  }
};

//////////////////////////////////////////////////////////////////////
// Helper functions to retrieve info values of a particular type
//////////////////////////////////////////////////////////////////////

/*
  Each function takes the category and index arguments that are
  passed into the WTInfo() function. The result is stored in the
  dictionary (4th argument) using the provided key (3rd arg).
 */

// Get a string (TCHAR[])
void getStringVal(long category, long index, std::string key, dict& res)
{
  void* p;
  long size;

  // Determine required buffer size
  size = WTInfo(category, index, 0);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  p = malloc(size);
  if (p==0)
  {
    throw OutOfMemory();
  }

  WTInfo(category, index, p);
  res[key] = std::string((char*)p);

  free(p);
}

void getWordVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  WORD w;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(w)!=size)
  {
    throw WintabException("Buffer size mismatch (WORD)");
  }

  WTInfo(category, index, &w);
  res[key] = w;
}

void getUIntVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  UINT ui;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(ui)!=size)
  {
    throw WintabException("Buffer size mismatch (UINT)");
  }

  WTInfo(category, index, &ui);
  res[key] = ui;
}

void getWTPktVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  WTPKT v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (WTPKT)");
  }

  WTInfo(category, index, &v);
  res[key] = v;
}

void getBoolVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  BOOL v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (BOOL)");
  }

  WTInfo(category, index, &v);
  res[key] = v;
}

void getDWordVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  DWORD v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (DWORD)");
  }

  WTInfo(category, index, &v);
  res[key] = v;
}

void getIntVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  int v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (int)");
  }

  WTInfo(category, index, &v);
  res[key] = v;
}

void getByteVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  BYTE v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (BYTE)");
  }

  WTInfo(category, index, &v);
  res[key] = v;
}

void getAxisVal(long category, long index, std::string key, dict& res)
{
  char buf[100];
  AXIS v;
  long size;

  // Determine required buffer size (passing 0 as last argument sometimes
  // results in a crash...)
  size = WTInfo(category, index, buf);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;
  if (sizeof(v)!=size)
  {
    throw WintabException("Buffer size mismatch (AXIS)");
  }

  WTInfo(category, index, &v);
  res[key] = make_tuple(v.axMin, v.axMax, v.axUnits, v.axResolution);
}

void getAxisArrayVal(long category, long index, std::string key, dict& res)
{
  AXIS* v;
  long size, items, i;

  // Determine required buffer size
  size = WTInfo(category, index, 0);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;

  items = size/sizeof(AXIS);

  v = (AXIS*)malloc(size);
  if (v==0)
  {
    throw OutOfMemory();
  }

  WTInfo(category, index, v);
  list lst;
  for(i=0; i<items; i++)
  {
    lst.append(make_tuple(v[i].axMin, v[i].axMax, v[i].axUnits, v[i].axResolution));
  }
  res[key] = lst;
  free(v);
}


void getStringArrayVal(long category, long index, std::string key, dict& res)
{
  TCHAR* v;
  TCHAR* p;
  long size, i;

  // Determine required buffer size
  size = WTInfo(category, index, 0);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;

  v = (TCHAR*)malloc(size);
  if (v==0)
  {
    throw OutOfMemory();
  }

  WTInfo(category, index, v);
  list lst;
  p = v;
  for(i=0; i<size-1; i++)
  {
    if (v[i]==0)
    {
      lst.append(std::string(p));
      p = v+i+1;
    }
  }
  res[key] = lst;
  free(v);
}

void getByteArrayVal(long category, long index, std::string key, dict& res)
{
  BYTE* v;
  long size, items, i;

  // Determine required buffer size
  size = WTInfo(category, index, 0);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;

  items = size/sizeof(BYTE);

  // alloc more memory than should be necessary 
  // (otherwise the below WTInfo() call will eventually produce a crash
  // when retrieving WTI_EXTENSIONS infos...?)
  v = (BYTE*)malloc(16*size);
  if (v==0)
  {
    throw OutOfMemory();
  }

  WTInfo(category, index, v);
  list lst;
  for(i=0; i<items; i++)
  {
    lst.append(v[i]);
  }
  res[key] = lst;
  free(v);
}

void getUIntArrayVal(long category, long index, std::string key, dict& res)
{
  UINT* v;
  long size, items, i;

  // Determine required buffer size
  size = WTInfo(category, index, 0);
  if (size==0)
  {
    res[key] = object();
    return;
  }

  //  std::cout<<"Required size: "<<size<<std::endl;

  items = size/sizeof(UINT);
  v = (UINT*)malloc(size);
  if (v==0)
  {
    throw OutOfMemory();
  }

  WTInfo(category, index, v);
  list lst;
  for(i=0; i<items; i++)
  {
    lst.append(v[i]);
  }
  res[key] = lst;
  free(v);
}



// Info
dict info(int category)
{
  dict res;

  if (category==WTI_INTERFACE)
  {
    getStringVal(WTI_INTERFACE, IFC_WINTABID, "WINTABID", res);
    getWordVal(WTI_INTERFACE, IFC_SPECVERSION, "SPECVERSION", res);
    getWordVal(WTI_INTERFACE, IFC_IMPLVERSION, "IMPLVERSION", res);
    getUIntVal(WTI_INTERFACE, IFC_NDEVICES, "NDEVICES", res);
    getUIntVal(WTI_INTERFACE, IFC_NCURSORS, "NCURSORS", res);
    getUIntVal(WTI_INTERFACE, IFC_NCONTEXTS, "NCONTEXTS", res);
    getUIntVal(WTI_INTERFACE, IFC_CTXOPTIONS, "CTXOPTIONS", res);
    getUIntVal(WTI_INTERFACE, IFC_CTXSAVESIZE, "CTXSAVESIZE", res);
    getUIntVal(WTI_INTERFACE, IFC_NEXTENSIONS, "NEXTENSIONS", res);
    getUIntVal(WTI_INTERFACE, IFC_NMANAGERS, "NMANAGERS", res);
  }
  else if (category==WTI_STATUS)
  {
    getUIntVal(WTI_STATUS, STA_CONTEXTS, "CONTEXTS", res);
    getUIntVal(WTI_STATUS, STA_SYSCTXS, "SYSCTXS", res);
    getUIntVal(WTI_STATUS, STA_PKTRATE, "PKTRATE", res);
    getWTPktVal(WTI_STATUS, STA_PKTDATA, "PKTDATA", res);
    getUIntVal(WTI_STATUS, STA_MANAGERS, "MANAGERS", res);
    getBoolVal(WTI_STATUS, STA_SYSTEM, "SYSTEM", res);
    getDWordVal(WTI_STATUS, STA_BUTTONUSE, "BUTTONUSE", res);
    getDWordVal(WTI_STATUS, STA_SYSBTNUSE, "SYSBTNUSE", res);
  }
  else if (category>=WTI_DEVICES && category<WTI_DEVICES+100)
  {
    getStringVal(category, DVC_NAME, "NAME", res);
    getUIntVal(category, DVC_HARDWARE, "HARDWARE", res);
    getUIntVal(category, DVC_NCSRTYPES, "NCSRTYPES", res);
    getUIntVal(category, DVC_FIRSTCSR, "FIRSTCSR", res);
    getUIntVal(category, DVC_PKTRATE, "PKTRATE", res);
    getWTPktVal(category, DVC_PKTDATA, "PKTDATA", res);
    getWTPktVal(category, DVC_PKTMODE, "PKTMODE", res);
    getWTPktVal(category, DVC_CSRDATA, "CSRDATA", res);
    getIntVal(category, DVC_XMARGIN, "XMARGIN", res);
    getIntVal(category, DVC_YMARGIN, "YMARGIN", res);
    getIntVal(category, DVC_ZMARGIN, "ZMARGIN", res);
    getAxisVal(category, DVC_X, "X", res);
    getAxisVal(category, DVC_Y, "Y", res);
    getAxisVal(category, DVC_Z, "Z", res);
    getAxisVal(category, DVC_NPRESSURE, "NPRESSURE", res);
    getAxisVal(category, DVC_TPRESSURE, "TPRESSURE", res);
    getAxisArrayVal(category, DVC_ORIENTATION, "ORIENTATION", res);
    getAxisArrayVal(category, DVC_ROTATION, "ROTATION", res);
    getStringVal(category, DVC_PNPID, "PNPID", res);
  }
  else if (category>=WTI_CURSORS && category<WTI_CURSORS+100)
  {
    getStringVal(category, CSR_NAME, "NAME", res);
    getBoolVal(category, CSR_ACTIVE, "ACTIVE", res);
    getWTPktVal(category, CSR_PKTDATA, "PKTDATA", res);
    getByteVal(category, CSR_BUTTONS, "BUTTONS", res);
    getByteVal(category, CSR_BUTTONBITS, "BUTTONBITS", res);
    getStringArrayVal(category, CSR_BTNNAMES, "BTNNAMES", res);
    getByteArrayVal(category, CSR_BUTTONMAP, "BUTTONMAP", res);
    getByteArrayVal(category, CSR_SYSBTNMAP, "SYSBTNMAP", res);
    getByteVal(category, CSR_NPBUTTON, "NPBUTTON", res);
    getUIntArrayVal(category, CSR_NPBTNMARKS, "NPBTNMARKS", res);
    getUIntArrayVal(category, CSR_NPRESPONSE, "NPRESPONSE", res);
    getByteVal(category, CSR_TPBUTTON, "TPBUTTON", res);
    getUIntArrayVal(category, CSR_TPBTNMARKS, "TPBTNMARKS", res);
    getUIntArrayVal(category, CSR_TPRESPONSE, "TPRESPONSE", res);
    getDWordVal(category, CSR_PHYSID, "PHYSID", res);
    getUIntVal(category, CSR_MODE, "MODE", res);
    getUIntVal(category, CSR_MINPKTDATA, "MINPKTDATA", res);
    getUIntVal(category, CSR_MINBUTTONS, "MINBUTTONS", res);
    getUIntVal(category, CSR_CAPABILITIES, "CAPABILITIES", res);
  }
  else if (category>=WTI_EXTENSIONS && category<WTI_EXTENSIONS+100)
  {
    getStringVal(category, EXT_NAME, "NAME", res);    
    getUIntVal(category, EXT_TAG, "TAG", res);
    getWTPktVal(category, EXT_MASK, "MASK", res);
    getUIntArrayVal(category, EXT_SIZE, "SIZE", res);
    getAxisArrayVal(category, EXT_AXES, "AXES", res);
    getByteArrayVal(category, EXT_DEFAULT, "DEFAULT", res);
    getByteArrayVal(category, EXT_DEFCONTEXT, "DEFCONTEXT", res);
    getByteArrayVal(category, EXT_DEFSYSCTX, "DEFSYSCTX", res);    
    getByteArrayVal(category, EXT_CURSORS, "CURSORS", res);    
  }
  else
  {
    throw WintabException("Unknown category");
  }
  return res;
}


// Exception translator function
void ExceptionTranslator(const WintabException& exc) 
{ 
  PyErr_SetString(PyExc_RuntimeError, exc.what()); 
} 

// Exception translator function
void MemExceptionTranslator(const OutOfMemory& exc) 
{ 
  PyErr_SetString(PyExc_MemoryError, exc.what()); 
} 


BOOST_PYTHON_MODULE(_wintab)
{
  register_exception_translator<WintabException>(&ExceptionTranslator);
  register_exception_translator<OutOfMemory>(&MemExceptionTranslator);

  def("info", info);

  class_<Packet>("Packet", init<>())
    .def_readonly("pktdata", &Packet::pktdata)
    .def_readonly("context", &Packet::context)
    .def_readonly("status", &Packet::status)
    .def_readonly("time", &Packet::time)
    .def_readonly("changed", &Packet::changed)
    .def_readonly("serial", &Packet::serial)
    .def_readonly("cursor", &Packet::cursor)
    .def_readonly("buttons", &Packet::buttons)
    .def_readonly("x", &Packet::x)
    .def_readonly("y", &Packet::y)
    .def_readonly("z", &Packet::z)
    .def_readonly("normalpressure", &Packet::normalpressure)
    .def_readonly("tangentpressure", &Packet::tangentpressure)
    .def_readonly("orient_azimuth", &Packet::orient_azimuth)
    .def_readonly("orient_altitude", &Packet::orient_altitude)
    .def_readonly("orient_twist", &Packet::orient_twist)
    .def_readonly("rot_pitch", &Packet::rot_pitch)
    .def_readonly("rot_roll", &Packet::rot_roll)
    .def_readonly("rot_yaw", &Packet::rot_yaw)

    .def("__repr__", &Packet::__repr__)
  ;

  class_<Context>("Context", init<>())
    .add_property("name", &Context::getName, &Context::setName)
    .add_property("options", &Context::getOptions, &Context::setOptions)
    .add_property("status", &Context::getStatus, &Context::setStatus)
    .add_property("locks", &Context::getLocks, &Context::setLocks)
    .add_property("msgbase", &Context::getMsgBase, &Context::setMsgBase)
    .add_property("device", &Context::getDevice, &Context::setDevice)
    .add_property("pktrate", &Context::getPktRate, &Context::setPktRate)
    .add_property("pktdata", &Context::getPktData, &Context::setPktData)
    .add_property("pktmode", &Context::getPktMode, &Context::setPktMode)
    .add_property("movemask", &Context::getMoveMask, &Context::setMoveMask)
    .add_property("btndnmask", &Context::getBtnDnMask, &Context::setBtnDnMask)
    .add_property("btnupmask", &Context::getBtnUpMask, &Context::setBtnUpMask)
    .add_property("inorgx", &Context::getInOrgX, &Context::setInOrgX)
    .add_property("inorgy", &Context::getInOrgY, &Context::setInOrgY)
    .add_property("inorgz", &Context::getInOrgZ, &Context::setInOrgZ)
    .add_property("inextx", &Context::getInExtX, &Context::setInExtX)
    .add_property("inexty", &Context::getInExtY, &Context::setInExtY)
    .add_property("inextz", &Context::getInExtZ, &Context::setInExtZ)
    .add_property("outorgx", &Context::getOutOrgX, &Context::setOutOrgX)
    .add_property("outorgy", &Context::getOutOrgY, &Context::setOutOrgY)
    .add_property("outorgz", &Context::getOutOrgZ, &Context::setOutOrgZ)
    .add_property("outextx", &Context::getOutExtX, &Context::setOutExtX)
    .add_property("outexty", &Context::getOutExtY, &Context::setOutExtY)
    .add_property("outextz", &Context::getOutExtZ, &Context::setOutExtZ)
    .add_property("sensx", &Context::getSensX, &Context::setSensX)
    .add_property("sensy", &Context::getSensY, &Context::setSensY)
    .add_property("sensz", &Context::getSensZ, &Context::setSensZ)
    .add_property("sysmode", &Context::getSysMode, &Context::setSysMode)
    .add_property("sysorgx", &Context::getSysOrgX, &Context::setSysOrgX)
    .add_property("sysorgy", &Context::getSysOrgY, &Context::setSysOrgY)
    .add_property("sysextx", &Context::getSysExtX, &Context::setSysExtX)
    .add_property("sysexty", &Context::getSysExtY, &Context::setSysExtY)
    .add_property("syssensx", &Context::getSysSensX, &Context::setSysSensX)
    .add_property("syssensy", &Context::getSysSensY, &Context::setSysSensY)

    .add_property("queuesize", &Context::getQueueSize, &Context::setQueueSize)

    .def_readonly("id_packet", &Context::id_packet)
    .def_readonly("id_csrchange", &Context::id_csrchange)
    .def_readonly("id_ctxopen", &Context::id_ctxopen)
    .def_readonly("id_ctxclose", &Context::id_ctxclose)
    .def_readonly("id_ctxupdate", &Context::id_ctxupdate)
    .def_readonly("id_ctxoverlap", &Context::id_ctxoverlap)
    .def_readonly("id_proximity", &Context::id_proximity)
    .def_readonly("id_infochange", &Context::id_infochange)

    .def("open", &Context::open, (arg("hwnd"), arg("enable")))
    .def("restore", &Context::restore, (arg("hwnd"), arg("saveinfo"), arg("enable")))
    .def("close", &Context::close)
    .def("packet", &Context::packet, arg("serial"))
    .def("enable", &Context::enable)
    .def("overlap", &Context::overlap)
    .def("config", &Context::config, arg("hwnd")=0)
    .def("get", &Context::get)
    .def("set", &Context::set)
    .def("save", &Context::save)
    .def("packetsGet", &Context::packetsGet, arg("maxpkts"))
    .def("packetsPeek", &Context::packetsPeek, arg("maxpkts"))
    .def("dataGet", &Context::dataGet, (arg("begin"), arg("end"), arg("maxpkts")))
    .def("dataPeek", &Context::dataPeek, (arg("begin"), arg("end"), arg("maxpkts")))
    .def("queuePacketsEx", &Context::queuePacketsEx)
  ;
}
