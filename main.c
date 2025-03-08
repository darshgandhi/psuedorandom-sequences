#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
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
    fprintf(fp,"]: ");
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

/* 
 * checks if subsequence is Pseudorandom or not, by basically executing the second
 * mathematical definition in our assignment sheet 
 */
bool checkPseudorandomMPI(int* E, int N, rank, size) {
    int kMax = (int)floor(log(N) / log(2));    // condition variable, named k in definition
    bool localRandom = true;
    bool globalRandom = true;

    // Not sure if we need to call MPI here or not otherwise just in the
    main is fine
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

int main(int argc, char** argv) {   
    // COMMENT OUT SECTIONS DEPENDING ON WHAT UR TESTING
    //=======================================================================
    // QUESTION 1
    // Without MPI: For single N
    /*
    int* E; // sequence of boolean values to evaluate
    bool random;
    int N = 11; // number of elements in E
    int hardcoded[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // update accordingly
    E = hardcoded;

    printf("Testing Hardcoded Sequence: ");
    printArray(E, N);

    random = checkPseudorandom(E, N);
    printf("Result For Hardcoded Sequence (Is Random?): %s", random ? "True" : "False");
    printf("\n");
    */
    //=======================================================================


    //=======================================================================
    //   QUESTION 2
    //   With MPI for ALL 2^N sequences
    //=======================================================================

    // Define Variables

    // Initialize MPI Stuff
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time, end_time;

    if (argc < 2) {
        if (rank == 0) {
            printf("Incorrect Entry!");
        }
        MPI_Finalize();
        return true;
    }

    // Get N and also Determine total number of sequences
    int N = atoi(argv[1]); // send the N in from the command line
    long long totalSeq = pow(2, N); // using long long for if 30 is input then int isnt long enough

    // Creates a output file for the results
    char localFilename[BUFF_LENGTH];
    snprintf(localFilename, sizeof(localFilename), "local_pr_%d.%d.txt", rank, N);
    FILE* localFile = fopen(localFilename, "w");

    // Handle Errors for file stuff
    if (file == NULL) {
        perror("Error opening file");
        exit(true);
    }

    // Determine workload for each process
    int localSeqCount = 0;
    long long workload = totalSeq / size;
    long long startLocal = rank * workload;
    long long endLocal = 0;
    if (rank == size - 1) {
        endLocal = totalSeq;
    } else {
        endLocal = (rank+1)*workload
    }

    startTime = MPI_Wtime();

    // Each sequence can be tested independently, so divide the 2^N sequences among processes.
    for (long long i = 0; i < totalSeq; i++) {
        int* E = createSeqIdx(N, i);
        bool random = checkPseudorandomMPI(E, N, rank, size);

        if (random) localSeqCount++;
        printToFile(E, N, localFile);
        fprintf(localFile, "Is Random? %s\n", random ? "True" : "False");

        free(E);
    }

    endTime = MPI_Wtime();

    fclose(localFile);

    // NEED TO GATHER STUFF AND THEN ALSO MERGE ALL TEMP FILES INTO A FINAL FILE BELOW
    // Keep MPI  FINALIZe below everything
    MPI_Finalize();
}