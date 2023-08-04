import numpy as np
import math, copy


def sigmoid(z):
    """
    Computes the sigmoid function 1/(1+exp(-z))
    Args:
    z(scalar or ndarray): Input of the sigmoid function

    Returns:
    g_z(scalar or ndarray): Output of the sigmoid function
    """
    g_z = 1 / (1 + np.exp(-z))
    return g_z


def compute_gradient_logistic(X, y, w, b): 
    """
    Computes the gradient for linear regression 
    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values
    w(ndarray(n, )): model parameter
    b(scalar): model parameter 

    Returns: 
    dj_dw(ndarray(n, )): The gradient of the cost w.r.t. the parameters w.
    dj_db(scalar): The gradient of the cost w.r.t. the parameter b.
    """
    m, n = X.shape
    dj_dw = np.zeros(n) # the gradient of the cost w.r.t. the parameters w
    dj_db = 0.

    for i in range(m):
        f_wb_i = sigmoid(np.dot(X[i], w) + b)
        err_i = f_wb_i - y[i]
        for j in range(n):
            dj_dw[j] += err_i * X[i][j]
        dj_db += err_i

    dj_dw /= m
    dj_db /= m

    return dj_dw, dj_db
            

def gradient_descent(X, y, w_in, b_in, alpha, num_iters): 
    """
    Performs batch gradient descent

    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values 
    w_in(ndarray(n, )): Initial values of model parameters 
    b_in(scalar): Initial values of model parameter 
    alpha(scalar): number of iterations to run gradient descent 

    Returns: 
    w(ndarray(n, )): Updated values of parameters 
    b(scalar): Updated values of parameter
    """

    J_history = [] 
    w = copy.deepcopy(w_in)
    b = b_in 

    for i in range(num_iters): 
        dj_db, dj_dw = compute_gradient_logistic(X, y, w, b)
        w = w - alpha * dj_dw
        b = b - alpha * dj_db

        if i < 100000: 
            J_history.append(compute_cost_logistic(X, y, w, b))

        if i % math.ceil(num_iters / 10) == 0: # print cost every 10% of iterations
            print("Iteration: ", i, " Cost: ", J_history[-1])

    return w, b, J_history


