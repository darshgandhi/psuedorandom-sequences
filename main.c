#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

int compute_T(int E[], int N, int k, int X[]) {
    int c = 0;
    bool target;
    
    // Iterating through E
    for (int n = 0; n <= N - k; n++) {
        target = true;
        
        for (int j = 0; j < k; j++) {
            if (E[n+j] != X[j]) {
                target = false;
                break;
            }
        }
        if (target) {
            c++;
        }
    }
    return c;
}

int* createSubseq(int i, int len) {
    int* X = (int*)malloc(len * sizeof(int));
    for (int j=0; j<len; j++) {
        if(i%2) {
            X[j] = -1;
        } else {
            X[j] = 1;
        }
        i = i / 2;
    }
    return X;
}


// somewhere in this function it for some reason alwasys returns false even if the string is random 
bool checkPseudorandom(int* E, int N) {
    int condition = (int)(log2(N));
    
    for (int k = 1; k <= condition; k++) {
        int kPow = (int)pow(2, k);
        
        // Iterating through all subsequences of k
        for (int i = 0; i < kPow; i++) {
            
            // Create subsequence & get T
            int* X = createSubseq(i, k);
            int M = N - k + 1;
            int T = compute_T(E, M, k, X);
            
            // N + 1 - K / 2^k 
            double rVal = (double)M/pow(2,k);
            printf("k: %d, T: %d, rVal: %f, fabs(T - rVal): %f sqrt: %f\n", k, T, rVal, fabs(T - rVal), (1.0/sqrt(N)));
            if (fabs(T - rVal) > (1.0/sqrt(N))) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    // Additionally we need another function so we can generate the array there are a few methods he wants us to follow by D. Knuth, TAOCP volume 2
    int E[] = {1, -1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1};
    int N = 16;
    bool random = checkPseudorandom(E, N);
    printf("Result: %s", random ? "True" : "False");
}
