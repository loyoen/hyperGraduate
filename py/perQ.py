Q = {}
def mainfunc():
    fi = open("..//res.txt","r")
    content = fi.read()
    lines = content.split("\n")
    for line in lines:
        elems = line.split("\t")
        ids = elems[0].split("H")
        if len(ids) < 2:
            continue
        try:
            Q[ids[1]] += float(elems[2])
        except:
            Q[ids[1]] = float(elems[2])
    for item in Q:
        print item," : ",Q[item]/100
mainfunc()
