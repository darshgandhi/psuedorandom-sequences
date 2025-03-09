#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

// We don't need this
// #include "MyMPI.h" 

#define BUFF_LENGTH 64


// prints E of length N to terminal
// Used for Testing
void printArray(int* E, int N) {
    // E is the bool sequence to print
    // N is the length of E
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%d", E[i]);
        if (i < (N - 1)) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Prints E of length N to file pointer fp
// Use only for valid sequences to avoid bad time complexity
void printToFile(int* E, int N, FILE* fp) {
    // E is the sequence of bools to print
    // N is the length of E
    // fp is the file pointer that pointer to where to print
    fprintf(fp, "[");
    for (int i = 0; i < N; i++) {
        fprintf(fp, "%d", E[i]);
        if (i < (N - 1)) fprintf(fp, ", ");
    }
    fprintf(fp, "]: ");
}

// creates a sequence X for the T function
int* createSubseq(int i, int k) {
    // i is integer value to convert
    // k is length of output sequence
    int* X = (int*)malloc(k * sizeof(int));
    for (int j = 0; j < k; j++) {
        // below line is used to convert i into a bool array
        X[j] = (i & (1 << (k - j - 1))) ? 1 : -1;
    }
    return X;
}

// This function is called from a for loop to
// all possible sequences of length N as i iterates from 0 to 2^N-1
int* createSeqIdx(int N, int i) {
    // i is integer to convert
    // N is length of sequence
    int* seq = (int*)malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
        // below line is used to convert i into a bool array
        seq[j] = (i & (1 << (N - j - 1))) ? 1 : -1;
    }
    return seq;
}

// computes number of times X occurs as a subsequence in E
int compute_T(int E[], int M, int k, int X[]) {
    // E is entered sequence that is checked for randomness
    // M is the length of comparison sequence (N + 1)
    // k is the length of X
    // X is the sequence for comparison to subsequences of E 
    int output = 0;
    for (int i = 0; i < M; i++) {
        bool identical = true;
        for (int j = 0; j < k; j++) {
            if (E[i + j] != X[j]) {
                identical = false;
                break;
            }
        }
        if (identical) output++;
    }
    return output;
}

/*
 * checks if subsequence is Pseudorandom or not using Knuths textbook version, by basically executing the second
 * mathematical definition in our assignment sheet
 */
bool checkPseudorandomTextbook(int* E, int N) {
    // E is the bool sequence to check 

    //int N = sizeof(*E) / sizeof(E[0]);   // number of elements in E
    //printf("%d\n", N);
    int kMax = (int)floor(log2(N));    // condition variable, named k in definition

    //printf("Testing sequence length %d w/ log2(N) = %d\n", N, kMax);

    // Iterating through all subsequences of length k
    for (int k = 1; k <= kMax; k++) {  // should never loop more than 4 times in this assignment
        int kPow = pow(2, k); // 2 to the power of k

        // printf("Testing K = %d\n", k);
        for (int i = 0; i < kPow; i++) {

            // Create subsequence X & run T
            int* X = createSubseq(i, k);
            int valueT = compute_T(E, (N + 1), k, X);
            free(X);

            double sqrtN = 1.0 / sqrt(N);
            double subVal = 1.0 / kPow;

            double dValueT = (double)valueT;

            double compValue = dValueT / N;
            compValue -= subVal;
            compValue = fabs(compValue);

            if (compValue > sqrtN) return false;
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

    // Iterating through all subsequences of length k
    for (int k = 1; k <= kMax; k++) {  // should never loop more than 4 times in this assignment
        int kPow = 1 << k; // 2 to the power of k

        // printf("Testing K = %d\n", k);
        for (int i = 0; i < kPow; i++) {

            // Create subsequence X & run T
            int* X = createSubseq(i, k);
            int M = N + 1 - k;
            int T = compute_T(E, M, k, X);

            double sqrtN = 1.0 / sqrt(N);
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
    if (localFile == NULL) {
        perror("Error opening file");
        exit(true);
    }

    // determining workload for each process
    int localSeqCount = 0;
    long long workload = totalSeq / size;
    long long startLocal = rank * workload;
    long long endLocal = 0;
    if (rank == size - 1) {
        endLocal = totalSeq;
    }
    else {
        endLocal = (rank + 1) * workload;
    }

    start_time = MPI_Wtime();

    // Each process is only iterating over its assigned range of sequences
    for (long long i = startLocal; i < endLocal; i++) {
        int* E = createSeqIdx(N, i);
        bool random = checkPseudorandomTextbook(E, N);

        if (random) localSeqCount++;
        printToFile(E, N, localFile);
        fprintf(localFile, "Is Random? %s\n", random ? "True" : "False");

        free(E);
    }

    end_time = MPI_Wtime();

    fclose(localFile);

    // gathering results and merging 
    int globalSeqCount = 0;
    MPI_Reduce(&localSeqCount, &globalSeqCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        char finalFilename[BUFF_LENGTH];
        snprintf(finalFilename, sizeof(finalFilename), "scratch/pr_%d.txt", N);

        FILE *finalFile = fopen(finalFilename, "w");
        if (finalFile == NULL) {
            perror("Error opening final output file");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        char localFilename[BUFF_LENGTH];
        char line[1024];
        for (int r = 0; r < size; r++) {
            snprintf(localFilename, sizeof(localFilename), "local_pr_%d.%d.txt", r, N);
            FILE *localFile = fopen(localFilename, "r");
            while (fgets(line, sizeof(line), localFile) != NULL) {
                fputs(line, finalFile);
            }
            fclose(localFile);
            remove(localFilename);

        }
        
        fprintf(finalFile, "\nTotal pseudorandom sequences: %d\n", globalSeqCount);
        fclose(finalFile);
        printf("Merged final output written to %s\n", finalFilename);
    }

    // Keep MPI  FINALIZe below everything
    MPI_Finalize();
}