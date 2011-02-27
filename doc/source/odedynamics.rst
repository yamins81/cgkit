.. % ODEDynamics component


:class:`ODEDynamics` --- Rigid body dynamics using the Open Dynamics Engine
===========================================================================


.. class:: ODEDynamics(name = "ODEDynamics",  gravity = 9.81,  substeps = 1,  enabled = True,  erp = None,  cfm = None,  defaultcontactproperties = None,  collision_events = False,  auto_add = False,  show_contacts = False,  contactmarkersize = 0.1,  contactnormalsize = 1.0,  auto_insert = True)

   *gravity* is the acceleration due to gravity. The direction of the acceleration
   is in negative "up" direction (as specified by the scene).

   *substeps* is the number of simulation steps per frame. You can increase this
   value to get a more accurate/stable simulation.

   The simulation will only run if *enabled* is ``True``, otherwise it's halted.

   *erp* and *cfm* are the global error reduction parameter and constraint force
   mixing value to be useed (see the ODE manual).

   *defaultcontactproperties* is a :class:`ODEContactProperties` object that
   specifies the default contact parameters. These parameters are used for contacts
   between two objects (resp. materials) that have not been set explicitly using
   :meth:`setContactProperties`.

   *collision_events* determines whether the component will generate
   ``ODE_COLLISION`` events whenever a collision has occured. An event handler
   takes a :class:`ODECollisionEvent` (see section  :ref:`odecollisionevent`)
   object as argument.

   If *auto_add* is ``True`` the component searches the scene for rigid bodies and
   hinges and adds them automatically to the component. This is done at the time
   the component is created, so any bodies or hinges created afterwards will be
   ignored.

   *show_contacts* determines whether the contact points and normals are visualized
   or not (this is mainly for debugging purposes). The size of the contact point
   markers and the length of the normals can be specified via the
   *contactmarkersize* and *contactnormalsize* arguments.


.. method:: ODEDynamics.add(objects, categorybits=None, collidebits=None)

   Add world objects to the simulation. *objects* can be a single object or a
   sequence of objects. An object may be specified by its name or the object
   itself. *categorybits* and *collidebits* are long values that control which
   objects can collide with which other object. The specified category and collide
   bits are assigned to every object in *objects*. Each bit in *categorybits*
   represents one category the objects belong to. *collidebits* is another bit
   field that specifies with which categories the objects may collide. By default,
   every bit is set in both values.


.. method:: ODEDynamics.reset()

   Reset the state of the simulated bodies. All bodies will be set to the position
   and velocity they had when they were added to the simulation. This method is
   also called when the RESET event is issued.


.. method:: ODEDynamics.setContactProperties((mat1, mat2), props)

   Set the contact properties for a material pair. *mat1* and *mat2* are two
   :class:`Material` objects and props is a :class:`ODEContactProperties` object
   describing the contact properties.


.. method:: ODEDynamics.getContactProperties((mat1, mat2))

   Return the contact properties for a material pair. The order of the materials is
   irrelevant. The return value is a :class:`ODEContactProperties` object. A
   default property object is returned if the pair does not have any properties
   set.


.. method:: ODEDynamics.createBodyManipulator(object)

   Return an :class:`ODEBodyManipulator` object that can be used to apply external
   forces/torques to the world object *object*.

.. note::

   To use the :class:`ODEDynamics` component the `PyODE
   <http://pyode.sourceforge.net/>`_ module has to be  installed on your system
   which wraps the  `Open Dynamics Engine <http://www.ode.org/>`_.

.. % ------------------------------------------------------


:class:`ODEContactProperties` --- Contact properties during collision
---------------------------------------------------------------------

The :class:`ODEContactProperties` class contains all the parameters that are
used when two objects collide.


.. class:: ODEContactProperties(mode = 0, mu = 0.3, mu2 = None, bounce = None, bounce_vel = None, soft_erp = None, soft_cfm = None, motion1 = None, motion2 = None, slip1 = None, slip2 = None, fdir1 = None)

   See the ODE manual (chapter  `7.3.7 Contact <http://ode.org/ode-latest-
   userguide.html#sec_7_3_7>`_) for an explanation of these parameters.

   .. note::

      You only have to specify the *mode* argument if you want to set the
      ContactApprox\* flags. The other flags are automatically set.

.. % ------------------------------------------------------


