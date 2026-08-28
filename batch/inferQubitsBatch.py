import sys
from inferQubits import inferQubits

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python inferQubitsBatch.py [folder] [filePrefix]")
    else:            
        inferQubits(sys.argv[1] + "/" + sys.argv[2] + ".csv")
