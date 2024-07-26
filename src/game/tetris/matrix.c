#include "matrix.h"

/// @file matrix.c
/// @brief Implementation of methods to operate with matrices

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief allocate and fill with zeros a matrix
/// @param r rows count
/// @param c columns count
/// @return the new matrix
int **calloc_matrix(const int r, const int c) {
  if (r < 1 || c < 1) {
    return NULL;
  }
  const size_t total_buffer_size = sizeof(int) * r * c + sizeof(int *) * r;
  int **tmp = (int **)malloc(total_buffer_size);
  if (tmp) {
    memset(tmp, 0, total_buffer_size);
    int *data_sector = (int *)(tmp + r);
    for (int i = 0; i != r; ++i) {
      tmp[i] = data_sector + i * c;
    }
  }
  return tmp;
}

/// @brief free memory that was occupied by the matrix
/// @param matrix the matrix
void free_matrix(int **matrix) {
  if (matrix) {
    free(matrix);
  }
}

/// @brief set all the cells of the matrix to Value
/// @param matrix the matrix to fill
/// @param r matrix rows count
/// @param c matrix columns count
/// @param value value to fill the matrix with
void fill_matrix(int **matrix, const int r, const int c, const int value) {
  if (!matrix || r < 0 || c < 0) return;
  for (int i = 0; i != r; ++i) {
    for (int j = 0; j != c; ++j) {
      matrix[i][j] = value;
    }
  }
}

/// @brief tells if all the cells of a row are filled with any nonzero value
/// @param matrix the matrix to check
/// @param row the row to check
/// @param row_size number of columns in the matrix
/// @return true if the row is full
bool get_is_a_filled_row(const int **matrix, const int row,
                         const int row_size) {
  if (!matrix || row_size < 0 || row < 0) return false;
  bool filled = true;
  for (int i = 0; filled && i < row_size; ++i) {
    if (matrix[row][i] == 0) {
      filled = false;
    }
  }
  return filled;
}

/// @brief shift down all the first to last_shift_row rows. (last_shift_row -
/// shift_steps) rows are deleted. new rows are initialized with zeros
/// @param matrix the matrix
/// @param last_shift_row last row in the shift pool
/// @param shift_steps how far the rows are shifted
/// @param row_size number of columns in the matrix
void shift_down_rows(int **matrix, const int last_shift_row,
                     const int shift_steps, const int row_size) {
  if (!matrix || last_shift_row < 0 || shift_steps < 0 || row_size < 0) return;
  for (int i = last_shift_row; i >= 0; --i) {
    const int src_row = i - shift_steps;
    if (src_row >= 0) {
      memcpy(matrix[i], matrix[src_row], row_size * sizeof(int));
    } else {
      memset(matrix[i], 0, row_size * sizeof(int));
    }
  }
}

/// @brief output to stdout the values of the matrix
/// @param matrix the matrix
/// @param r number of rows
/// @param c number of columns
void print_matrix(int **const matrix, const int r, const int c) {
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      if (j) fputc(' ', stdout);
      printf("%d", matrix[i][j] > 0);
    }
    fputc('\n', stdout);
  }
}
