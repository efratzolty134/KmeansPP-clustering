#include <math.h>
#include <stdio.h>
#include <stdlib.h>


/*struct cord - each cord is a node in a linked list*/
typedef struct cord {
    double value;
    struct cord *next;
}cord;

/*functions to create and free cords*/

cord* createCord(double val){
    cord * c = (cord*)malloc(sizeof(cord));
    c->value = val;
    c->next = NULL;
    return c;
}

void freeCords(cord* head) {
    cord* current = head;
    cord* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

/*struct vector - each vector is a node in a linked list*/
typedef struct vector{
    struct vector *next;
    struct cord *cords;
}vector;


/* Create and delete vectors */
vector* createVector(cord* head_cord){
    vector * v = (vector*)malloc(sizeof(vector));
    v->cords = head_cord;
    v->next = NULL;
    return v;
}

void freeVectors(vector* head) {
    vector* current = head;
    vector* next;

    while (current != NULL) {
        next = current->next;
        freeCords(current->cords);
        free(current);
        current = next;
    }
}

/* Function to calculate the Euclidean distance between two points */
double euclidean_distance(double* point1, double* point2, int d) {
    double dist = 0;
    int i;
    for (i = 0; i < d; i++) {
        double diff = fabs(point1[i] - point2[i]);
        dist += pow(diff, 2);
    }
    return pow(dist, 0.5);
}


int argmin(int k, int d, double* datapoint, double** centroids) {
    int minIndex = 0;
    double minDist = 1.0/0.0;
    int i;
    int j;
    double dist = 0;
    double* curr_centroid = malloc(d*sizeof(double));

    if (curr_centroid == NULL) {
    fprintf(stderr, "Memory allocation for curr_centroid data failed\n");
    exit(1);
    }

    for (i = 0; i < k; i++) {
        for (j = 0; j < d; j++) {
            curr_centroid[j] = centroids[i][j];
        }
        dist = euclidean_distance(datapoint, curr_centroid, d);
        if (dist < minDist) {
            minDist = dist;
            minIndex = i;
        }
    }
    free(curr_centroid);
    return minIndex;   
}
    
void add_to_centroids(int cols, double** centroids, int* sizes, double* datapoint, int minIndex) {
    int i;
    for (i = 0; i < cols; i++) {
        centroids[minIndex][i] += datapoint[i];
    }
    sizes[minIndex]++;
}

int check_convergence(int k, int d, double epsilon, double **prev_centroids, double **centroids)
{
    int i;
    int j;
    double* centroid1 = malloc(d*sizeof(double));
    double* centroid2 = malloc(d*sizeof(double));
    
    if ((centroid1 == NULL) ||(centroid2 == NULL)) {
    fprintf(stderr, "Memory allocation for centroid1 or centroid2 failed\n");
    exit(1);
    }

    for (i = 0; i < k; i++) {
        for (j = 0; j < d; j++) {
            centroid1[j] = prev_centroids[i][j];
            centroid2[j] = centroids[i][j];
        }
        if (euclidean_distance(centroid1, centroid2, d) >= epsilon) {
            free(centroid1);
            free(centroid2);
            return 0;
        }
    }
    free(centroid1);
    free(centroid2);
    return 1;
}

double **kmeans(int rows, int cols, int k, int iter, double epsilon, double **input_data, double **initCentroidsC)
{
    /* initialize centroids */
    int minIndex;
    int i;
    int j;
    int l;
    int *sizes = malloc(k * sizeof(int));
    double **centroids = malloc(k * sizeof(double *));
    double **prev_centroids = malloc(k * sizeof(double *));
    double **sum_centroids = malloc(k * sizeof(double *));

    /* initialize prev centroids and sizes */
    for (i = 0; i < k; i++) {
        sizes[i] = 0;
        centroids[i] = malloc(cols * sizeof(double));
        prev_centroids[i] = malloc(cols * sizeof(double));
        sum_centroids[i] = malloc(cols * sizeof(double));
        for (j = 0; j < cols; j++) {
            centroids[i][j] = initCentroidsC[i][j];
            prev_centroids[i][j] = initCentroidsC[i][j];
            sum_centroids[i][j] = initCentroidsC[i][j];
        }
    }

    /* begin iterations */
    for (i=0; i<iter; i++) {
        
        for(j = 0; j < k; j++){
            sizes[j] = 0;
            for (l = 0; l < cols; l++) {
                sum_centroids[j][l] = 0.0;
            }
        }

        for (j = 0; j < rows; j++) {
            minIndex = argmin(k, cols, input_data[j], prev_centroids);
            add_to_centroids(cols, sum_centroids, sizes, input_data[j], minIndex);
        }
        for (j=0; j<k; j++){
            for (l=0; l<cols; l++){
                centroids[j][l] = sum_centroids[j][l] / (double)sizes[j];
            }
        }

        if (check_convergence(k, cols, epsilon, prev_centroids, centroids)==1) {
            break;
        }
        for (l = 0; l < k; l++) {
            for (j = 0; j < cols; j++) {
                prev_centroids[l][j] = centroids[l][j];
            }      
        }
    }

    /* free memory */
    
    for (i = 0; i < k; i++) {
        free(prev_centroids[i]);
        free(sum_centroids[i]);
    }
    free(prev_centroids);
    free(sum_centroids);
    free(sizes);

    return centroids;
}

