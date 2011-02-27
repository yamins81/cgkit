# Vorberechnung für noise

from random import *
from cgtypes import *

out = open("noisetabs.h","w")

print >>out,"/* Tables with random values */\n\n"

# Permutation erzeugen
perm = range(256)
for i in range(256):
    j=randrange(256)
    dummy=perm[i]
    perm[i]=perm[j]
    perm[j]=dummy

print >> out, "static unsigned char perm[%d] = {\n  "%len(perm),
for i in range(len(perm)):
    s="%3d"%perm[i]
    if i<len(perm)-1:
        s+=","
    print >> out, s,
    if i%14==13:
        print >> out, "\n  ",
print >> out, "};\n"

# Zufallszahlen für cellnoise erzeugen
anz=256
print >> out, "static double uniform[%d] = {\n  "%anz,
for i in range(anz):
    s="%s"%repr(random())
    if i<anz-1:
        s+=","
    print >> out, s,
    if i%3==2:
        print >> out, "\n  ",
print >> out, "};"


# Gradienten erzeugen
def create_grads(dim):
    grads=[]
    for i in range(256):
        v=vec4(2,0,0)
        while v.length()>1.0:
            v=vec4(uniform(-1,1), uniform(-1,1), uniform(-1,1), uniform(-1,1))
            if dim<4:
                v[3]=0.0
            if dim<3:
                v[2]=0.0
            if dim<2:
                v[1]=0.0
        v=v.normalize()
        grads+=[v]

    print >> out, "\n"
    print >> out, "static double grads%d[%d][%d] = {"%(dim,len(grads),dim)
    for i in range(len(grads)):
        v=grads[i]
        if dim==1:
            s = "  {"+repr(v.x)+"}"
        if dim==2:
            s = "  {"+repr(v.x)+", "+repr(v.y)+"}"
        elif dim==3:
            s = "  {"+repr(v.x)+", "+repr(v.y)+", "+repr(v.z)+"}"
        elif dim==4:
            s = "  {"+repr(v.x)+", "+repr(v.y)+", "+repr(v.z)+", "+repr(v.w)+"}"
        if i<len(perm)-1:
            s+=","
        else:
            s+="};"
        print >> out, s


create_grads(2)
create_grads(3)
create_grads(4)
