from preamble import *
from DRN_interface import *
from solar_mod import *
from pbarlike import *
import matplotlib.pyplot as plt

m_DM = 100.
bf = np.array([0,0,1,0,0,0,0,0])
sv = 10**(-25.5228)
pp = np.array([1.8, 1.79, 2.405, 2.357, 7.92e+03, 0.37, 2.05e+28, 0.419, 8.84, 0.09, 2.60])

pbar_flux_pred = pbar_flux(m_DM,sv,bf)
loglike = chi2(pbar_flux_pred)

print("chi2 = ",loglike)



plt.title('Cosmic Ray Antiproton Flux')
plt.plot(E_ams, E_ams**2.7 * pbar_flux_pred, c = 'b', linestyle = 'dashed', label = 'total predicted antiproton flux')
plt.plot(E_ams, E_ams**2.7 * phi_ams, c = 'g', linestyle = 'dotted', label = 'AMS antiproton flux')

plt.legend()
plt.xscale('log')
plt.yscale('log')
plt.ylabel('$\Phi \cdot E^{2.7}$ [GeV$^{1.7}$ m$^{-2}$ sr$^{-1}$ s$^{-1}$]')
plt.xlabel('E [GeV]')
plt.show()



