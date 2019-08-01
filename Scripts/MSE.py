import matplotlib.pyplot as plt

original_vectors_file = open("original_vec.txt", "r")

approx_vectors_file = open("approx_vec.txt", "r")

original_vectors = []
approx_vectors = []

for line in original_vectors_file:
    str_vec = line.split(",")
    vec = [float(s) for s in str_vec[0:-1]]
    original_vectors.append(vec)


for line in approx_vectors_file:
    str_vec = line.split(", ")
    vec = [float(s) for s in str_vec[0:-1]]
    approx_vectors.append(vec)

print(original_vectors)
print(approx_vectors)

mses = []

for v1, v2 in zip(original_vectors, approx_vectors):
    diff = [x1 - x2 for (x1, x2) in zip(v1, v2)]
    mse = sum([d ** 2 for d in diff]) / len(v1)
    mses.append(mse)

print(mses)


plt.plot(range(1, len(mses) + 1), mses)
plt.xlabel("GD Iteration")
plt.ylabel("MSE")
plt.title("MSE w.r.t GD iteration")
plt.show()
