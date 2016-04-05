def GetAcc(filename):
    fi = open(filename,"r")
    content = fi.read()
    al = content.split("\n")
    cnt = 0
    pexit = {}
    tmppart = 0
    
    pm = {}
    for i in range(0,32):
        try:
            pm[al[i]] += 1
        except:
            pm[al[i]] = 1
    imax = 0
    for elem in pm:
        if pm[elem]>imax and elem not in pexit:
            imax = pm[elem]
            tmppart = elem
    pexit[tmppart] = 1
    
    cnt += imax
    
    pm = {}
    for i in range(32,64):
        try:
            pm[al[i]] += 1
        except:
            pm[al[i]] = 1
    imax = 0
    for elem in pm:
        if pm[elem]>imax and elem not in pexit:
            imax = pm[elem]
            tmppart = elem
    pexit[tmppart] = 1
    cnt += imax
    
    pm = {}
    for i in range(64,96):
        try:
            pm[al[i]] += 1
        except:
            pm[al[i]] = 1
    imax = 0
    for elem in pm:
        if pm[elem]>imax and elem not in pexit:
            imax = pm[elem]
            tmppart = elem
    pexit[tmppart] = 1
    cnt += imax
    
    pm = {}
    for i in range(96,128):
        try:
            pm[al[i]] += 1
        except:
            pm[al[i]] = 1
    imax = 0
    for elem in pm:
        if pm[elem]>imax and elem not in pexit:
            imax = pm[elem]
            tmppart = elem
    pexit[tmppart] = 1
    cnt += imax
    fi.close()
    return cnt

def mainfunc():
    p = [0,3,6,9,12,15,18,21,24,27,30,33]
    for elem in p:
        cnt = 0
        for i in range(0,100):
            filename = "..//output//%02dH%02d" % (i,elem)
            cnt += GetAcc(filename+"//partition.txt")
        print float(cnt)/(128*100)
mainfunc()
