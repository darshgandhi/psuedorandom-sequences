#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

void printArray(int* E, int N) {
    printf("[");
    for (int i =0; i< N; i++) {
        printf("%d",E[i]);
        if(i < N -1) {
            printf(", ");
        }
    }
    printf("]\n");
}

int* generateSequence(int N) {
    int* seq = (int*)malloc(N * sizeof(int));
    for (int i=0;i<N;i++) {
        int randVal = rand() % 2;
        seq[i] = randVal ? 1 : -1;
    }
    return seq;
}   

int compute_T(int E[], int N, int k, int X[]) {
    int c = 0;
    for (int n = 0; n <= N - k; n++) {
        bool target = true;
        
        for (int j = 0; j < k; j++) {
            if (E[n+j] != X[j]) {target = false; break;}
        }
        if (target) c++;
    }
    return c;
}

int* createSubseq(int i, int len) {
    int* X = (int*)malloc(len * sizeof(int));
    for (int j=0; j<len; j++) {
        X[j] = (i % 2) ? -1 : 1;
        i = i / 2;
    }
    return X;
}

// somewhere in this function it for some reason alwasys returns false even if the string is random 
bool checkPseudorandom(int* E, int N) {
    int condition = (int)(log2(N));
    
    for (int k = 1; k <= condition; k++) {
        int kPow = (int)pow(2, k);
        int M = N - k + 1;

        // Iterating through all subsequences of k
        for (int i = 0; i < kPow; i++) {
            
            // Create subsequence & get T
            int* X = createSubseq(i, k);
            int T = compute_T(E, M, k, X);

            printf("k: %d, T: %d, rVal: %f, T - rVal: %f sqrt: %f\n", k, T, fabs(T - (float)M/pow(2,k)), (1.0/sqrt(N)));
            
            // Free X 
            free(X);

            if (fabs(T - (float)M/pow(2,k)) > (1.0/sqrt(N))) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    // Additionally we need another function so we can generate the array there are a few methods he wants us to follow by D. Knuth, TAOCP volume 2
    srand(time(NULL)); // Need this for random generation otherwise it does the same array
    
    // Implementation Question 1 Without MPI:
    int N = 10;
    int* E = generateSequence(N);
    printArray(E, N);

    bool random = checkPseudorandom(E, N);
    printf("Result: %s", random ? "True" : "False");

    // Implementation Question 2 With MPI for 2^N:
    /*
        int N = 10;
        int seqLength = pow(2, N);
        int* E = generateSequence(seqLength);
        printArray(E, seqLength);

        bool random = checkPseudorandom(E, seqLength);
        printf("Result: %s", random ? "True" : "False");
    */
}
