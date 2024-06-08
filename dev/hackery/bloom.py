import math
import sys

def optimal_bloom_filter_params(n, p):
    # Calculating optimal m and k using the formulas
    m = - (n * math.log(p)) / (math.log(2) ** 2)
    k = (m / n) * math.log(2)

    bits_per_element = m / n
    false_positive_rate = p  # This is the chosen false positive rate

    print(f'bits: {m}, hashes: {k}, bpe: {bits_per_element}')

for p in sys.stdin:
  optimal_bloom_filter_params(65536, float(p))
