from preamble import *
from DRN_interface import *
from solar_mod import *
from pbarlike import *
import matplotlib.pyplot as plt

m_DM = np.array([100.,200.])
bf = np.array([0,0,1,0,0,0,0,0])
sv = 10**(-25.5228)
pp = np.array([1.8, 1.79, 2.405, 2.357, 7.92e+03, 0.37, 2.05e+28, 0.419, 8.84, 0.09, 2.60])

pbar_flux_pred,pbar_CR = pbar_flux(m_DM,sv,bf)
chi2_DMCR,chi2_CR = chi2_array(pbar_flux_pred,pbar_CR)
del_chi = del_chi2(chi2_DMCR,chi2_CR)
print(np.shape(pbar_flux_pred))

# print(pbar_flux_pred[0])

print("chi2 = ",del_chi)



plt.title('Cosmic Ray Antiproton Flux')
plt.plot(E_ams, E_ams**2.7 * pbar_flux_pred[0][0], c = 'b', linestyle = 'dashed', label = 'total predicted + solar modulated antiproton flux')
plt.plot(E_ams, E_ams**2.7 * phi_ams, c = 'g', linestyle = 'dotted', label = 'AMS antiproton flux')

plt.legend()
plt.xscale('log')
plt.yscale('log')
plt.ylabel('$\Phi \cdot E^{2.7}$ [GeV$^{1.7}$ m$^{-2}$ sr$^{-1}$ s$^{-1}$]')
plt.xlabel('E [GeV]')
plt.show()
