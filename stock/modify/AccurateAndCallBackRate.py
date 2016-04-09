import json
import random
allcntmap1 = {}
allcntmap2 = {}
def GetComMap():
    fi = open("companies.json","r+")
    content = fi.read()
    coms = json.loads(content)
    fi.close()
    return coms

def dealline(line,coms):
    elems = line.split(" ")
    fieldsmap = {}
    totalcnt = 0
    for elem in elems:
        if elem=="":
            continue
        totalcnt += 1
        thisf = coms[elem][1]
        if thisf in fieldsmap:
            fieldsmap[thisf] += 1
        else:
            fieldsmap[thisf] = 1
    tmpdic = sorted(fieldsmap.iteritems(),key=lambda d:d[1],reverse=True)
    field = tmpdic[0][0]
    cnt = tmpdic[0][1]
    if field in allcntmap1:
        allcntmap1[field] += cnt
        allcntmap2[field] += totalcnt 
    else:
        allcntmap1[field] = cnt
        allcntmap2[field] = totalcnt 
    
def mainfunc():
    fi = open("ans.txt","r+")
    content = fi.read()
    lines = content.split("\n")
    fi.close()
    coms = GetComMap()
    for line in lines:
        if line=="":
            continue
        dealline(line,coms)
    for item in allcntmap1:
        print item,allcntmap1[item],allcntmap2[item],float(allcntmap1[item])/allcntmap2[item]
mainfunc()
