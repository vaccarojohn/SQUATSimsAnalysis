import sys
from mergeCSV import merge

if __name__ == "__main__":
    filenames = []
    if len(sys.argv) == 6 and sys.argv[2] == "-f":
        filenames = [sys.argv[1] + "/" + sys.argv[3] + ".csv", sys.argv[1] + "/" + sys.argv[4] + ".csv"]
        
        merge(filenames, sys.argv[1] + "/" + sys.argv[5] + ".csv")
    elif len(sys.argv) == 6 and sys.argv[3] == "-t":
        for i in range(int(sys.argv[4]), int(sys.argv[5]) + 1):
            filenames.append(sys.argv[1] + "/" + sys.argv[2] + "_t" + str(i) + ".csv")

        merge(filenames, sys.argv[1] + "/" + sys.argv[2] + ".csv")
    else:
        print("Usage: python mergeCSVBatch.py [folder] [-f file1 file2 output] [filename -t start_thread end_thread].")