# KmeansPP-clustering
This project implements the K-means++ algorithm in Python for initial centroid selection in K-means clustering. The Python code interfaces with a C extension that runs the core K-means algorithm, using Python’s C API for improved performance and integration.

## Overview
- K-means++ is used to choose better initial centroids before running the classic K-means algorithm.
- The initial centroids are selected in Python using NumPy and pandas.
- The clustering is executed in C, wrapped as a Python extension module for performance.
- Designed as an academic project for the "Software Project (0368-2161)" course at Tel Aviv University.

## File Structure

| File              | Description |
|-------------------|-------------|
| `kmeans_pp.py`    | Main Python script — handles command-line arguments , I/O, K-means++, and integration with C extension |
| `kmeans.c`        | Core K-means algorithm |
| `kmeans.h`        | Header for K-means function |
| `kmeansmodule.c`  | C extension for Python using C API |
| `setup.py`        | Build script for creating the Python extension |
