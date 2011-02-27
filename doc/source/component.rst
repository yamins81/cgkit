.. % Component


:class:`Component` --- Component base class
===========================================

A component is just something that has a name to identify it and that stores
slot objects. Usually, the slots are used for providing inputs to the component
which are used to generate outputs that can also be accessed via slots. The
component is then the 'black box' that computes the output from the input.


.. class:: Component(name = "",  auto_insert = True)

   *name* is the name of the component which can be used to identify it.

   The component will be inserted into the scene automatically if  *auto_insert* is
   set to ``True``.


   .. attribute:: Component.name

      The component's name. You can read and write this attribute.

   .. % Methods


   .. method:: Component.addSlot(name, slot)

      Add a new slot to the component. *name* specifies the slot name and *slot* is
      the slot object. A :exc:`KeyError` exception is thrown if there is already a
      slot with the specified name.


   .. method:: Component.hasSlot(name)

      Check if a slot with a particular name does exist.


   .. method:: Component.iterSlots()

      Return an iterator that iterates over all slot names.


   .. method:: Component.numSlots()

      Return the number of slots in this component.


   .. method:: Component.removeSlot(name)

      Remove the slot with the given name. A :exc:`KeyError` exception is thrown if
      there is no slot with the specified name.


   .. method:: Component.slot(name)

      Return the slot with the given name. A :exc:`KeyError` exception is thrown if
      there is no slot with the specified name.

