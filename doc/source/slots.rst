
:mod:`slots` --- Slot classes
=============================

.. module:: cgkit.slots
   :synopsis: Slot classes


.. % -------------------------------------------------


Dependent class
---------------


.. class:: Dependent()


.. method:: Dependent.onValueChanged()

   Normal/array


.. method:: Dependent.onResize(newsize)

   This method is called by a controlling array slot whenever the size of the slot
   has changed.


.. method:: Dependent.queryResizeVeto(size)

   Check if a dependent vetoes a resize operation. This method is called by an
   array slot on all its dependents whenever the slot size should be changed to
   *size*. If the method returns ``True`` the resize operation is rejected. The
   default implementation returns ``False``. Only slots with size constraints
   should return ``True``.

.. % -------------------------------------------------


Slot classes
------------

There are the following standard slots:

* DoubleSlot

* IntSlot

* BoolSlot

* StrSlot

* PySlot

* Vec3Slot

* Vec4Slot

* Mat3Slot

* Mat4Slot

* QuatSlot


.. class:: Slot([value, flags])

   Create a slot that initially carries the specified value.

   Todo: Describe the flags argument.

.. % \begin{memberdesc}{_value}
.. % The current slot value.
.. % \end{memberdesc}
.. % isSlotCompatible


.. method:: Slot.isSlotCompatible(slot)

   Return ``True`` if *slot* is compatible with this slot. Two slots are compatible
   if they hold the same type of value. Only compatible slots can be connected to
   each other. But note that even when this method returns true, it can happen that
   the slots can not be connected. This is the case whenever the target just
   doesn't accept input connections (for example, because it's computing its output
   value procedurally).

.. % typeName


.. method:: Slot.typeName()

   Return a string containing the name of the stored type. This name can be used to
   display the type of the slot to the user. The name should be chosen so that if
   two slots cannot be connected to each other then they should also return
   different names (however, if two slots have different names it is still possible
   that they can be connected to each other).

.. % getController


.. method:: Slot.getController()

   Return the associated controlling slot or ``None``.

.. % setController


.. method:: Slot.setController(slot)

   Set or remove a controller for this slot.  The current controller is replaced
   with *slot* which may also be ``None`` to remove the current controller only. If
   the controller is not compatible with this slot an :exc:`EIncompatibleSlotTypes`
   exception is thrown.

.. % getValue


.. method:: Slot.getValue()

   Return the current slot value.

.. % setValue


.. method:: Slot.setValue(val)

   Set a new value. If the slot has a controller the new value is propagated up to
   the controller. It is then up to the controller how this new value influences
   the actual output value. So it is not guaranteed that the output of the slot is
   really the new value. When the value really changes this method triggers the
   onValueChanged() method on all dependent objects.

.. % connect


.. method:: Slot.connect(slot)

   Connect the output of this slot with the input of *slot*. Calling this method is
   equivalent to calling ``slot.setController(self)``. The method has no effect if
   *slot* is ``None``.

.. % disconnect


.. method:: Slot.disconnect(slot)

   Breaks an existing connection between this slot and *slot*. Calling this method
   is equivalent to calling ``slot.setController(None)``. The method checks if the
   connection actually exists before breaking it. If it does not exist, a
   :exc:`ValueError` exception is raised. The method has no effect if *slot* is
   ``None``.

.. % addDependent


.. method:: Slot.addDependent(d)

   Establish a dependency between this slot and another object *d*. The argument
   *d* is added to the list of dependent objects. This means that *d* somehow
   depends on the value of this slot and that *d* has to be notified whenever the
   value of this slot changes. The actual notification is done by calling
   :meth:`notifyDependents` which in turn calls :meth:`onValueChanged` on all
   dependent objects.

   Todo: C++ warning!

.. % removeDependent


.. method:: Slot.removeDependent(d)

   Remove the dependency between this slot and another object *d*.

.. % notifyDependents


