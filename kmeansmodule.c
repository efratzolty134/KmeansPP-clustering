#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "kmeans.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* convert python input data to an array in c and allocate memory */
static double **convertToArray(PyObject *pyData, int d)
{
    int i, j, k;
    PyObject *pyVector;
    PyObject *pyCord;
    int len = (int)PyList_Size(pyData); /* Get the length of the list*/
    double **cArray = calloc(len, sizeof(double *));

    for (k = 0; k < len; k++)
    {
        cArray[k] = calloc(d, sizeof(double));
        assert(cArray[k] != NULL);
    }

    for (i = 0; i < len; i++)
    {
        pyVector = PyList_GetItem(pyData, i);
        for (j = 0; j < d; j++)
        {
            pyCord = PyList_GetItem(pyVector, j);
            cArray[i][j] = PyFloat_AsDouble(pyCord);
        }
    }
    return cArray;
}

void freeArray(double **array, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        free(array[i]);
    }
    free(array);
}

/*
 * Wrapper function for fit
 */
static PyObject* fit(PyObject *self, PyObject *args)
{
    int rows, cols, k, iter, i, j;
    double epsilon;
    double **inputCdata, **initCentroidsC, **finalCentroids;
    PyObject *inputPyData, *initCentroidsPy, *result, *pyVector, *pyCord;

    if (!PyArg_ParseTuple(args, "iiiidOO", &rows, &cols, &k, &iter, &epsilon, &inputPyData, &initCentroidsPy))
    {
        printf("An Error Has Occurred");
        return NULL;
    }
    if (!PyList_Check(inputPyData))
    {
        printf("An Error Has Occurred");
        return NULL;
    }
    if (!PyList_Check(initCentroidsPy))
    {
        printf("An Error Has Occurred");
        return NULL;
    }

    inputCdata = convertToArray(inputPyData, cols);
    initCentroidsC = convertToArray(initCentroidsPy, cols);

    finalCentroids = (double **)kmeans(rows, cols, k, iter, epsilon, inputCdata, initCentroidsC);

    result = PyList_New(k);
    for (i = 0; i < k; i++)
    {
        pyVector = PyList_New(cols);
        for (j = 0; j < cols; j++)
        {
            pyCord = Py_BuildValue("f", finalCentroids[i][j]);
            PyList_SetItem(pyVector, j, pyCord);
        }
        PyList_SetItem(result, i, pyVector);
    }

    freeArray(finalCentroids, k);
    freeArray(initCentroidsC, k);

    return result;
}

/*module's function table*/
static PyMethodDef fit_FunctionsTable[] = {
    {"fit",
     (PyCFunction)fit,
     METH_VARARGS,
     PyDoc_STR("finds the centroids for the input data using the kmeans algorithm from hw1")},
    {NULL, NULL, 0, NULL}};

/* modules definition*/
static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    "C module for implementing kmeans C function in Python",
    -1,
    fit_FunctionsTable};


PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *modul;
    modul = PyModule_Create(&kmeansmodule);
    if (!modul)
    {
        return NULL;
    }
    return modul;
}
