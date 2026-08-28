import pandas
import numpy as np

infile_dir = '/sdf/home/j/jvaccaro/DMQIS-OLAF-G4CMP/energy_study/co60_data'
outfile_dir = 'graph_data'

for i in range(1, 30):
    print("Loading file " + str(i + 1) + "/30...")
    
    dataframe = pandas.read_csv(infile_dir + '/QUIETFridgeCoarseOutput-' + str(i) + '_nt_QUIETFridgeCoarse_t0.csv')
    event_ids = dataframe.loc[:, 'eventID'].to_numpy()
    edep_keV = dataframe.loc[:, 'edep_keV'].to_numpy()
    n = len(np.unique(event_ids))
    
    data_energydep.append(np.sum(edep_keV))
    data_energydepevts.append(n)

    gamma_mask = dataframe.loc[:, 'particleName'].to_numpy() == 'gamma'
    n_gamma = len(np.unique(event_ids[gamma_mask]))

    data_gammadep.append(np.sum(edep_keV[gamma_mask]))
    data_gammadepevts.append(n_gamma)

print("Writing to output...")
np.savez_compressed(outfile_dir + '/energy_study_data.npz', energydep=data_energydep, energydepevts=data_energydepevts,
                                                            gammadep=data_gammadep, gammadepevts=data_gammadepevts)
    
print("Data successfully written to file energy_study_data.npz!")