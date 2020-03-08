#ifndef _BAKA_MATRIX_H_
#define _BAKA_MATRIX_H_

#include "baka_vectors.h"

typedef float Matrix4[4][4];

void matrix_print(Matrix4 mat);
void matrix_identity(Matrix4 mat);
void matrix_zero(Matrix4 mat);
void matrix_view(Matrix4 mat, Vec3 position, Vec3 target, Vec3 up);
void matrix_perspective(Matrix4 mat, float fov, float aspect, float near, float far);

#endif