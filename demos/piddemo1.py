######################################################################
# PID demo
######################################################################

# Create the materials for the target position markers
red = GLMaterial(diffuse=(0.5,0,0), emission=(0.2,0,0))
mark = GLMaterial(diffuse=(0.8,0.4,0.2), emission=(0.8,0.4,0.2))

# Create 3 spheres indicating the target positions
m3=Sphere(radius=0.1, pos=(0,0,0), material=red)
m2=Sphere(radius=0.1, pos=(1,0,0), material=mark)
m1=Sphere(radius=0.1, pos=(2,0,0), material=red)

# Create a cylinder
CCylinder(
    radius   = 0.02,
    length   = 2,
    pos      = (1,0,0),
    rot      = mat3().fromEulerXYZ(0,radians(90),0),
    material = GLMaterial(diffuse=(0.6,0.4,0), emission=(0.2,0.13,0))
)

# Create a ball that will slide along an ODE slider joint...
ball = Sphere(
    name      = "Ball",
    radius    = 0.2,
    mass      = 1.0,
    segmentsu = 16,
    segmentsv = 16,
    material  = GLMaterial(diffuse=(0.3,0.7,0.4), emission=(0.1,0.23,0.13))
)

joint = ODESliderJoint(
    body1 = ball
)
joint.motorfmax = 10.0
joint.motorvel = 0.1

# Create the PID controller that will receive the joint position as input
# and controls the motor velocity of the slider joint...
pid = PIDController(
    setpoint = 1.0,
    Kp = 5.0,
    Ki = 0,
    Kd = 0.2,
)

joint.position_slot.connect(pid.input_slot)
pid.output_slot.connect(joint.motorvel_slot)

# Create the dynamics components...
ode = ODEDynamics()
ode.add(ball)
ode.add(joint)

def key(e):
    """Callback for KEY_PRESS events.

    Press '1' to '4' to set a new set point on the PID controller.
    """
    
    global pid, m1, m2, m3
    if e.key=="1":
        pid.setpoint = 2.0
        m1.setMaterial(mark)
        m2.setMaterial(red)
        m3.setMaterial(red)
    elif e.key=="2":
        pid.setpoint = 1.0
        m1.setMaterial(red)
        m2.setMaterial(mark)
        m3.setMaterial(red)
    elif e.key=="3":
        pid.setpoint = 0.0
        m1.setMaterial(red)
        m2.setMaterial(red)
        m3.setMaterial(mark)
    elif e.key=="4":
        pid.setpoint = 2.0
        m1.setMaterial(red)
        m2.setMaterial(red)
        m3.setMaterial(red)

# Register the key() callback
eventmanager.connect(KEY_PRESS, key)

print "Press keys '1' to '4' to select a target position..."
