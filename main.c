#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
// #include "myMPI.h"   // not sure if this is needed for the teach cluster

// prints E of length N to terminal
void printArray(int* E, int N) {
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%d", E[i]);
        if(i < (N - 1)) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Prints E of length N to file pointer fp
void printToFile(int* E, int N, FILE* fp) {
    fprintf(fp, "[");  
    for (int i = 0; i < N; i++) {
        fprintf(fp, "%d", E[i]);
        if(i < (N - 1)) fprintf(fp, ", ");
    }
    fprintf(fp,"]\n");
}

// creates sequence seq of length N
int* createSeq(int N) {
    int* seq = (int*) malloc(N * sizeof(int));

    // this loop should populate seq with {-1, +1} values
    for (int i = 0; i < N; i++) {     
        int randVal = rand() % 2;   // not truly random due to how rand() function is implemented
        seq[i] = (randVal == 0) ? 1 : -1;
    }
    return seq;
}   

// unclear what this function does, creates a new sequence so the function name is inaccurate
// I believe on further review that this function 
int* createSeqIdx(int N, int i) {
    int* seq = (int*) malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
        seq[j] = (i / ((int) pow(2, N - j - 1)) % 2 == 1) ? 1 : -1;
    }
    return seq;
} 

// can be parallelized
// computes number of times X occurs as a subsequence in E
int compute_T(int E[], int N, int k, int X[]) {
    int c = 0;
    for (int n = 0; n <= N - k; n++) {
        bool target = true;
        
        for (int j = 0; j < k; j++) {
            if (E[n+j] != X[j]) {
                target = false; 
                break;
            }
        }
        if (target) c++;
    }
    return c;
}

// creates a sequence X for the T function
int* createSubseq(int i, int len) {
    int* X = (int*) malloc(len * sizeof(int));
    for (int j=0; j<len; j++) {
        X[j] = (i % 2) ? -1 : 1;
        i = i / 2;
    }
    return X;
}

// All of this can be parallelized, thus the functions it calls can be parallelized
/* 
 * checks if subsequence is Pseudorandom or not, by basically executing the second
 * mathematical definition in our assignment sheet 
 */
bool checkPseudorandom(int* E, int N) {
    int condition = (int) (log2(N));    // condition variable, named k in definition
    
    for (int k = 1; k <= condition; k++) {  // should never loop more than 4 times in this assignment
        int kPow = (int) pow(2, k); // 2 to the power of k
        int M = N - k + 1;  // 

        // Iterating through all subsequences of length k
        for (int i = 0; i < kPow; i++) {
            
            // Create subsequence X & run T
            int* X = createSubseq(i, k);
            int T = compute_T(E, M, k, X);
            free(X);

            printf("k: %d, T: %d, rVal: %f, T - rVal: %f sqrt: %f\n", k, T, fabs(T - (float) M/pow(2,k)), (1.0/sqrt(N)), 0.0);

            if (fabs(T - (float) M / pow(2,k)) > (1.0/sqrt(N))) {
                return false;   // always returned for some reason, fix asap
            }
        }
    }
    return true;
}

// main() function, standard for every c program. Entry begins here
int main() {
    // Additionally we need another function so we can generate the array there are a few methods he wants us to follow by D. Knuth, TAOCP volume 2
    srand(time(NULL)); // Need this for random generation otherwise it does the same array
    
    // variables for main function (in C vars should be initialized at the start of the function)
    char filename[20];
    int N = 0;
    int* E;
    bool random;
    int seqLength = 0;
    FILE* file;

    // Implementation Question 1 Without MPI:
    E = createSeq(N);
    printArray(E, N);

    random = checkPseudorandom(E, N);
    printf("Result: %s", random ? "True" : "False");

    // Implementation Question 2 With MPI for ALL 2^N sequences:
    N = 15;
    seqLength = pow(2, N);

    // NEED TO ADD MPI STUFF SOMEWHERE
    snprintf(filename, sizeof(filename), "scratch/pr.%d.txt", N);
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < seqLength; i++) {
        int* E = createSeqIdx(N, i);
        printToFile(E, N, file);
        bool random = checkPseudorandom(E, N);
        fprintf(file, "Is Sequence Random: %s\n", random ? "True" : "False");
        free(E);
    }

    fclose(file);
    
    // Implementation Question 4:
    //DONT RUN BELOW BEFORE MPI IMPLEMENTATION:
    /*int start = 20;
    int end = 30;
    for (start; start <= end; start++) {
        // Find all Pseudorandom Sequences of lengths 20 <= N <= 30
        int N = start;
        int seqLength = pow(2, N);

        // File Stuff
        char filename[10];
        snprintf(filename, sizeof(filename), "/scratch/pr.%d.txt", start);
        FILE* file = fopen(filename, "w");

        if (!file) {
            printf("Issue Opening File $s\n", filename);
            break;
        }

        for (int i = 0; i < seqLength; i++) {
            int* E = createSeqIdx(N, i);
            printToFile(E, N, file);
            bool random = checkPseudorandom(E, N);
            fprintf(file, "Is Sequence Random: %s\n", random ? "True" : "False");
            free(E);
        }
        fclose(file);
    }*/
}
