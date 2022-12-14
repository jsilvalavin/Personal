# Elwin van 't Wout
# Pontificia Universidad Católica de Chile
# 7-10-2022

# Generate a matrix with real eigenvalues uniformly located in a specific
# range and store the matrix in a text file.

import numpy as np

def generate_matrix(dim):
    from scipy.stats import ortho_group
    from scipy.sparse import spdiags
    a = ortho_group.rvs(dim, random_state=0)
    b = np.linspace(1., 10., dim)
    return a @ spdiags(b, 0, dim, dim) @ a.T

def write_matrix(matrix, filename='matrix.txt'):
    with open(filename, 'w') as f:
        f.write(str(matrix.shape[0])+"\n"+str(matrix.shape[1])+"\n")
        for i in range(matrix.shape[0]):
            for j in range(matrix.shape[1]):
                f.write(str(matrix[i,j])+"\n")
        f.write("end")

ndim = 1000
mat = generate_matrix(ndim)
write_matrix(mat)
