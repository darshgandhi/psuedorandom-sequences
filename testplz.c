#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

// Count how many times the pattern X (of length k) appears in sequence E of length N.
int compute_T(int E[], int N, int k, int X[]) {
    int count = 0;
    int window_count = N - k + 1;  // total number of sliding windows
    for (int n = 0; n < window_count; n++) {
        bool match = true;
        for (int j = 0; j < k; j++) {
            if (E[n + j] != X[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            count++;
        }
    }
    return count;
}

int* createSubseq(int i, int len) {
    int* X = (int*)malloc(len * sizeof(int));
    if (X == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < len; j++) {
        // If the current bit is 1, set element to -1; otherwise, set to 1.
        X[j] = (i % 2) ? -1 : 1;
        i = i / 2;
    }
    return X;
}

// Check if sequence E (of length N) is pseudorandom according to the definition.
bool checkPseudorandom(int* E, int N) {
    int max_k = (int)log2(N);
    
    for (int k = 1; k <= max_k; k++) {
        int total_patterns = (int)pow(2, k);
        // Iterate through all possible patterns X of length k.
        for (int i = 0; i < total_patterns; i++) {
            int* X = createSubseq(i, k);
            int window_count = N - k + 1;
            int T = compute_T(E, N, k, X);
            
            double expected = (double)window_count / pow(2, k);
            printf("k: %d, T: %d, Expected: %f, |T - Expected|: %f, Threshold: %f\n",
                   k, T, expected, fabs(T - expected), (1.0 / sqrt(N)));
            
            free(X); // Free the allocated memory for the pattern
            
            if (fabs(T - expected) > (1.0 / sqrt(N))) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    int E[] = {1, -1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1};
    int N = sizeof(E) / sizeof(E[0]); 
    bool isRandom = checkPseudorandom(E, N);
    printf("Result: %s\n", isRandom ? "True" : "False");
    return 0;
}