.. method:: Slot.notifyDependents()

   Notify all dependent slots about a value change. This method calls the
   :meth:`onValueChanged` method of all slots whose value depends on the  value of
   this slot.

.. % computeValue


.. method:: Slot.computeValue()

   This method is used to compute a new value whenever :meth:`getValue` is called
   and the cache is invalid. It should only be implemented on procedural slots.

.. % -------------------------------------------------


ArraySlot classes
-----------------


.. class:: ArraySlot(multiplicity=1, constraint=None)

   Create an array slot.

   *multiplicity* is the "array length" of one individual item.

   *constraint* is a SizeConstraint object that constrains the size of the array
   slot (see section :ref:`sizeconstraints`).

.. % size


.. method:: ArraySlot.size()

   Return the number of elements in this array slot.

.. % isResizable


.. method:: ArraySlot.isResizable(size, ignorelocalconstraint=False)

   Check if this slot can currently be resized to a particular size. This method
   can be used to check if calling :meth:`resize` with *size* as argument would
   succeed or not.

   The method returns ``False`` if either the slot itself is size constrained or it
   is connected to a size constrained slot. In the latter case, the function will
   return ``True`` again if the connection is removed. The method will also return
   ``True`` if *size* matches the constraint size.

   If *ignorelocalconstraint* is ``True`` the method will compute its result as if
   there would no constraint be present in this  slot (so only dependent objects
   could reject the resize operation).

.. % resize


.. method:: ArraySlot.resize(size)

   Resize the array slot so that it can carry *size* elements. The values in the
   array are not modified by this operation.

.. % multiplicity


.. method:: ArraySlot.multiplicity()

   Return the multiplicity of the array slot.

.. % copyValues


.. method:: ArraySlot.copyValues(begin, end, target, index)

   Copies the slice [*begin* : *end*] into position *index* of the slot *target*.
   If you want to copy one single value you have to pass [n, n+1]. It is also
   possible to pass negative values. For the copy operation to succeed, target has
   to be of the same type than this slot.

   If any index is out of range, an exception is thrown.

   Note: If this method is used to copy values within the same slot then the target
   range shouldn't currently overlap with the source slice.

.. % isSlotCompatible


.. method:: ArraySlot.isSlotCompatible(slot)

   Return ``True`` if *slot* is compatible with this slot. Two slots are compatible
   if they hold the same type of value. Only compatible slots can be connected to
   each other. But note that even when this method returns true, it can happen that
   the slots can not be connected. This is the case whenever the target just
   doesn't accept input connections (for example, because it's computing its output
   value procedurally).

.. % typeName


.. method:: ArraySlot.typeName()

   Return a string containing the name of the stored type. This name can be used to
   display the type of the slot to the user. The name should be chosen so that if
   two slots cannot be connected to each other then they should also return
   different names (however, if two slots have different names it is still possible
   that they can be connected to each other).

.. % getController


.. method:: ArraySlot.getController()

   Return the associated controlling slot or ``None``.

.. % setController


.. method:: ArraySlot.setController(slot)

   Set or remove a controller for this slot.  The current controller is replaced
   with *slot* which may also be ``None`` to remove the current controller only. If
   the controller is not compatible with this slot an :exc:`EIncompatibleSlotTypes`
   exception is thrown.

.. % getValue


.. method:: ArraySlot.getValue(index)

   Return one item of the array slot. If the multiplicity of the slot is 1 then
   return value is just an individual value, otherwise it's a tuple containing the
   values.

.. % setValue


.. method:: ArraySlot.setValue(index, value)

   Set one item to a new value. If the multiplicity of the slot is 1 *value* has to
   be one single value, otherwise it must be a sequence containing *multiplicity*
   elements.

.. % connect


.. method:: ArraySlot.connect(slot)

   Connect the output of this slot with the input of *slot*. Calling this method is
   equivalent to calling ``slot.setController(self)``.

.. % addDependent


