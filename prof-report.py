import sys
import matplotlib.pyplot as plt

def gatherInfo(fileName, infoType):
    infoDict = dict()
    file = open(fileName)
    while line := file.readline().split():
        if line[0] == infoType:
            if line[1] in infoDict:
                infoDict[line[1]] += 1
            else:
                infoDict[line[1]] = 1
    return dict(sorted(infoDict.items(), key=lambda item: item[1]))
def main() -> int:
    if len(sys.argv) != 2:
        print("Error: require profiler dump file to be passed as command line argument")
        exit(-1)

    dumpFileName = sys.argv[1]

    instrs = gatherInfo(dumpFileName, "[OP]")
    funcs = gatherInfo(dumpFileName, "[F_START]")

    print(instrs)
    print(funcs)

    plt.title("Number of different instructions")
    plt.bar(instrs.keys(), instrs.values())
    plt.show()

    plt.title("Number of function calls")
    plt.bar(funcs.keys(), funcs.values())
    plt.show()

    return 0

if __name__ == '__main__':
    sys.exit(main())
