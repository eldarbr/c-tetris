#ifndef _MATRIXLIB_
#define _MATRIXLIB_

/// @file matrix.h
/// @brief Declaration of methods to operate with matrices

#include <stdbool.h>

int **calloc_matrix(const int r, const int c);
void free_matrix(int **matrix);

void fill_matrix(int **matrix, const int r, const int c, const int value);
bool get_is_a_filled_row(const int **matrix, const int row, const int row_size);
void shift_down_rows(int **matrix, const int last_shift_row,
                     const int shift_steps, const int row_size);

void print_matrix(int **const matrix, const int r, const int c);

#endif
