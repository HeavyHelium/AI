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
        self.neurons: List[Neurons] = [Neuron(num_inputs, activation) for _ in range(num_outputs)]

    def __call__(self, x: List[float]) -> List[float]:
        return [n(x) for n in self.neurons]

    @property
    def parameters(self) -> List[float]:
        return [p for n in self.neurons for p in n.parameters]

    def __repr__(self) -> str:
        return f"Layer with neurons: [{', '.join(str(n) for n in self.neurons)}]"


class NeuralNet:
    """
    Extremely simple 2 layer perceptron NN
    """

    ALPHA: float = 0.05

    def __init__(self, hidden_cnt: int) -> None:
        self.layers = [Layer(2, hidden_cnt, activation=True), 
                       Layer(hidden_cnt, 1, activation=True)]
    
    def fit(self, X_train: List[List[int]], 
                  y_train: List[List[int]], 
                  epochs: int = 10000) -> None:
        pass 

    @staticmethod
    def loss_output(expected: float, predicted: float) -> float:
        return predicted * (1 - predicted) * (expected - predicted)

    def loss_hidden(n: Neuron, 
                    inp: List[int], 
                    errors: List[float]) -> float:
        
        val = n(inp)
        return val * (1 - val) * n(errors)

    def update(self, x, y) -> float:
        """
        :Returns: the loss of outputnode
        """
        a_ouput: float = self.predict(x)        
        a_hidden: List[float] = self.layers[0](x)

        output_losses: float = [NeuralNet.loss_output(t, a_out) for t, a_out in zip(y, a_ouput)]
        hidden_losses: List[float] = [a_h * (1 - a_h) * 1 * sum(self.layers[1].neurons[k].weights[h] 
                                                                 for k, delta_k in enumerate(output_losses))  
                                      for h, a_h in enumerate(a_hidden)]





    def predict(self, x: List[int]) -> float:
        """
        forward-prop calculation
        """
        for layer in self.layers:
            x = layer(x)
        
        assert len(x) == 1

        return x[0]



if __name__ == "__main__":
    l = Layer(2, 3)
    print(l.parameters)

    print(l.parameters)






    