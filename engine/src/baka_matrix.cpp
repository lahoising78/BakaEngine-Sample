#include "baka_matrix.h"
#include "baka_logger.h"
#include <string.h>
#include <math.h>

void matrix_print(Matrix4 mat)
{
    int i;

    for(i = 0; i < 4; i++)
    {
        bakalog("%.2f %.2f %.2f %.2f", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
    }
}

void matrix_identity(Matrix4 mat)
{
    matrix_zero(mat);
    mat[0][0] = 1;
    mat[1][1] = 1;
    mat[2][2] = 1;
    mat[3][3] = 1;
}

void matrix_zero(Matrix4 mat)
{
    memset(mat, 0, sizeof(Matrix4));
}

void matrix_view(Matrix4 out, Vec3 position, Vec3 target, Vec3 up)
{
    Vec3 f,s,u;
    f = target - position;
    vec3_normalize(f);
    
    s = vec3_cross_product(f, up);
    vec3_normalize(s);
    
    u = vec3_cross_product(s, f);
 
    matrix_identity(out);
    out[0][0] = s.x;
    out[1][0] = s.y;
    out[2][0] = s.z;
    out[0][1] = u.x;
    out[1][1] = u.y;
    out[2][1] = u.z;
    out[0][2] = -f.x;
    out[1][2] = -f.y;
    out[2][2] = -f.z;
    out[3][0] = vec3_dot_product(s, position)?-vec3_dot_product(s, position):0;
    out[3][1] = vec3_dot_product(u, position)?-vec3_dot_product(u, position):0;
    out[3][2] = vec3_dot_product(f, position)?vec3_dot_product(f, position):0;
}

void matrix_perspective(Matrix4 mat, float fov, float aspect, float near, float far)
{
    float halftanfov = tan(fov * 0.5);
    matrix_zero(mat);

    if (aspect == 0)
    {
        bakalog("gfc_matrix_perspective: aspect ratio cannot be zero");
        return;
    }
    if (halftanfov == 0)
    {
        bakalog("gfc_matrix_perspective: bad fov");
        return;
    }
    if (near == far)
    {
        bakalog("gfc_matrix_perspective: near plane and far plane cannot be the same");
        return;
    }

    matrix_zero(mat);
    mat[0][0] = 1 / (aspect * halftanfov);
    mat[1][1] = 1 / (halftanfov);
    mat[2][2] = - ((far + near) / (far - near));
    mat[2][3] = -1;
    if ((far - near) == 0)
    {
        mat[3][2] = 0;
    }
    else
    mat[3][2] = -(2 * far * near) / (far - near);
    return;
}