/**
 * Fast exponentiation benchmark.
 * Written by Alexis Engelke, 2019.
 */


#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <dbrew.h>
#include <dbrew-llvm.h>

#include "timer.h"


typedef double (* ExpFunc)(double, size_t);

static
double
exp(double val, size_t exp)
{
    double ans = 1;
    while (exp)
    {
        if (exp & 1) ans *= val;
        val *= val;
        exp = exp >> 1;
    }
    return ans;
}

static
void
benchmark_run(size_t run_count, bool transform)
{
    ExpFunc func;
    JTimer timerCompile = {0}, timerRun = {0};

    LLState* state = NULL;
    LLFunction* llfn = NULL;
    Rewriter* r = NULL;

    __asm__ volatile("" ::: "memory");
    JTimerCont(&timerCompile);
    __asm__ volatile("" ::: "memory");
    if (!transform)
        func = exp;
    else
    {
        LLConfig llconfig = {
            .name = "exp",
            .stackSize = 128,
            .signature = 02772,
        };

        r = dbrew_new();
        state = ll_engine_init();

        llfn = ll_decode_function(exp, (DecodeFunc) dbrew_decode, r, &llconfig, state);
        assert(llfn != NULL);

        llfn = ll_function_specialize(llfn, 1, 40, 0, state);
        ll_engine_optimize(state, 3);
        // ll_engine_dump(state);
        func = (ExpFunc) ll_function_get_pointer(llfn, state);
    }

    __asm__ volatile("" ::: "memory");
    JTimerStop(&timerCompile);
    JTimerCont(&timerRun);
    __asm__ volatile("" ::: "memory");
    double result = 0;
    for (size_t runs = 0; runs < run_count; runs++)
        result = func(1.0123, 40);
    __asm__ volatile("" ::: "memory");
    JTimerStop(&timerRun);
    __asm__ volatile("" ::: "memory");

    printf("1.0123^40 = %f\n", result);
    printf("trans=%d,ctime=%f;rtime=%f\n", !!transform, JTimerRead(&timerCompile), JTimerRead(&timerRun));

    if (state != NULL)
        ll_engine_dispose(state);

    if (r != NULL)
        dbrew_free(r);
}

int
main(int argc, char** argv)
{
    if (argc < 4) {
        printf("Usage: %s [transmode] [compiles] [runs per compile]\n", argv[0]);
        return 1;
    }

    bool transform = strtoul(argv[1], NULL, 0) != 0;
    size_t iter_count = strtoul(argv[2], NULL, 0);
    size_t run_count = strtoul(argv[3], NULL, 0);

    for (size_t i = 0; i < iter_count; i++)
    {
        benchmark_run(run_count, transform);
    }


    return 0;
}
