import mykmeanssp
import sys
import pandas as pd
import numpy as np

def parseData():
    """
    Parses command line arguments and input files, checks validity of input parameters.
    Returns the merged input data, number of clusters (k), maximum iterations (iter), and epsilon (epsilon).
    """
    # parse input
    inputLen = len(sys.argv) - 1
    k = int(sys.argv[1])
    iter = 300 if inputLen < 5 else int(sys.argv[2])
    epsilon = float(sys.argv[2]) if inputLen < 5 else float(sys.argv[3])
    input1 = sys.argv[3] if inputLen < 5 else sys.argv[4]
    input2 = sys.argv[4] if inputLen < 5 else sys.argv[5]

    # Merge 2 input files and sort by first column
    file1 = pd.read_csv(input1, header=None)
    file2 = pd.read_csv(input2, header=None)
    inputFiles = pd.merge(file1, file2, how="inner", on=[0])
    inputFiles.sort_values(by=0, inplace=True)
    lineCount = len(inputFiles.index)

    # check valid input
    error_cnt = 0
    if k >= (lineCount - 1) or k <= 1:  # check if: 1. k is numeric 2. 1<k<n
        print("Invalid number of clusters!")
        error_cnt += 1
    if iter >= 1000 or iter <= 1:  # check if: 1.itr is numeric 2. 1<itr<1000
        print("Invalid maximum iteration!")
        error_cnt += 1
    if error_cnt > 0:
        sys.exit(0)

    return inputFiles, k, iter, epsilon

def euclideanDist(x, y):
    """
    calculates the Euclidean distance between x and y vectors
    :param x: list of floats representing x vector
    :param y: list of floats representing y vector
    :return: distance as a float
    """
    dist = 0
    for i in range(1, len(x)):
        dist += (x[i] - y[i]) ** 2
    return dist ** 0.5


def minDist(dpKey, keyList,inputData ):
    """
    calculate the distance to the closest centroid represented by key
    from keyList and return the distance
    note - keys are unique!
    :param currKey: key of current vector in data
    :param keyList: list of keys of all centroids
    :return: distance between vector(with currKey) and closest cluster centroid
    """
    # default centroid initialization
    x = inputData.loc[inputData[0] == dpKey].values.flatten()
    y = inputData.loc[inputData[0] == keyList[0]].values.flatten()
    minDist = euclideanDist(x, y)

    for i in range(1, len(keyList)):
        y = inputData.loc[inputData[0] == keyList[i]].values.flatten()
        currDist = euclideanDist(x, y)
        if currDist < minDist:
            minDist = currDist

    return minDist

def chooseInitialCent(inputData, k):
    """
    Chooses initial centroids using the K-means++ algorithm.
    :param inputData: DataFrame containing the input data
    :param k: number of clusters
    :return: List of keys of the chosen centroids
    """
    np.random.seed(1234)
    numOfVectors = len(inputData.index)
    #choose first cdentroid uniformly at random among the data points
    randIndex = np.random.randint(numOfVectors)
    keyList = [inputData.iloc[randIndex, 0]]

    #for each datapoint not chosen yet, calculate the distance 
    #between the datapoint and the nearest center that has already been chosen
    for i in range(k - 1):
        distances = {}
        for j in range(numOfVectors):
            currKey = inputData.iloc[j, 0]
            if currKey in keyList:
                distances[currKey] = 0
            distances[currKey] = minDist(currKey, keyList, inputData)

        # calculate weighted probability distribution
        sigmaD = sum(distances.values())
        probabilityList = [distances[inputData.iloc[j, 0]] / sigmaD for j in range(numOfVectors)]

        # get new random key based on new probabilities and append to keyList (chosen clusters by key)
        dataPointKeys = inputData[0].values.flatten()
        newKey = np.random.choice(dataPointKeys, 1, p=probabilityList)
        keyList.append(newKey[0])  # newKey returned as an NDarray - we need the first and only element

    return keyList

def retrieveInitialCents(key_list):
    """
    Retrieves initial centroids from the input data based on the keys in keyList.
    :param keyList: List of keys of the chosen centroids
    :return: List of centroid vectors
    """
    # Exclude the first column
    dataAsDF = inputData.iloc[:, 1:]

    # Initialize an empty list to store the filtered vectors
    filteredVectors = []

    # Iterate over key_list and retrieve vectors in the desired order
    for key in key_list:
        vector = dataAsDF[inputData.iloc[:, 0] == key].values.tolist()
        if vector:
            filteredVectors.append(vector[0])

    return filteredVectors


if __name__ == "__main__":
    inputData, k, iter, epsilon = parseData()
    keyList = chooseInitialCent(inputData, k)
    rows = len(inputData.index)
    cols = len(inputData.columns) - 1

    dataAsDF = inputData.iloc[:, 1:]
    dataAsList = dataAsDF.values.tolist()
    firstInitCentroidsList = retrieveInitialCents(keyList)

    
    centroids = mykmeanssp.fit(rows, cols, k, iter, epsilon, dataAsList, firstInitCentroidsList)

    #print first k chosen centroids by their keys
    print(','.join(map(str, [int(x) for x in keyList])))
    
    #print the final centroids
    for centroid in centroids:
        formatted_centroid = [f"{x:.4f}" for x in centroid]
        print(','.join(formatted_centroid))





    
    
