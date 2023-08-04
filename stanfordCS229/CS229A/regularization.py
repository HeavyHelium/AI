import numpy as np

def sigmoid(z): 
    return 1 / (1 + np.exp(-z))



def compute_cost_linear_reg(X, y, w, b, lambda_ = 1): 
    """
    Computes the cost over all examples
    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values
    w(ndawrray(n, )): model parameters
    b(scalar): model parameter
    lambda_(scalar): Controls amount of regularization 
    Returns: 
    total_cost(scalar): cost
    """
    
    m = X.shape[0]
    n = len(w)
    cost = 0.
    for i in range(m):
        f_wb_i = np.dot(X[i], w) + b
        cost += (f_wb_i - y[i]) ** 2
    cost /= (2 * m)

    # Add regularization amount
    reg_cost = 0 
    for j in range(n): 
        reg_cost += (w[j]**2)

    reg_cost = (lambda_ / (2 * m)) * reg_cost

    return cost + reg_cost



def compute_cost_logistic_reg(X, y, w, b, lambda_ = 1):
    """
    Computes the cost over all examples 
    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values
    w(ndarray(n, )): model parameters
    b(scalar): model parameter
    lambbda_(scalar): Controls the amount of regualarization
    Returns: 
    total_cost(scalar): cost
    """

    m, n = X.shape
    cost = 0.
    for i in range(m): 
        z_i = np.dot(X[i], w) + b
        f_wb_i = sigmoid(z_i)
        cost += -y[i]*np.log(f_wb_i) - (1 - y[i]) * np.log(1 - f_wb_i)
    cost /= m

    reg_cost = 0 
    for j in range(n):
        reg_cost += (w[j] ** 2)
    reg_cost = (lambda_ / (2 * m)) * reg_cost

    return cost + reg_cost



def compute_gradient_linear_reg(X, y, w, b, lambda_):
    """
    Computes the gradient for linear regresison 
    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values 
    w(ndarray(n, )): model parameters
    b(scalar): model paramter 
    lambda_(scalar): Controls the amount of regularization 

    Returns: 
    dj_dw(ndarray(n, )): The gradient of the cost w.r.t. the parameters w

    dj_db(scalar): The gradient of the cost w.r.t. the parameter b 
    """

    m, n = X.shape 
    dj_dw = np.zeros((n, ))
    dj_db = 0.

    for i in range(m): 
        err = (np.dot(X[i], w) + b) - y[i]
        for j in range(n):
            dj_dw[j] = dj_dw[j] + err * X[i, j]
        dj_db = dj_db + err

    dj_dw /= m  
    dj_db /= m

    for j in range(n): 
        dj_dw[j] += (lambda_ / m) * w[j]

    return dj_dw, dj_db



def compute_gradient_logistic_reg(X, y, w, b, lambda_):
    """
    Computes the gradient for linear regresison 
    Args: 
    X(ndarray(m, n)): Data, m examples with n features 
    y(ndarray(m, )): target values 
    w(ndarray(n, )): model parameters
    b(scalar): model paramter 
    lambda_(scalar): Controls the amount of regularization 

    Returns: 
    dj_dw(ndarray(n, )): The gradient of the cost w.r.t. the parameters w

    dj_db(scalar): The gradient of the cost w.r.t. the parameter b 
    """

    m, n = X.shape 
    dj_dw = np.zeros((n, ))
    dj_db = 0.

    for i in range(m): 
        f_wi = sigmoid(np.dot(X[i], w) + b)
        err = f_wi - y[i]
        for j in range(n):
            dj_dw[j] = dj_dw[j] + err * X[i, j]
        dj_db = dj_db + err

    dj_dw /= m  
    dj_db /= m

    for j in range(n): 
        dj_dw[j] += (lambda_ / m) * w[j]

    return dj_dw, dj_db



