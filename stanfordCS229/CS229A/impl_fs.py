import numpy as np 
import matplotlib.pyplot as plt 



def zscore_normalize(X):
    """
    computes X, zscore normalized by column

    Args: 
    X (ndarray (m,n)):      input data, m examples, n features 
                             
    Returns:                
    X_norm (ndarray (m,n)): input normalized by column 
    mu (ndarray (n,)):      mean of each column
    sigma (ndarray (n,)):   standard deviation of each column 
    """
    mu = np.mean(X, axis=0)# axis 0 stands for column-wise 
    sigma = np.std(X, axis=0)

    X_norm = (X - mu) / sigma

    return (X_norm, mu, sigma)

