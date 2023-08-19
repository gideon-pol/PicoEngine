import math


G = 10

M1 = 1
M2 = 0.5

R = 2.5

v = math.sqrt(G * (M1 + M2) / R)
f = G * M1 * M2 / R**2

print(v)
print(f)