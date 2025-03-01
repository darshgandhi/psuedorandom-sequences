#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int** getSubsequences(int* E, int N, int k) {
    int** X = (int**)malloc((N - k + 1) * sizeof(int*));
    
    for (int i = 0; i <= N - k; i++) {
        X[i] = (int*)malloc(k * sizeof(int));
        for (int j = 0; j < k; j++) {
            X[i][j] = E[i + j];
        }
    }
    
    return X;
}

void checkPseudorandom(int* E, int N) {
    int condition = log(N) / log(2);
    
    for (int k = 1; k <= condition; k++) {
        int** X = getSubsequences(E, N, k);
        
        // Printing out X FOR TESTING ONLY
        for (int i = 0; i < (N - k + 1); i++) {
            printf("Subsequence %d: ", i + 1);
            for (int j = 0; j < k; j++) {
                printf("%d ", X[i][j]);
            }
            printf("\n");
            free(X[i]);
        }
        free(X);
        
        // this is only until making the subsets
    }
}

int main() {
    int E[] = {-1, 1, -1, 1, 1, -1, -1, 1};
    int N = 8;
    checkPseudorandom(E, N);
}
