import random
edgenum = 100
fi = open("H.hgr","w+")
fi.write(str(edgenum)+" 128 11\n")
def GetEdgeState(percent):
    key = random.randint(1,100)
    if(key<=percent):
        return 1
    return 0

def CreateInnerEdge():
    cid = random.randint(1,4)
    nodenum = random.randint(2,32)
    l = range((cid-1)*32+1,cid*32+1)
    random.shuffle(l)
    pice = random.sample(l,nodenum)
    fi.write("1")
    for elem in pice:
        fi.write(" "+str(elem))
    fi.write("\n")

def is_OutEdge(pice):
    cid = (pice[0]-1)/32+1
    for elem in pice:
        if (elem-1)/32+1 != cid:
            return True
    return False
    
def CreateOutEdge():
    nodenum = random.randint(2,32)
    l = range(1,128+1)
    random.shuffle(l)
    pice = random.sample(l,nodenum)
    while True:
        if is_OutEdge(pice):
            break
        random.shuffle(l)
        pice = random.sample(l,nodenum)
    fi.write("1")
    for elem in pice:
        fi.write(" "+str(elem))
    fi.write("\n")

def CreateEdge():
    state = GetEdgeState(10)
    if state == 0:
        print "inner"
        CreateInnerEdge()
    elif state == 1:
        print "out"
        CreateOutEdge()
    
def mainfunc():
    for i in range(0,edgenum):
        CreateEdge()
    for i in range(0,128):
        fi.write("1\n");
mainfunc()
fi.close()
