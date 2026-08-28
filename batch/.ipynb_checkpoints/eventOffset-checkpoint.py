import pandas
import numpy as np

def eventOffset(filename, offset):
    print("Offsetting " + filename + "...")
    df = pandas.read_csv(filename)
    df['eventID'] += offset
    df.to_csv(filename, index=False)