:class:`ODEBodyManipulator` --- Apply external forces/torques to bodies
-----------------------------------------------------------------------

The :class:`ODEBodyManipulator` class can be used to apply external forces and
torques to a rigid body.


.. class:: ODEBodyManipulator

   You get an instance of this class by calling the :meth:`createBodyManipulator`
   method of the :class:`ODEDynamics` component. One particular body manipulator
   instance is always associated with one particular rigid body. A manipulator
   object has the following attributes and methods:


.. attribute:: ODEBodyManipulator.body

   This attribute contains the rigid body (:class:`WorldObject`) this manipulator
   is associated with. You can only read this attribute. If you want to control
   another body, use the :meth:`createBodyManipulator` method of the dynamics
   component.


.. attribute:: ODEBodyManipulator.odebody

   This is the Body instance of the PyODE module. You can use this object if you
   want to access special features of ODE that are not exposed otherwise. But note
   that you won't get the expected results if you call methods like
   :meth:`addForce` directly on the ODE body and you're using more than one sub
   step in your simulation. The force would only be applied during the first sub
   step because it is reset after each step. Use this manipulator class instead,
   that's what it's for.

.. % addForce


.. method:: ODEBodyManipulator.addForce(force, relforce=False, pos=None, relpos=False)

   Add an external force to the current force vector. *force* is a vector
   containing the force to apply. If *relforce* is ``True`` the force is
   interpreted in local object space, otherwise it is assumed to be given in global
   world space. By default, the force is applied at the center of gravity. You can
   also pass a different position in the *pos* argument which must describe a point
   in space. *relpos* determines if the point is given in object space or world
   space (default).

.. % addTorque


.. method:: ODEBodyManipulator.addTorque(torque, reltorque=False)

   Add an external torque to the current torque vector. *torque* is a vector
   containing the torque to apply. *reltorque* determines if the torque vector is
   given in object space or world space (default).

.. % setInitialPos


.. method:: ODEBodyManipulator.setInitialPos(pos)

   Set the initial position of the body. *pos* must be a 3-sequence of  floats
   containing the new position.

.. % setInitialRot


.. method:: ODEBodyManipulator.setInitialRot(rot)

   Set the initial orientation of the body. *rot* must be a :class:`mat3`
   containing a rotation matrix.

.. % setInitialLinearVel


.. method:: ODEBodyManipulator.setLinearVel(vel)

   Set the initial linear velocity of the body. *vel* must be a 3-sequence of
   floats containing the new velocity.

.. % setInitialAngularVel


.. method:: ODEBodyManipulator.setAngularVel(vel)

   Set the initial angular velocity of the body. *vel* must be a 3-sequence of
   floats containing the new velocity.

.. % setPos


.. method:: ODEBodyManipulator.setPos(pos)

   Set the position of the body. *pos* must be a 3-sequence of floats containing
   the new position.

.. % setRot


.. method:: ODEBodyManipulator.setRot(rot)

   Set the orientation of the body. *rot* must be a :class:`mat3` containing a
   rotation matrix.

.. % setLinearVel


.. method:: ODEBodyManipulator.setLinearVel(vel)

   Set the linear velocity of the body. *vel* must be a 3-sequence of floats
   containing the new velocity.

.. % setAngularVel


.. method:: ODEBodyManipulator.setAngularVel(vel)

   Set the angular velocity of the body. *vel* must be a 3-sequence of floats
   containing the new velocity.

.. % ------------------------------------------------------


.. _odecollisionevent:

:class:`ODECollisionEvent` --- Collision event object
-----------------------------------------------------

An :class:`ODECollisionEvent` object is passed as argument to the event handler
for ``ODE_COLLISION`` events.


.. class:: ODECollisionEvent(obj1, obj2, contacts, contactproperties)

   *obj1* and *obj2* are the two world objects that have collided with  each other.

   *contacts* is a list of :class:`ode.Contact` objects that each describes a
   contact point.

   *contactproperties* is a :class:`ODEContactProperties` object that describes the
   properties of the contact. It depends on the materials of the *obj1* and *obj2*.
   The event handler may modify this object to change the result of the collision.
   Note however, that the changes will be permanent and also affect later
   collisions.

.. % averageContactGeom


.. method:: ODECollisionEvent.averageContactGeom()

   Return the average contact position, normal and penetration depth (in this
   order). The position and normal are returned as :class:`vec3` objects, the
   penetration depth is a float.

