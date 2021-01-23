#include <stdio.h>
#include <malloc.h>

extern void summ(int N, const int *A, const int *B, int *R);

int main() {
    int N;
    int *A, *B, *R;
    scanf("%d", &N);
    A = (int*)malloc(N*sizeof(int));
    B = (int*) malloc(N*sizeof(int));
    R = (int*) malloc(N*sizeof(int));
    for (int i = 0; i < N; ++i) {
        scanf("%d", &A[i]);
    }
    for (int i = 0; i < N; ++i) {
        scanf("%d", &B[i]);
    }
    summ(N, A, B, R);
    for (int i = 0; i < N;++i) {
        printf("%d", R[i]);
    }
    free(A);
    free(B);
    free(R);
}
