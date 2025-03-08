#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <mpi.h>
//#include "MyMPI.h"
#define BUFF_LENGTH 64

// prints E of length N to terminal
// Used for Testing
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
// Use only for valid sequences to avoid bad time complexity
void printToFile(int* E, int N, FILE* fp) {
    fprintf(fp, "[");
    for (int i = 0; i < N; i++) {
        fprintf(fp, "%d", E[i]);
        if(i < (N - 1)) fprintf(fp, ", ");
    }
    fprintf(fp,"]\n");
}

// creates a sequence X for the T function
int* createSubseq(int i, int k) {
    int* X = (int*) malloc(k * sizeof(int));
    for (int j = 0; j < k; j++) {
        X[j] = (i & (1 << (k - j - 1))) ? 1 : -1;
    }
    return X;
}

// This function is called from a for loop to
// all possible sequences of length N as i iterates from 0 to 2^N-1
int* createSeqIdx(int N, int i) {
    int* seq = (int*) malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
        seq[j] = (i & (1 << (N - j - 1))) ? 1 : -1;
    }
    return seq;
}

// computes number of times X occurs as a subsequence in E
int compute_T(int E[], int M, int k, int X[]) {
    int c = 0;
    for (int i = 0; i < M; i++) {
        bool target = true;
        for (int j = 0; j < k; j++) {
            if (E[i+j] != X[j]) { 
                target = false; 
                break; 
            }
        }
        if (target) c++;
    }
    return c;
}

/* 
 * checks if subsequence is Pseudorandom or not using Knuths textbook version, by basically executing the second
 * mathematical definition in our assignment sheet 
 */
bool checkPseudorandomTextbook(int* E, int N) {
    int kMax = (int)floor(log(N) / log(2));    // condition variable, named k in definition
    //printf("Testing sequence length %d w/ log2(N) = %d\n", N, kMax);

    // Iterating through all subsequences of length k (Each K can be worked on at the same time using MPI)
    for (int k = 1; k <= kMax; k++) {  // should never loop more than 4 times in this assignment
        int kPow = 1 << k; // 2 to the power of k

        // printf("Testing K = %d\n", k);
        for (int i = 0; i < kPow; i++) {
            
            // Create subsequence X & run T
            int* X = createSubseq(i, k);
            int M = N + 1 - k; 
            // printf("For N = %d and k = %d, M = %d\n", N, k, M);
            int T = compute_T(E, M, k, X);
            
            double sqrtN = sqrt(N); 
            double subVal = (double)(N + 1 - k) / (1 << k); // 1 << k is the same as pow(2,k) form before
            
            free(X);

            if (fabs(T - subVal) > sqrtN) return false;
            /*  // alternate version of the above line for finding errors
            printf("k: %d, T: %d, T - subVal: %f sqrt: %f\n", k, T, fabs(T - subVal), (1.0/sqrt(N)));
            if (fabs(T - subVal) > sqrtN) {
                printf("FAILED: Subsequence doesn't meet randomness\n");
                return false;
            } else {
                printf("Found Sequence\n");
            }   // */
        }
    }
    return true;
}

/* 
 * checks if subsequence is Pseudorandom or not, by basically executing the second
 * mathematical definition in our assignment sheet 
 */
bool checkPseudorandom(int* E, int N) {
    int kMax = (int)floor(log(N) / log(2));    // condition variable, named k in definition

    // Iterating through all subsequences of length k (Each K can be worked on at the same time using MPI)
    for (int k = 1; k <= kMax; k++) {  // should never loop more than 4 times in this assignment
        int kPow = 1 << k; // 2 to the power of k

        // printf("Testing K = %d\n", k);
        for (int i = 0; i < kPow; i++) {
            
            // Create subsequence X & run T
            int* X = createSubseq(i, k);
            int M = N + 1 - k; 
            int T = compute_T(E, M, k, X);
            
            double sqrtN = 1.0/sqrt(N); 
            double subVal = (double)(N + 1 - k) / (1 << k); // 1 << k is the same as pow(2,k) form before
            
            free(X);

            if (fabs(T - subVal) > sqrtN) return false;
        }
    }
    return true;
}

// main() function, standard for every c program. Entry begins here
int main() {   

    // variables for main function
    char filename[BUFF_LENGTH]; // name of file to write parallel output to
    double elapsedTime = 0.0; // Parallel execution time
    int* E; // sequence of boolean values to evaluate
    bool random;
    FILE* file;

    // Implementation Question 1 Without MPI: For single N
    int N = 11; // number of elements in E
    int hardcoded[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    E = hardcoded;

    printf("Testing Hardcoded Sequence: ");
    printArray(E, N);

    random = checkPseudorandom(E, N);
    printf("Result For Hardcoded Sequence (Is Random?): %s", random ? "True" : "False");
    printf("\n");

    // Implementation Question 2 With MPI for ALL 2^N sequences:
    long long totalSeq = pow(2, N); // using long long for if 30 is input then int isnt long enough

    printf("# of Unique Sequences: %d\n",totalSeq);
    snprintf(filename, sizeof(filename), "scratch/pr.%d.txt", N);
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(true);
    }


    // Each sequence can be tested independently, so divide the 2^N sequences among processes.
    for (long long i = 0; i < totalSeq; i++) {
        int* E = createSeqIdx(N, i);
        bool random = checkPseudorandom(E, N);
        if (random) {
            printToFile(E, N, file);
            fprintf(file, "Is Sequence Random: %s\n", random ? "True" : "False");
        }
        free(E);
    }

    fclose(file);
    
    // Implementation Question 4:
    //DONT RUN BELOW BEFORE MPI IMPLEMENTATION:
    /*int start = 24;
    int end = 27;
    for (; start <= end; start++) {
        // Find all Pseudorandom Sequences of lengths 20 <= N <= 30
        int N = start;
        int seqLength = pow(2, N);

        // File Stuff
        printf("Trying N = %d with seqLength= %d", N, seqLength);
        snprintf(filename, sizeof(filename), "/scratch/pr.%d.txt", start);
        FILE* file = fopen(filename, "w");
        fprintf(file, "Trying N = %d with seqLength= %d", N, seqLength);

        if (!file) {
            printf("Issue Opening File %s\n", file);
            break;
        }

        for (int i = 0; i < seqLength; i++) {
            int* E = createSeqIdx(N, i);
            bool random = checkPseudorandom(E, N);
            if (random) {
                printf("Here");
                printToFile(E, N, file);
                fprintf(file, "Is Sequence Random: %s\n", random ? "True" : "False");
            }
            free(E);
        }
        fclose(file);
    }*/
}