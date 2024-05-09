import numpy as np
import torch.nn as nn

from scipy.spatial.distance import cityblock

class SEClassifier:
    def __init__(self) -> None:
        pass

    def fit(self, train_vector: np.ndarray):
        self.mean_vector = np.mean(train_vector, axis=0)
    
    def decision_function(self, test_vector):
        assert test_vector.shape[1] == self.mean_vector.shape[0]

        return np.linalg.norm(test_vector - self.mean_vector, axis=1)[0]

class SMClassifier:
    def __init__(self) -> None:
        pass

    def fit(self, train_vector: np.ndarray):
        self.mean_vector = np.mean(train_vector, axis=0)
    
    def decision_function(self, test_vector):
        assert test_vector.shape[1] == self.mean_vector.shape[0]

        decision_scores = []

        for i in range(test_vector.shape[0]):
            decision_scores.append(cityblock(test_vector[i], self.mean_vector))

        return np.array(decision_scores)
