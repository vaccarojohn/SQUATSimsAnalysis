import sys
from eventOffset import eventOffset

if __name__ == "__main__":
    if len(sys.argv) == 4:
        eventOffset(sys.argv[1] + "/" + sys.argv[2] + ".csv", int(sys.argv[3]))
    elif len(sys.argv) == 7 and sys.argv[4] == "-t":
        for i in range(int(sys.argv[5]), int(sys.argv[6]) + 1):
            eventOffset(sys.argv[1] + "/" + sys.argv[2] + "_t" + str(i) + ".csv", int(sys.argv[3]))
    else:
        print("Usage: python eventOffsetBatch.py [folder] [filename] [offset] [-t start_thread end_thread].")
