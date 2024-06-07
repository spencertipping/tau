import math
import numpy as np

def optimal_bloom_filter_params(n, p_list):
    results = []

    for p in p_list:
        # Calculating optimal m and k using the formulas
        m = - (n * math.log(p)) / (math.log(2) ** 2)
        k = (m / n) * math.log(2)

        bits_per_element = m / n
        false_positive_rate = p  # This is the chosen false positive rate

        results.append({
            'desired_false_positive_rate': p,
            'optimal_number_of_bits': m,
            'optimal_number_of_hashes': k,
            'bits_per_element': bits_per_element
        })

    return results

# Example usage
n = 65536  # number of elements
# Generate p values from 0.0001 to 0.5 along an exponential scale
p_list = np.geomspace(0.0001, 0.5, num=10)  # 10 points along the exponential scale

metrics = optimal_bloom_filter_params(n, p_list)
for result in metrics:
    print(f"Desired False Positive Rate: {result['desired_false_positive_rate']}")
    print(f"Optimal Number of Bits: {result['optimal_number_of_bits']}")
    print(f"Optimal Number of Hash Functions: {result['optimal_number_of_hashes']}")
    print(f"Bits per Element: {result['bits_per_element']}\n")
