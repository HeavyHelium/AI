import random
import math
from typing import List

def sigmoid(x: float) -> float:
    return 1 / (1 + math.e ** (-x))


class Neuron:
    """ 
    simple node in a neural network
    with sigmoid activation
    """
    MIN_PARAM_INIT = -0.05
    MAX_PARAM_INIT = 0.05

    def __init__(self, num_inputs: int, activation=False) -> None:
        self.weights: List[float] = [random.uniform(Neuron.MIN_PARAM_INIT, 
                                                    Neuron.MAX_PARAM_INIT) for _ in range(num_inputs)]
        self.b = random.uniform(Neuron.MIN_PARAM_INIT, 
                                Neuron.MAX_PARAM_INIT)
        self.activation = activation

    def __call__(self, x: List[float]) -> float:
        s: float = sum(w_i * x_i for w_i, x_i in zip(self.weights, x))
        return sigmoid(s + self.b) if self.activation else s + self.b

    def __repr__(self) -> str:
        return f"Neuron({self.parameters})"
    
    @property
    def parameters(self) -> List[float]:
        return self.weights + [self.b]

class Layer:
    def __init__(self, num_inputs: int, num_outputs: int, activation=False):
        self.neurons: List[Neuron] = [Neuron(num_inputs, activation) for _ in range(num_outputs)]

    def __call__(self, x: List[float]) -> List[float]:
        return [n(x) for n in self.neurons] if self.neurons else x

    @property
    def parameters(self) -> List[float]:
        return [p for n in self.neurons for p in n.parameters]

    def __repr__(self) -> str:
        return f"Layer with neurons: [{', '.join(str(n) for n in self.neurons)}]"

    def update(self, losses: List[float], 
                     inputs: List[float], 
                     alpha: float) -> None:
        for j in range(len(self.neurons)):
            # update weights
            for i in range(len(self.neurons[j].weights)):
                self.neurons[j].weights[i] += alpha * losses[j] * inputs[i]
            # update bias
            self.neurons[j].b += alpha * losses[j]

class NeuralNet:
    """
    Extremely simple 2 layer perceptron NN
    (3 layers, if you count the input)
    """

    ALPHA: float = 0.1 # learning rate

    def __init__(self, 
                 input_cnt: int, 
                 hidden_cnt: int, 
                 output_cnt: int) -> None:

        self.layers = [Layer(input_cnt, hidden_cnt, activation=True), 
                       Layer(hidden_cnt, output_cnt, activation=True) if hidden_cnt 
                       else Layer(input_cnt, output_cnt, activation=True)]
    
    def fit(self, X_train: List[List[float]], 
                  y_train: List[List[float]], 
                  epochs: int, 
                  alpha: float = None) -> None:
        
        alpha = NeuralNet.ALPHA if not alpha else alpha

        for _ in range(epochs):
            for x, y in zip(X_train, y_train):
                self.update(x, y, alpha)

    @staticmethod
    def loss_output(expected: float, predicted: float) -> float:
        return predicted * (1 - predicted) * (expected - predicted)

    def update(self, x: List[float], y: List[float], alpha: float) -> float:
        """
        Updates the paramaters given the entry (x, y)
        """
        a_ouput: float = self.predict(x)        
        a_hidden: List[float] = self.layers[0](x)

        output_losses: float = [NeuralNet.loss_output(t, a_out) for t, a_out in zip(y, a_ouput)]

        hidden_losses: List[float] = [a_h * (1 - a_h) * sum(self.layers[1].neurons[k].weights[h] * delta_k 
                                                            for k, delta_k in enumerate(output_losses))  
                                      for h, a_h in enumerate(a_hidden)] if len(self.layers[0].neurons) else []


        self.layers[1].update(output_losses, a_hidden, alpha)
        self.layers[0].update(hidden_losses, x, alpha)


    def predict(self, x: List[int]) -> float:
        """
        forward-prop calculation
        """
        for layer in self.layers:
            x = layer(x)
        
        assert len(x) == 1

        return x

    @classmethod
    def LogicalAnd(cls, epochs, alpha: float = None) -> "NeuralNet":
        X_train = [[1, 1], [1, 0], [0, 0], [0, 1]]
        y_train = [[1], [0], [0], [0]]

        nn = cls(input_cnt=2, hidden_cnt=0, output_cnt=1)
        nn.fit(X_train, y_train, epochs=epochs)

        return nn

    @classmethod
    def LogicalOr(cls, epochs, alpha: float = None) -> "NeuralNet":
        X_train = [[1, 1], [1, 0], [0, 0], [0, 1]]
        y_train = [[1], [1], [0], [1]]

        nn = cls(input_cnt=2, hidden_cnt=0, output_cnt=1)
        nn.fit(X_train, y_train, epochs=epochs)

        return nn

    @classmethod
    def LogicalXOR(cls, epochs, alpha: float = None) -> "NeuralNet":
        X_train = [[1, 1], [1, 0], [0, 0], [0, 1]]
        y_train = [[0], [1], [0], [1]]

        nn = cls(input_cnt=2, hidden_cnt=3, output_cnt=1)
        nn.fit(X_train, y_train, epochs=epochs)

        return nn

if __name__ == "__main__":
    X_test = [[1, 1], [1, 0], [0, 0], [0, 1]]

    land = NeuralNet.LogicalAnd(100000)
    for x, y in zip(X_test, [1, 0, 0, 0]):
        print(f"{x[0]} and {x[1]}: {land.predict(x)}")

    print()

    lor = NeuralNet.LogicalOr(100000)
    for x, y in zip(X_test, [1, 1, 0, 1]):
        print(f"{x[0]} or {x[1]}: {lor.predict(x)}")

    print()

    xor = NeuralNet.LogicalXOR(100000, alpha=0.01)
    for x, y in zip(X_test, [0, 1, 0, 1]):
        print(f"{x[0]} XOR {x[1]}: {xor.predict(x)}")    