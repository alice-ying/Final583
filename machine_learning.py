import csv
import numpy as np
from sklearn import svm
from sklearn.metrics import accuracy_score


def generate_training_data():
    x_train = np.empty((0, 8))
    y_train = np.empty((0, 2))
    x_test = np.empty((0, 8))
    y_test = np.empty((0, 2))
    direc = '/home/ncchuang/Final583/benchmarks/'

    train_benchmark_folders = ['automotive/basicmath/']
    test_benchmark_folders = ['automotive/basicmath/']

    for folder in train_benchmark_folders:
        heuristics_path = direc + folder + 'heuristics.csv'
        true_prob_path = direc + folder + 'true_probability.csv'

        with open(heuristics_path, 'r') as f:
            heuristics_data = list(csv.reader(f, delimiter=","))
        heuristics_data = np.array(heuristics_data)

        with open(true_prob_path, 'r') as f:
            probability_data = list(csv.reader(f, delimiter=","))
        probability_data = np.array(probability_data)

        x_train = np.concatenate((x_train, heuristics_data), axis=0)
        y_train = np.concatenate((y_train, probability_data), axis=0)

    for folder in test_benchmark_folders:
        heuristics_path = direc + folder + 'heuristics.csv'
        true_prob_path = direc + folder + 'true_probability.csv'

        with open(heuristics_path, 'r') as f:
            heuristics_data = list(csv.reader(f, delimiter=","))
        heuristics_data = np.array(heuristics_data)

        with open(true_prob_path, 'r') as f:
            probability_data = list(csv.reader(f, delimiter=","))
        probability_data = np.array(probability_data)

        x_test = np.concatenate((x_test, heuristics_data), axis=0)
        y_test = np.concatenate((y_test, probability_data), axis=0)

    y_train = y_train[:, 0]
    y_test = y_test[:, 0]
    print(y_train)
    print(y_test)

    for i in range(len(y_train)):
        y_train[i] = round((float)(y_train[i]), 0)
    for i in range(len(y_test)):
        y_test[i] = round((float)(y_test[i]), 0)
    print(y_train)
    print(y_test)

    return x_train, y_train, x_test, y_test


def model(x_train, y_train, x_test, y_test):
    clf = svm.SVC(gamma=0.001, C=100.)
    clf.fit(x_train, y_train)
    y_pred = clf.predict(x_test)
    return accuracy_score(y_pred, y_test)


def main():
    x_train, y_train, x_test, y_test = generate_training_data()
    acc = model(x_train, y_train, x_test, y_test)
    print(acc)



if __name__ == "__main__":
    main()