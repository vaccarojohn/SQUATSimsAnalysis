import sys
from processCSV import process

if __name__ == "__main__":
    if len(sys.argv) == 3:
        process(sys.argv[1] + "/" + sys.argv[2] + ".csv")
    elif len(sys.argv) == 6 and sys.argv[3] == "-t":
        for i in range(int(sys.argv[4]), int(sys.argv[5]) + 1):
            process(sys.argv[1] + "/" + sys.argv[2] + "_t" + str(i) + ".csv")
    else:
        print("Usage: python processCSVBatch.py [folder] [filename] [-t start_thread end_thread].")
