/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int bsize = 8; /* only for (s, E, b) b == 5 case, block has 2^5=32 bytes which can store 8 ints(4 bytes) */
    int i, j, jj;
    int em = bsize * (M / bsize); /* Amount that fits evenly into blocks */
    int diag;
    for (jj = 0; jj < em; jj += bsize) {
        for (i = 0; i < N; i++) {
            for (j = jj; j < jj + bsize; j++) {
                if (i == j) {
                    // prevent evication between A and B when address map to the same cache set
                    diag = A[i][i];
                } else {
                    B[j][i] = A[i][j];
                }
            }
            // meet the real diangonal item 
            if (i >= jj && i < jj + bsize) {
                B[i][i] = diag;
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * transpose for 64 * 64 Matrix
 */
char ultra_transpose_submit_desc64[] = "Ultra Transpose submission";
void ultra_transpose_submit64(int M, int N, int A[N][M], int B[M][N])
{
    int bsize = 8; /* only for (s, E, b) b == 5 case, block has 2^5=32 bytes which can store 8 ints(4 bytes) */
    int i, j, jj;
    int em = bsize * (M / bsize); /* Amount that fits evenly into blocks */
    int diag;
    for (jj = 0; jj < em; jj += bsize) {
        for (i = 0; i < N; i++) {
            for (j = jj; j < jj + bsize; j++) {
                if (i == j) {
                    // prevent evication between A and B when address map to the same cache set
                    diag = A[i][i];
                } else {
                    B[j][i] = A[i][j];
                }
            }
            // meet the real diangonal item 
            if (i >= jj && i < jj + bsize) {
                B[i][i] = diag;
            }
        }
    }
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register your solution function */
    registerTransFunction(ultra_transpose_submit64, ultra_transpose_submit_desc64); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

