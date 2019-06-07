from random import randint
import sys

na = ord('a')
nz = ord('z')

def genWords(n=30,maxLength=10,sep=' ',f1=sys.stdout,f2=sys.stdout):
    dic = {}
    words = []
    for i in range(n):
        l = randint(1,maxLength)
        if l in dic:
            dic[l]+=1
        else:
            dic[l]=1
        words.append(genWord(l))
    for i in sorted(dic):
        print(i,dic[i],file=f2)
    s = sep.join(words)
    print(s,file=f1)
    return s


def genWord(l=5):
    return ''.join(genChar() for i in range(l))


def genChar():
    return chr(randint(na,nz))

if __name__ == '__main__':
    n = len(sys.argv)
    s1 = 'input.txt'
    s2 = 'result.txt'
    if n==3:
        s1 = sys.argv[1]
        s2 = sys.argv[2]
    elif n!=1:
        print(f'Usage: python3 {__file__} <words file> <result file>')
        exit(0)
    genWords(f1=open(s1,'w'),f2=open(s2,'w'))
    genWords()
