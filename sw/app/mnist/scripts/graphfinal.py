import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
num_cycles_history = [2353088, 2353088, 2353089, 2353172, 2353646, 2755100]
cache_history = [[8192, 8192], [4096, 8192], [2048, 8192], [1024, 8192], [512, 8192], [128, 8192]]

# Load the data
num_cycles_history = [2353088, 2353088, 2353089, 2353172, 2353646, 2755100]

# Create a list of cache sizes in KB
cache_sizes_kb = [x[0] for x in cache_history]

# Fit the logarithmic regression model
def log_regression(x, a, b):
    return a * np.log(x) + b

params, _ = curve_fit(log_regression, cache_sizes_kb, num_cycles_history)

# Calculate the R-squared value
from sklearn.metrics import r2_score

r_squared = r2_score(num_cycles_history, log_regression(cache_sizes_kb, *params))

print("R-squared:", r_squared)

# Create the logarithmic regression graph
plt.figure(figsize=(10, 6))

plt.semilogy(cache_sizes_kb, num_cycles_history, 'o', label='Data points')
plt.semilogy(cache_sizes_kb, log_regression(cache_sizes_kb, *params), 'b-', label='Logarithmic regression')

plt.xlabel('Cache size (KB)')
plt.ylabel('Number of cycles (x10^6)')
plt.title('Logarithmic regression of number of cycles on cache size')
plt.legend()

plt.tight_layout()
plt.show()