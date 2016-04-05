import random
import os

edgenum = 100

def GetEdgeState(percent):
    key = random.randint(1,100)
    if(key<=percent):
        return 1
    return 0

def CreateInnerEdge(fi):
    cid = random.randint(1,4)
    nodenum = random.randint(2,32)
    l = range((cid-1)*32+1,cid*32+1)
    random.shuffle(l)
    pice = random.sample(l,nodenum)
    fi.write("1")
    for elem in pice:
        fi.write(" "+str(elem))
    fi.write("\n")
    return nodenum

def is_OutEdge(pice):
    cid = (pice[0]-1)/32+1
    for elem in pice:
        if (elem-1)/32+1 != cid:
            return True
    return False
    
def CreateOutEdge(fi):
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
    return nodenum

def CreateEdge(percent,fi):
    state = GetEdgeState(percent)
    nodenum = 0
    if state == 0:
        #print "inner"
        nodenum = CreateInnerEdge(fi)
        return [1,nodenum]
    elif state == 1:
        #print "out"
        nodenum = CreateOutEdge(fi)
        return [0,nodenum]
    
def oneloop(loop,menufile):
    plist = [0,3,6,9,12,15,18,21,24,27,30,33]
    for p in plist:
        filepath = "%02dH%02d" % (loop,p)
        menufile.write("path:..//testcases//%02dH%02d 2 10 10 1 1 1 0 0\n" % (loop,p))
        try:
            os.mkdir(filepath)
        except:
            print "err"
            
        fi = open(filepath+"//Hgraph.hgr","w+")
        fi.write(str(edgenum)+" 128 11\n")
        
        incnt = 0
        nodecnt = 0
        for i in range(0,edgenum):
            ans = CreateEdge(p,fi)
            incnt += ans[0]
            nodecnt += ans[1]
        #print incnt,edgenum-incnt,float(nodecnt)/edgenum
        
        for i in range(0,128):
            fi.write("1\n");
        fi.close()
        
def mainfunc():
    loop = 100
    fi = open("..//testcases_menu.txt","w+")
    for i in range(0,100):
        oneloop(i,fi)
    fi.close()       
mainfunc()
