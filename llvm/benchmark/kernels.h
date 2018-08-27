
#ifndef BENCHMARK_KERNELS_H
#define BENCHMARK_KERNELS_H


typedef struct {
    uint64_t xdiff, ydiff;
    double factor;
} StencilPoint;

typedef struct {
    uint64_t points;
    StencilPoint p[];
} Stencil;

typedef struct {
    double factor;
    uint64_t points;
    StencilPoint* p;
} StencilFactor;

typedef struct {
    uint64_t factors;
    StencilFactor f[];
} SortedStencil;


#ifndef STENCIL_INTERLINES
#define STENCIL_INTERLINES 0
#endif

#define STENCIL_N ((STENCIL_INTERLINES) * 8 + 8)
#define STENCIL_INDEX(x,y) ((y) * ((STENCIL_N) + 1) + (x))
#define STENCIL_OFFSET(base,x,y) ((base) + (y) * ((STENCIL_N) + 1) + (x))

typedef void(*StencilFunction)(void*, double* restrict, double* restrict, uint64_t);

void stencil_element_native(void* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_element_struct(Stencil* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_element_sorted_struct(SortedStencil* a, double* restrict b, double* restrict c, uint64_t index);

void stencil_line_native(void* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_line_struct(Stencil* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_line_sorted_struct(SortedStencil* a, double* restrict b, double* restrict c, uint64_t index);

void stencil_matrix_native(void* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_matrix_struct(Stencil* a, double* restrict b, double* restrict c, uint64_t index);
void stencil_matrix_sorted_struct(SortedStencil* a, double* restrict b, double* restrict c, uint64_t index);

void stencil_element_dbrew(void* a, double* restrict b, double* restrict c, uint64_t index, StencilFunction fn);
void stencil_line_dbrew(void* a, double* restrict b, double* restrict c, uint64_t index, StencilFunction fn);
void stencil_matrix_dbrew(void* a, double* restrict b, double* restrict c, uint64_t index, StencilFunction fn);

#endif

