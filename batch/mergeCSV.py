import pandas
import numpy as np

def merge(filenames, output):
    dfs = []
    for filename in filenames:
        print("Merging " + filename + "...")
        dfs.append(pandas.read_csv(filename))

    combined = pandas.concat(dfs, ignore_index=True)
    combined.sort_values(by='eventID', ignore_index=True).to_csv(output, index=False)
