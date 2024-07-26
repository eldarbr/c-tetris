#include "figures.h"

/// @file figures.c
/// @brief Implementation of functions to create figures

#include <string.h>

#include "defines.h"
#include "matrix.h"

typedef void (*figure_filler_t)(int **);

#define def_fillers                                                         \
  figure_filler_t fillers[] = {fill_figure_0, fill_figure_1, fill_figure_2, \
                               fill_figure_3, fill_figure_4, fill_figure_5, \
                               fill_figure_6};

#define colour 1
void fill_figure_0(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  for (int i = 0; i != 4; ++i) {
    buff[2][i] = colour;
  }
}
#undef colour

#define colour 2
void fill_figure_1(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  for (int i = 0; i != 3; ++i) {
    buff[2][i] = colour;
  }
  buff[1][0] = colour;
}
#undef colour

#define colour 3
void fill_figure_2(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  for (int i = 0; i != 3; ++i) {
    buff[2][i] = colour;
  }
  buff[1][2] = colour;
}
#undef colour

#define colour 4
void fill_figure_3(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  buff[1][1] = colour;
  buff[1][2] = colour;
  buff[2][1] = colour;
  buff[2][2] = colour;
}
#undef colour

#define colour 5
void fill_figure_4(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  buff[1][1] = colour;
  buff[1][2] = colour;
  buff[2][0] = colour;
  buff[2][1] = colour;
}
#undef colour

#define colour 6
void fill_figure_5(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  for (int i = 0; i != 3; ++i) {
    buff[2][i] = colour;
  }
  buff[1][1] = colour;
}
#undef colour

#define colour 7
void fill_figure_6(int **buff) {
  fill_matrix(buff, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  buff[1][0] = colour;
  buff[1][1] = colour;
  buff[2][1] = colour;
  buff[2][2] = colour;
}
#undef colour

/// @brief Fills the buffer with a figure pattern. The pattern is chosen by id
/// @param buff matrix large enough to contain the pattern
/// @param id pattern id
void fill_figure_by_id(int **const buff, const int id) {
  def_fillers;
  if (id >= 0 && id < ALLOWED_FIGURES_COUNT) {
    fillers[id](buff);
  }
}
