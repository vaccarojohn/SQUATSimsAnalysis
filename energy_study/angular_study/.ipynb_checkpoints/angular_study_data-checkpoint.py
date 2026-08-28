import math
import pandas as pd
import numpy as np

infile_dir = '/sdf/home/j/jvaccaro/DMQIS/DMQIS-OLAF-G4CMP/energy_study/angular_study/g4_data'
angles = [0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 255, 270, 285, 300, 315, 330, 345]
outfile_dir = 'graph_data'

N = 8000000000
data_energydep = []
data_energydepevts = []
data_energydep_std = []
data_energydepevts_std = []
data_ngamma = []
data_ngamma_std = []

for theta in angles:
    dataframe = pd.read_csv(infile_dir + '/OLAFOutput-theta' + str(theta) + '.csv')
    event_ids = dataframe.loc[:, 'eventID'].to_numpy()
    edep_keV = dataframe.loc[:, 'edep_keV'].to_numpy()
    particle_names = dataframe.loc[:, 'particleName'].to_numpy()

    nonzero_mask = (edep_keV > 0)
    n = len(np.unique(event_ids[nonzero_mask]))
    energydep = np.sum(edep_keV)    
    
    data_energydep.append(energydep)
    data_energydepevts.append(n)
    
    energydep_std = np.sqrt(np.sum((edep_keV[nonzero_mask] - energydep / n) ** 2)) / n
    data_energydep_std.append(energydep_std)

    energydep_ratio = n / N
    data_energydepevts_std.append(math.sqrt(energydep_ratio * (1 - energydep_ratio) / N))

    gamma_mask = particle_names == 'gamma'
    n_gamma = len(np.unique(event_ids[gamma_mask]))
    data_ngamma.append(n_gamma)
    
    gamma_ratio = n_gamma / N
    data_ngamma_std.append(math.sqrt(gamma_ratio * (1 - gamma_ratio) / N))

print("Writing to output...")
np.savez_compressed(outfile_dir + '/angular_data.npz', energydep=data_energydep, energydepevts=data_energydepevts, energydep_std=data_energydep_std, 
                    energydepevts_std=data_energydepevts_std, ngamma=data_ngamma, ngamma_std=data_ngamma_std)
    
print("Data successfully written to file angular_data.npz!")