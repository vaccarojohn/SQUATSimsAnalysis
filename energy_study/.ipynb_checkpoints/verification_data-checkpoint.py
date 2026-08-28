import pandas
import numpy as np

infile_dir = '/sdf/home/j/jvaccaro/DMQIS-OLAF-G4CMP/energy_study/verification_data'
outfile_dir = 'graph_data'

# No 100 keV gammas arrived at the chip
data_ngamma = [0]

for i in range(1, 30):
    print("Loading file " + str(i + 1) + "/30...")
    
    dataframe = pandas.read_csv(infile_dir + '/QUIETFridgeCoarseOutput-' + str(i) + '_nt_QUIETFridgeCoarse_t0.csv')
    event_ids = dataframe.loc[:, 'eventID'].to_numpy()
    n = len(np.unique(event_ids))

    data_ngamma.append(n)

print("Writing to output...")
np.savez_compressed(outfile_dir + '/verification_data.npz', ngamma=data_ngamma)
    
print("Data successfully written to file verification_data.npz!")