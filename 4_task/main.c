#include <stdio.h>
#include <malloc.h>

extern void mergesort(int from, int to, const int *in, int *out);

int main() {
    int N;
    int *A, *B;
    scanf("%d", &N);
    A = (int*)malloc(N*sizeof(int));
    B = (int*) malloc(N*sizeof(int));
    for (int i = 0; i < N; ++i) {
        scanf("%d", &A[i]);
    }
    mergesort(0, N, A, B);
    for (int i = 0; i < N;++i) {
        printf("%d ", B[i]);
    }
    free(A);
    free(B);

    return 0;
}
