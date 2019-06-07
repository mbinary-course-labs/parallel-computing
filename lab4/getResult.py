import re
import sys

def count(f):
    with open(f) as fp:
        s = fp.read()
        s = re.sub(r'\r+',' ',s)
        s = re.sub(r'\n+',' ',s).strip()
        words = re.split(r' +',s)
        dic = {}
        for w in words:
            n = len(w)
            if n in dic:
                dic[n].append(w)
            else:
                dic[n]=[w]
        for i in sorted(dic):
            print(i,len(dic[i]), dic[i])


if __name__ == '__main__':
    count(sys.argv[1])
