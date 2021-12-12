import csv
import numpy as np
from sklearn import svm
from sklearn.metrics import accuracy_score, precision_score, recall_score


def generate_training_data():
    x_train = np.empty((0, 8))
    y_train = np.empty((0, 2))
    x_test = np.empty((0, 8))
    y_test = np.empty((0, 2))
    direc = '/home/acying/Final583/benchmarks/'

    train_benchmark_folders = ['automotive/basicmath/', 'automotive/bitcount/', 'automotive/qsort/', 'automotive/susan/', 'network/dijkstra/', 'network/patricia/']
    test_benchmark_folders = ['automotive/basicmath/']

    not_applicable = np.array(['0.5'] * 8)

    for folder in train_benchmark_folders:
        heuristics_path = direc + folder + 'heuristics.csv'
        true_prob_path = direc + folder + 'true_probability.csv'

        with open(heuristics_path, 'r') as f:
            heuristics_data = list(csv.reader(f, delimiter=","))
        heuristics_data = np.array(heuristics_data)

        with open(true_prob_path, 'r') as f:
            probability_data = list(csv.reader(f, delimiter=","))
        probability_data = np.array(probability_data)

        not_applicable_idxs = np.where(np.all(heuristics_data == not_applicable, axis=1))
        heuristics_data = np.delete(heuristics_data, not_applicable_idxs, axis=0).astype(np.float)
        probability_data = np.delete(probability_data, not_applicable_idxs, axis=0).astype(np.float)

        x_train = np.concatenate((x_train, heuristics_data), axis=0)
        y_train = np.concatenate((y_train, probability_data), axis=0)
    
    print(x_train.shape)

    for folder in test_benchmark_folders:
        heuristics_path = direc + folder + 'heuristics.csv'
        true_prob_path = direc + folder + 'true_probability.csv'

        with open(heuristics_path, 'r') as f:
            heuristics_data = list(csv.reader(f, delimiter=","))
        heuristics_data = np.array(heuristics_data)

        with open(true_prob_path, 'r') as f:
            probability_data = list(csv.reader(f, delimiter=","))
        probability_data = np.array(probability_data)

        x_test = np.concatenate((x_test, heuristics_data.astype(np.float)), axis=0)
        y_test = np.concatenate((y_test, probability_data.astype(np.float)), axis=0)

    y_train = y_train[:, 0]
    y_test = y_test[:, 0]

    for i in range(len(y_train)):
        y_train[i] = round((float)(y_train[i]), 0)
    for i in range(len(y_test)):
        y_test[i] = round((float)(y_test[i]), 0)

    return x_train, y_train, x_test, y_test


def model(x_train, y_train, x_test, y_test):
    clf = svm.SVC(gamma=0.001, C=100.)
    clf.fit(x_train, y_train)
    y_pred = clf.predict(x_test)

    precision = precision_score(y_test, y_pred)
    recall = recall_score(y_test, y_pred)
    accuracy = accuracy_score(y_test, y_pred)
    return precision, recall, accuracy


def main():
    x_train, y_train, x_test, y_test = generate_training_data()
    precision, recall, accuracy = model(x_train, y_train, x_test, y_test)
    print("PRECISION: ", precision)
    print("RECALL: ", recall)
    print("ACCURACY: ", accuracy)
    



if __name__ == "__main__":
    main()