import pandas as pd
import numpy as np

def inferQubits(filename):
    df = pd.read_csv(filename)

    qubits = []
    for index, row in df.iterrows():
        if row['finalPosition_y'] >= 1230 and row['finalPosition_y'] <= 1610:
            qubits.append('top')
        elif row['finalPosition_y'] >= -90 and row['finalPosition_y'] <= 290:
            qubits.append('middle')
        elif row['finalPosition_y'] >= -1410 and row['finalPosition_y'] <= -1030:
            qubits.append('bottom')
        else:
            qubits.append('unknown')
    
    df['qubit'] = qubits        
    df.to_csv(filename, index=False)