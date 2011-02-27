
:mod:`pluginmanager` --- Managing plugins
=========================================

.. module:: cgkit.pluginmanager
   :synopsis: Managing plugins


A plugin is a file that gets imported just like a regular Python module. Each
plugin can be uniquely identified either by its absolute filename or its module
name. The module name is only available once the plugin is loaded. Any objects
(usually classes) that are specially marked will be made available by the plugin
manager for later retrieval.

A class gets imported if it has an attribute ``_protocols`` which is a list of
supported protocols (a protocol is identified by an arbitrary hashable object).
Each class can be uniquely identified by its module name and class name.

Plugins are loaded by calling :func:`importPlugin` or  :func:`importPlugins`.
You can also register any class as plugin class any time by calling the
:func:`register` function. When a plugin is loaded or an object is registered
the plugin/object is represented by a descriptor class
(:class:`PluginDescriptor` resp. :class:`PluginObjectDescriptor`).  These
descriptor objects contain information about the plugin/object. The  object
descriptor also holds a reference to the actual plugin object.

You can iterate over all currently available protocols via the
:func:`iterProtocols` function. The :func:`iterProtoObjects`  function can be
used to iterate over all objects that support a particular protocol and
:func:`iterObjects` can be used to iterate over all objects. The default
iterator iterates over all plugin descriptors.

Example plugin class::

   # file: plugin.py

   class PluginClass:

       _protocols = ["MyProtocol"]

       def __init__(self):
           ...
       ...

And here's how to use it::

    # Load the plugin
    pdesc = importPlugin("plugin.py")
    if pdesc.status!=STATUS_OK:
        # there was an error...

    # Search a plugin class
    objdesc = findObject("plugin.PluginClass")
    # Create an instance of the plugin class...
    PluginClass = objdesc.object
    instance = PluginClass()

The actual functionality is implemented in the :class:`PluginManager` class.
However, the module creates a global instance of this class which can be
accessed by the module functions. These functions just call the corresponding
methods of the global plugin manager object. If you want to load plugins that
should be stored separately from the plugins in your surrounding application
then you can also create your own instance of the :class:`PluginManager` class.

.. % importPlugin


.. function:: importPlugin(filename, overwrite=False)

   Loads a plugin file. The given file is executed, i.e. it is imported like a
   module. Any object (usually class or function) that has an attribute
   ``_protocols`` is stored in the plugin manager. This attribute is a list of
   protocol specifiers (which are just arbitrary hashable objects).

   The file must have a suffix that is recognized as a module by the Python
   interpreter (one of the suffixes returned by :func:`imp.get_suffixes`),
   otherwise an :exc:`UnknownFileType` exception is thrown.

   It is possible to load a plugin several times if *overwrite* is set to ``True``.
   The new definitions override the previous ones. However, if references or
   instances to old objects are kept somewhere they still refer to the old
   definition. When writing a plugin you should always bear in mind that the file
   could be executed several times and write your initialization code accordingly.

   The function returns a :class:`PluginDescriptor` object which contains
   information about the imported plugin.

.. % importPlugins


.. function:: importPlugins(plugins, out=sys.stderr)

   Import several plugins at once. *plugins* can be a single file/directory name or
   a sequence of file/directory names. Directories are recursively descended. *out*
   is a stream which is used to output status messages.


.. function:: register(obj, name=None, pdesc=None, overwrite=False)


.. function:: remove(objdesc)


.. function:: removePlugin(pdesc)


.. function:: removeAll()


.. function:: findPlugin(filename)


.. function:: findObject(name, modname=None)


.. function:: iterPlugins()


.. function:: iterProtocols()


.. function:: iterProtoObjects(proto)


.. function:: iterObjects()

.. % ---------------------------------------------------


PluginManager class
-------------------

.. % ---------------------------------------------------


PluginDescriptor
----------------

A :class:`PluginDescriptor` object keeps the status information of a plugin
file. It has the following attributes:


.. class:: PluginDescriptor()


   .. data:: filename

      The absolute file name of the plugin.


   .. data:: filedesc

      A file descriptor tuple (*suffix*, *mode*, *type*) as returned by the
      :func:`get_suffixes` function in the :mod:`imp` module (*type* is one of
      ``imp.PY_SOURCE``, ``imp.PY_COMPILED``  or ``imp.C_EXTENSION``).


   .. data:: objcount

      Number of imported objects (usually this should be number of classes +  number
      of functions).


   .. data:: classcount

      Number of imported classes.


   .. data:: funccount

      Number of imported functions.


   .. data:: status

      Status flags. If everything is ok the status is ``STATUS_OK``, otherwise its a
      combination of ``STATUS_EXCEPTION`` and ``STATUS_DUPLICATE``.


   .. data:: traceback

      A string containing the traceback message (if the ``STATUS_EXCEPTION`` flag is
      set).


   .. data:: objdescs

      A list of object descriptors of all imported objects.


   .. data:: module

      The module object of the imported plugin.


   .. data:: modulename

      The name of the plugin module.

.. % ---------------------------------------------------


PluginObjectDescriptor
----------------------

A :class:`PluginObjectDescriptor` object keeps the status information of a
plugin object. It has the following attributes and methods:


.. class:: PluginObjectDescriptor()


   .. data:: object

      The actual plugin object.


   .. data:: name

      The name of the object.


   .. data:: plugindesc

      The :class:`PluginDescriptor` of the associated plugin.


   .. data:: status

      Status flags. If everything is ok the status is ``STATUS_OK``, otherwise its
      ``STATUS_DUPLICATE``.


   .. method:: PluginObjectDescriptor.moduleName()

      Return the module name where the plugin object belongs to (or None).


   .. method:: PluginObjectDescriptor.objectIdentifier()

      Return the name under which the object is identified. This name is composed of
      the module name and the object name, separated by a dot.

