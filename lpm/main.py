import sys
import os
urlval = ""
OUTDIR = f"./out/"
LISTDIR = "./list"

def main(a1):
    splittext = (open(LISTDIR))
    splittext = splittext.read()
    splittext = splittext.split('|')
    loc = splittext.index(a1)
    tloc = loc + 1
    #print(tloc)
    url = (splittext[tloc]).strip()
    urlval = (url.split('/'))[-1]
    os.system(f"wget {url}")
    os.system(f"cp {urlval} {OUTDIR}")
    os.system(f"rm -rf {urlval}")
    os.system(f"chmod +x {OUTDIR}{urlval}")


main(sys.argv[1])