#include <stdio.h>
#include <malloc.h>

extern float dot_product(int N, const float *A, const float *B);

int main() {
    int N;
    float *A, *B;
    scanf("%d", &N);
    A = (float*)malloc(N*sizeof(float));
    B = (float*)malloc(N*sizeof(float));

    for(int i = 0; i < N; ++i) {
	scanf("%f", &A[i]);
    }
    for(int i = 0; i < N; ++i) {
        scanf("%f", &B[i]);
    }
    
    printf("%f", dot_product(N, A, B));

    free(A);
    free(B);

    return 0;
}

