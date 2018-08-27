
#include <stdint.h>

#include "kernels.h"

inline
void
stencil_element_native(void* __attribute__((unused)) a, double* restrict b, double* restrict c, uint64_t index)
{
    c[index] = 0.25 * (b[STENCIL_OFFSET(index, 0, -1)] + b[STENCIL_OFFSET(index, 0, 1)] + b[STENCIL_OFFSET(index, -1, 0)] + b[STENCIL_OFFSET(index, 1, 0)]);
}

inline
void
stencil_element_struct(Stencil* restrict s, double* restrict b, double* restrict c, uint64_t index)
{
    double result1 = 0;
    for(uint64_t i = 0; i < s->points; i++)
    {
        StencilPoint* p = s->p + i;
        result1 += p->factor * b[STENCIL_OFFSET(index, p->xdiff, p->ydiff)];
    }
    c[index] = result1;
}

inline
void
stencil_element_sorted_struct(SortedStencil* restrict s, double* restrict b, double* restrict c, uint64_t index)
{
    double result1 = 0, sum1 = 0;
    for (uint64_t i = 0; i < s->factors; i++)
    {
        StencilFactor* sf = s->f + i;
        StencilPoint* p = sf->p;
        sum1 = b[STENCIL_OFFSET(index, p->xdiff, p->ydiff)];
        for (uint64_t j = 1; j < sf->points; j++)
        {
            p = sf->p + j;
            sum1 += b[STENCIL_OFFSET(index, p->xdiff, p->ydiff)];
        }
        result1 += sf->factor * sum1;
    }
    c[index] = result1;
}

void
stencil_element_dbrew(void* a, double* restrict b, double* restrict c, uint64_t index, StencilFunction fn)
{
    fn(a, b, c, STENCIL_OFFSET(index, 0, 0));
    __asm__ volatile("" ::: "memory"); // avoid tail calls
}

void
stencil_line_native(void* a, double* restrict b, double* restrict c, uint64_t index)
{
    uint64_t j;
    for (j = 1; j < STENCIL_N; ++j)
    {
        stencil_element_native(a, b, c, STENCIL_OFFSET(index, j, 0));
    }
}

void
stencil_line_struct(Stencil* a, double* restrict b, double* restrict c, uint64_t index)
{
    uint64_t j;
    for (j = 1; j < STENCIL_N; ++j)
    {
        stencil_element_struct(a, b, c, STENCIL_OFFSET(index, j, 0));
    }
}

void
stencil_line_sorted_struct(SortedStencil* a, double* restrict b, double* restrict c, uint64_t index)
{
    uint64_t j;
    for (j = 1; j < STENCIL_N; ++j)
    {
        stencil_element_sorted_struct(a, b, c, STENCIL_OFFSET(index, j, 0));
    }
}

void
stencil_line_dbrew(void* a, double* restrict b, double* restrict c, uint64_t index, StencilFunction fn)
{
    uint64_t j;
    for (j = 1; j < STENCIL_N; ++j)
    {
        fn(a, b, c, STENCIL_OFFSET(index, j, 0));
    }
}

void
stencil_matrix_native(void* a, double* restrict b, double* restrict c, uint64_t __attribute__((unused)) index)
{
    uint64_t i, j;
    for (i = 1; i < STENCIL_N; ++i)
    {
        for (j = 1; j < STENCIL_N; ++j)
        {
            stencil_element_native(a, b, c, STENCIL_INDEX(j, i));
        }
    }
}

void
stencil_matrix_struct(Stencil* a, double* restrict b, double* restrict c, uint64_t __attribute__((unused)) index)
{
    uint64_t i, j;
    for (i = 1; i < STENCIL_N; ++i)
    {
        for (j = 1; j < STENCIL_N; ++j)
        {
            stencil_element_struct(a, b, c, STENCIL_INDEX(j, i));
        }
    }
}

void
stencil_matrix_sorted_struct(SortedStencil* a, double* restrict b, double* restrict c, uint64_t __attribute__((unused)) index)
{
    uint64_t i, j;
    for (i = 1; i < STENCIL_N; ++i)
    {
        for (j = 1; j < STENCIL_N; ++j)
        {
            stencil_element_sorted_struct(a, b, c, STENCIL_INDEX(j, i));
        }
    }
}

void
stencil_matrix_dbrew(void* a, double* restrict b, double* restrict c, uint64_t __attribute__((unused)) index, StencilFunction fn)
{
    uint64_t i, j;
    for (i = 1; i < STENCIL_N; ++i)
    {
        for (j = 1; j < STENCIL_N; ++j)
        {
            fn(a, b, c, STENCIL_INDEX(j, i));
        }
    }
}


