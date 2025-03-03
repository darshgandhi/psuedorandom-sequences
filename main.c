#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

int calulateT(int* E, int N, int k, int* X) {
    int count = 0;
    
    // Iterating through E
    for (int n = 0; n <= N - k; n++) {
        bool target = true;
        
        for (int j = 0; j < k; j++) {
            if (E[n+j] != X[j]) {
                target = false;
                break;
            }
        }
        
        if (target) {
            count++;
        }
    }
    return count;
}

bool checkPseudorandom(int* E, int N) {
    int condition = (int)(log(N) / log(2));
    
    for (int k = 1; k <= condition; k++) {
        int kPow = (int)pow(2, k);
        
        // Iterating through all subsequences of k
        for (int i = 0; i < kPow; i++) {
            int X[k];
            
            int y = i;
            // Generating Subsequence
            for (int j=0; j<k; j++) {
                if(y%2) {
                    X[j] = -1;
                } else {
                    X[j] = 1;
                }
                y = y / 2;
            }
            
            // Condition Left Value { T(E, M, X) }
            int M = N + 1 - k;
            int T = calulateT(E, M, k, X);
            
            // Condition Right Value { N + 1 - K / 2^k }
            double rVal = (double)M/(double)pow(2,k);
            
            if (!(fabs(T - rVal) <= (1/sqrt(N)))) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    int E[] = {-1, 1, -1, 1, 1, -1, -1, 1};
    int N = 8;
    bool random = checkPseudorandom(E, N);
    printf("%s", random ? "True" : "False");
}