.. method:: ArraySlot.addDependent(d)

   Establish a dependency between this slot and another object *d*. The argument
   *d* is added to the list of dependent objects. This means that *d* somehow
   depends on the value of this slot and that *d* has to be notified whenever the
   value of this slot changes. The actual notification is done by calling
   :meth:`notifyDependents` which in turn calls :meth:`onValueChanged` on all
   dependent objects.

   Todo: C++ warning!

.. % removeDependent


.. method:: ArraySlot.removeDependent(d)

   Remove the dependency between this slot and another object *d*.

.. % notifyDependents
.. % \begin{methoddesc}{notifyDependents}{}
.. % Notify all dependent slots about a value change. This method calls the
.. % \method{onValueChanged()} method of all slots whose value depends on the
.. % value of this slot.
.. % \end{methoddesc}
.. % -------------------------------------------------


.. _sizeconstraints:

SizeConstraint objects
----------------------

:class:`SizeConstraint` objects can be passed into the constructor of an array
slot to constrain its size. There are two variants, the
:class:`LinearSizeConstraint` and the :class:`UserSizeConstraint`. The former
constrains an array slot to a size that linearly depends on the size of another
array slot, whereas the latter receives its size from the user.

.. % LinearSizeConstraint


.. class:: LinearSizeConstraint(ctrlslot, a=1, b=0)

   This class depends its size linearly on the size of another slot.

   *ctrlslot* is an array slot whose size determines the size of the constraint.

   *a* and *b* are two coefficients that are used to calculate the constraint size
   which is :math:`a \cdot s + b` where s is the size of *ctrlslot*.

.. % getCoeffs


.. method:: LinearSizeConstraint.getCoeffs()

   Returns a tuple (*a*, *b*) with the current coefficients.

.. % setCoeffs


.. method:: LinearSizeConstraint.setCoeffs(a, b)

   Set new coefficients for the constraint. The operation might raise an exception
   if the new coefficients are not compatible with another constraint. In this
   case, the old coefficients remain active.

.. % UserSizeConstraint


.. class:: UserSizeConstraint(size=0)

   This class stores a size that can be manually set by the user.

   *size* is the initial size of the constraint.

.. % setSize


.. method:: UserSizeConstraint.setSize(size)

   Set a new constraint size. All slots that are constrained by this object will be
   resized. Note that the operation might fail and an exception is thrown when the
   resize operation is not allowed (usually because there are other slots involved
   that are also size constrained).

.. % -------------------------------------------------


Procedural slots
----------------

There are the following standard procedural slots:

* ProceduralDoubleSlot

* ProceduralIntSlot

* ProceduralVec3Slot

* ProceduralVec4Slot

* ProceduralMat3Slot

* ProceduralMat4Slot

* ProceduralQuatSlot

.. % -------------------------------------------------


NotificationForwarder class
---------------------------

A :class:`NotificationForwarder` object can be used to forward slot events from
one particular slot to an arbitrary method instead of the :meth:`onValueChanged`
or :meth:`onResize` method. To accomplish this the
:class:`NotificationForwarder` object is added as a dependent object of the
slot. This means, whenever the value of the slot changes, the
:meth:`onValueChanged` method of the :class:`NotificationForwarder` object is
called which in turn calls an arbitrary other function or method.


.. class:: NotificationForwarder(onvalchanged, onresize=None)

   *onvalchanged* is an arbitrary callable object that is invoked whenever a value
   changed event has occured. If the forwarder is used with a normal slot, the
   callable does not take any arguments. If it is used with an array slot, it takes
   two arguments *start* and *end* that specify what range has been modified.

   *onresize* is a callable that gets called whenever the size of an array slot has
   changed. It takes one argument which is the new size of the slot.

Example::

   >>> from cgkit import *
   >>> def myCallback(): print "Something has happened"
   ...
   >>> s=Sphere()
   >>> n=NotificationForwarder(myCallback)
   >>> s.radius_slot.addDependent(n)
   Something has happened
   >>> s.radius=5
   Something has happened
   >>> s.radius=2
   Something has happened  

