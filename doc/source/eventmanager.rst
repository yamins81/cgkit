
:mod:`eventmanager` --- Receiving and routing events
====================================================

.. module:: cgkit.eventmanager
   :synopsis: Receiving and routing events


This module defines the :class:`EventManager` class which is used to manage
events. An event is something that occurs at a particular time and that might
trigger actions. Each type of event has to be identified by a unique name and
each occurrence of an event can be  associated with arbitrary data (for example,
if a key press occurs the data contains the key code).

When dealing with events you have to consider two parts. One part is producing
the events and the other part is consuming them. And it is the
:class:`EventManager` that acts as a broker between the two. Whenever an event
occurs the producer tells the event manager about the event and the event
manager takes care of notifying anyone who showed interest in this type of
event. This way, the producer and consumer don't have to know each other but can
rely on the event manager who is always present (whereas a particular producer
or consumer might not be available all the time).

The event manager distinguishes between system wide events and normal events.
Normal events are those that have a relationship with the current scene and that
are removed if the scene is cleared. On the other hand, system wide events are
independent from the current scene and remain intact if the scene is cleared.

There is always a global event manager instance that should be used for managing
all events:


.. function:: eventManager()

   Returns the global event manager object.


.. class:: EventManager()

   Event manager class. Usually you don't have to create an instance of this class
   but use the global event manager instead.


.. method:: EventManager.event(name, *params, **keyargs)

   Signal the event manager that an event has occurred. The type of event is
   identified by the string *name* which must be a unique name among all event
   types. The remaining arguments are the data associated with this particular
   occurrence. This data is passed to the consumers.

   The method returns ``True`` if any of the event handlers returned ``True`` (i.e.
   the event was consumed). In this case, the notification chain was interrupted.
   This means, any event handler that would have been called after the one that
   returned ``True`` was not called anymore.


.. method:: EventManager.connect(name, receiver, priority=10, system=False)

   Connect a function or method to an event type. Whenever an event of type *name*
   occurs the event handler specified by *receiver* is called. *receiver* can
   either be a callable such as a function or a method or it can be an instance of
   a class that must implement an  ``onname()`` method. *priority* determines the
   order in  which the receivers are invoked. Receivers with lower values (=high
   priority) are invoked first.

   The argument *system* specifies if the connection is system wide or not.


.. method:: EventManager.disconnect(name, receiver=None, system=False)

   Disconnect a function or method from the event type *name*. The argument
   *receiver* has the same meaning as in the :meth:`connect` method. If *receiver*
   is ``None`` then all connections from the event are removed. The argument
   *system* specifies if the connection is system wide or not.


.. method:: EventManager.disconnectAll(system=False)

   Remove all connections from all events. *system* specifies whether the system
   wide connections or the scene wide connections shall be removed

