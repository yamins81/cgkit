/*
  3DXWare wrapper
 */

#include <boost/python.hpp>
#include <iostream>
#include <exception>

#ifdef WIN32
#define OS_WIN32
#endif

extern "C" {
#include <spwmacro.h>
#include <si.h>
#include <siapp.h>
}

using namespace boost::python;

// Exception class
class SpaceDeviceException : public std::exception
{
  SpwRetVal retval;
  public:
  SpaceDeviceException(long aretval) : retval(SpwRetVal(aretval)) {}
  ~SpaceDeviceException() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return SpwErrorString(retval);
  }
};


/**
  Wrapper around the 3Dconnexion SDK.
 */
class SpaceDevice
{
  // SpaceMouse/SpaceBall handle
  SiHdl device;

  public:
  SpaceDevice()
    : device(0)
  {
    //    std::cout<<"SiInitialize()"<<std::endl;
    SiInitialize();
  }

  ~SpaceDevice()
  {
    //    std::cout<<"SiTerminate()"<<std::endl;
    close();
    SiTerminate();
  }

  //////////////////////////////////////////////////////////////////////

  /**
    Connect to a device.

    appname is the application name and hWnd the window handle of the
    window that should receive the events.
    devID is the device ID or SI_ANY_DEVICE.
  */
  void open(std::string appname, int hwnd, SiDevID devID=SI_ANY_DEVICE)
  {
    SiOpenData data;

    SiOpenWinInit(&data, (HWND)hwnd);
    device = SiOpen((char*)appname.c_str(), devID, SI_NO_MASK, SI_EVENT, &data);
    if (device==0)
    {
      // the error should be in SpwErrorVal, but this variable isn't there...?
      //      std::cout<<"ERROR: could not open device"<<std::endl;
      throw SpaceDeviceException(SPW_ERROR);
    }
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Close the device.
   */
  SpwRetVal close()
  {
    SpwRetVal res = SPW_NO_ERROR;

    if (device!=0)
    {
      res = (SpwRetVal)SiClose(device);
      device = 0;
    }
    return res;
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Translaste a Win32 event into a SpaceDevice event.

     Returns a tuple (RetVal, EventType, Data)
   */
  tuple translateWin32Event(long msgid, long wparam, long lparam)
  {
    SiGetEventData eventdata;
    SiSpwEvent event;  /* result of the translation */

    SiGetEventWinInit(&eventdata, msgid, wparam, lparam);
    SpwRetVal res = SpwRetVal(SiGetEvent(device, 0, &eventdata, &event));

    if (res==SI_IS_EVENT)
    {
      switch(event.type)
      {
       // Button press or release?
       case SI_BUTTON_EVENT:
	 return make_tuple(res, 
			   SiEventType(event.type), 
			   make_tuple(buttonList(event.u.spwData.bData.pressed), buttonList(event.u.spwData.bData.released)));
       // Motion?
       case SI_MOTION_EVENT:
	 {
           tuple t;  /* translation */
	   tuple r;  /* rotation */
           t = make_tuple(event.u.spwData.mData[SI_TX], event.u.spwData.mData[SI_TY], event.u.spwData.mData[SI_TZ]);
           r = make_tuple(-event.u.spwData.mData[SI_RX], -event.u.spwData.mData[SI_RY], -event.u.spwData.mData[SI_RZ]);
	   return make_tuple(res, 
			     SiEventType(event.type), 
			     make_tuple(t, r, event.u.spwData.period));
	 }
       // Zero?
       case SI_ZERO_EVENT:
	 return make_tuple(res, SiEventType(event.type), object());
       // Exception?
       case SI_EXCEPTION_EVENT:
	 return make_tuple(res, SiEventType(event.type), object());
      }
      return make_tuple(res, SiEventType(event.type), object());
    }
    else  /* no valid space mouse event */
    {
      return make_tuple(res, object(), object());
    }
  }

  //////////////////////////////////////////////////////////////////////

  /**
    Causes the device to emit a sequence of tones and pauses.

    Lowercase letters represent a tone, uppercase letters represent a pause.
    The closer the letter is to the beginning of the alphabet the shorter 
    the pause or tone.
   */
  void beep(std::string s)
  {
    long res = SiBeep(device, (char*)(s.c_str()));
    if (res!=SPW_NO_ERROR)
      throw SpaceDeviceException(res);
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Get the device ID.
   */
  long getDeviceID()
  {
    return SiGetDeviceID(device);
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Get infos about the device.

     Returns a tuple (device type, numButtons, numDegrees, canBeep, firmware).
   */
  tuple getDeviceInfo()
  {
    SiDevInfo info;
    long res = SiGetDeviceInfo(device, &info);
    if (res!=SPW_NO_ERROR)
      throw SpaceDeviceException(res);

    return make_tuple(SiDevType(info.devType), 
		      info.numButtons, 
		      info.numDegrees,
		      bool(info.canBeep),
		      info.firmware);
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Get version infos about the driver.

     Returns a tuple (major, minor, build, versionstr, datestr)
   */
  tuple getDriverInfo()
  {
    SiVerInfo info;
    long res = SiGetDriverInfo(&info);
    if (res!=SPW_NO_ERROR)
      throw SpaceDeviceException(res);
    
    return make_tuple(info.major, info.minor, info.build, info.version, info.date);
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Get the number of devices.
   */
  int getNumDevices()
  {
    return SiGetNumDevices();
  }

  //////////////////////////////////////////////////////////////////////

  /**
     Rezero.
   */
  void rezero()
  {
    long res = SiRezero(device);
    if (res!=SPW_NO_ERROR)
      throw SpaceDeviceException(res);
  }

  //////////////////////////////////////////////////////////////////////

  /**
     setUIMode.
   */
  void setUIMode(bool show)
  {
    long res;
    if (show)
      res = SiSetUiMode(device, SI_UI_ALL_CONTROLS);
    else
      res = SiSetUiMode(device, SI_UI_NO_CONTROLS);
    
    if (res!=SPW_NO_ERROR)
      throw SpaceDeviceException(res);
  }


  private:
  //////////////////////////////////////////////////////////////////////

  /**
     Convert a button "mask" into a list of button numbers.
     Bit 2 is button 1, bit 3 -> button 2, ...
   */
  list buttonList(int buttons)
  {
    list res;
    int nr = 1;

    buttons >>= 1;   
    while(nr<30)
    {
      if (buttons & 0x01)
      {
	res.append(nr);
      }
      nr += 1;
      buttons >>= 1;
    }
    return res;
  }

};

// Exception translator function
void ExceptionTranslator(const SpaceDeviceException& exc) 
{ 
  PyErr_SetString(PyExc_RuntimeError, exc.what()); 
} 


BOOST_PYTHON_MODULE(_spacedevice)
{
  register_exception_translator<SpaceDeviceException>(&ExceptionTranslator);

  enum_<SpwRetVal>("RetVal")
    .value("NO_ERROR", SPW_NO_ERROR)
    .value("ERROR", SPW_ERROR)
    .value("BAD_HANDLE", SI_BAD_HANDLE)
    .value("BAD_ID", SI_BAD_ID)
    .value("BAD_VALUE", SI_BAD_VALUE)
    .value("IS_EVENT", SI_IS_EVENT)
    .value("SKIP_EVENT", SI_SKIP_EVENT)
    .value("NOT_EVENT", SI_NOT_EVENT)
    .value("NO_DRIVER", SI_NO_DRIVER)
    .value("NO_RESPONSE", SI_NO_RESPONSE)
    .value("UNSUPPORTED", SI_UNSUPPORTED)
    .value("UNINITIALIZED", SI_UNINITIALIZED)
    .value("WRONG_DRIVER", SI_WRONG_DRIVER)
    .value("INTERNAL_ERROR", SI_INTERNAL_ERROR)
  ;

  enum_<SiEventType>("EventType")
    .value("BUTTON_EVENT", SI_BUTTON_EVENT)
    .value("MOTION_EVENT", SI_MOTION_EVENT)
    .value("COMBO_EVENT", SI_COMBO_EVENT)
    .value("ZERO_EVENT", SI_ZERO_EVENT)
    .value("EXCEPTION_EVENT", SI_EXCEPTION_EVENT)
    .value("OUT_OF_BAND", SI_OUT_OF_BAND)
    .value("ORIENTATION_EVENT", SI_ORIENTATION_EVENT)
    .value("KEYBOARD_EVENT", SI_KEYBOARD_EVENT)
    .value("LPFK_EVENT", SI_LPFK_EVENT)
  ;

  enum_<SiDevType>("DevType")
    .value("UNKNOWN_DEVICE", SI_UNKNOWN_DEVICE)
    .value("SPACEBALL_2003", SI_SPACEBALL_2003)
    .value("SPACEBALL_3003", SI_SPACEBALL_3003)
    .value("SPACE_CONTROLLER", SI_SPACE_CONTROLLER)
    .value("AVENGER", SI_AVENGER)
    .value("SPACEORB_360", SI_SPACEORB_360)
    .value("NAVIGATOR", SI_NAVIGATOR)
    .value("SPACEBALL_2003A", SI_SPACEBALL_2003A)
    .value("SPACEBALL_2003B", SI_SPACEBALL_2003B)
    .value("SPACEBALL_2003C", SI_SPACEBALL_2003C)
    .value("SPACEBALL_3003A", SI_SPACEBALL_3003A)
    .value("SPACEBALL_3003B", SI_SPACEBALL_3003B)
    .value("SPACEBALL_3003C", SI_SPACEBALL_3003C)
    .value("SPACEBALL_4000", SI_SPACEBALL_4000)
    .value("SPACEMOUSE_CLASSIC", SI_SPACEMOUSE_CLASSIC)
    .value("SPACEMOUSE_PLUS", SI_SPACEMOUSE_PLUS)
    .value("SPACEMOUSE_XT", SI_SPACEMOUSE_XT)
    .value("CYBERMAN", SI_CYBERMAN)
    .value("CADMAN", SI_CADMAN)
    .value("SPACEMOUSE_CLASSIC_PROMO", SI_SPACEMOUSE_CLASSIC_PROMO)
    .value("SERIAL_CADMAN", SI_SERIAL_CADMAN)
    .value("SPACEBALL_5000", SI_SPACEBALL_5000)
    .value("TEST_NO_DEVICE", SI_TEST_NO_DEVICE)
    .value("3DX_KEYBOARD_BLACK", SI_3DX_KEYBOARD_BLACK)
    .value("3DX_KEYBOARD_WHITE", SI_3DX_KEYBOARD_WHITE)
    .value("TRAVELER", SI_TRAVELER)
    .value("TRAVELER1", SI_TRAVELER1)
    .value("SPACEBALL_5000A", SI_SPACEBALL_5000A)
  ;

  class_<SpaceDevice>("SpaceDevice", init<>())
    .def("open", &SpaceDevice::open, (arg("appname"), arg("hwnd"), arg("devID")=-1))
    .def("close", &SpaceDevice::close)
    .def("translateWin32Event", &SpaceDevice::translateWin32Event, (arg("msgid"), arg("wparam"), arg("lparam")))
    .def("beep", &SpaceDevice::beep, arg("s"))
    .def("getDeviceID", &SpaceDevice::getDeviceID)
    .def("getDeviceInfo", &SpaceDevice::getDeviceInfo)
    .def("getDriverInfo", &SpaceDevice::getDriverInfo)
    .def("getNumDevices", &SpaceDevice::getNumDevices)
    .def("rezero", &SpaceDevice::rezero)
    .def("setUIMode", &SpaceDevice::setUIMode, arg("show"))
  ;
}
