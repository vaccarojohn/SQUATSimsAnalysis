import numpy as np

# Source: https://www.nist.gov/pml/x-ray-mass-attenuation-coefficients

gamma_energies_keV = np.array([100, 150, 200, 300, 400, 500, 600, 800, 1000, 1250, 1500, 2000, 3000])

aluminum_mass_attenuation = np.array([0.1704, 0.1378, 0.1223, 0.1042, 0.09276, 0.08445, 0.07802, 0.06841, 0.06146, 0.05496, 0.05006, 0.04324, 0.03541]) # cm^2 / g
aluminum_density = 2.699 # g / cm^3
aluminum_thickness = (22 + 10) * 0.1 # cm

copper_mass_attenuation = np.array([0.4584, 0.2217, 0.1559, 0.1119, 0.09413, 0.08362, 0.07625, 0.06605, 0.05901, 0.05261, 0.04803, 0.04205, 0.03599]) # cm^2 / g
copper_density = 8.960 # g / cm^3
copper_thickness = (10 + 8 + 8 + 8 + 1.02 + 15.2) * 0.1 # cm

nickel_mass_attenuation = np.array([0.4440, 0.2208, 0.1582, 0.1154, 0.09765, 0.08698, 0.7944, 0.06891, 0.06160, 0.05494, 0.0515, 0.04387, 0.03745]) # cm^2 / g
detector_mat_density = 8.7 # g / cm^3
detector_mat_thickness = 0 # cm

attenuation_factors = np.exp(-aluminum_mass_attenuation * aluminum_density * aluminum_thickness - copper_mass_attenuation * copper_density * copper_thickness
                             - nickel_mass_attenuation * detector_mat_density * detector_mat_thickness)
    