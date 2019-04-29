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
#include <drob.h>

#include "timer.h"

enum TransMode {
    TRANSMODE_PLAIN = 0,
    TRANSMODE_DBREW,
    TRANSMODE_LLVM,
    TRANSMODE_DROB,
    TRANSMODE_MAX
};

typedef double (* ExpFunc)(double, size_t);

static
double
fast_exp(double val, size_t exp)
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
benchmark_run(size_t run_count, enum TransMode mode)
{
    ExpFunc func;
    JTimer timerCompile = {0}, timerRun = {0};

    LLEngine* state = NULL;
    LLFunction* llfn = NULL;
    Rewriter* r = NULL;


    __asm__ volatile("" ::: "memory");
    JTimerCont(&timerCompile);
    __asm__ volatile("" ::: "memory");

    switch (mode) {
    case TRANSMODE_DBREW: {
        r = dbrew_new();

        dbrew_verbose(r, false, false, false);
        dbrew_optverbose(r, false);
        dbrew_set_decoding_capacity(r, 100000, 100);
        dbrew_set_capture_capacity(r, 100000, 100, 10000);
        dbrew_set_function(r, (uintptr_t)fast_exp);

        dbrew_config_parcount(r, 2);
        dbrew_config_force_unknown(r, 0);
        dbrew_config_staticpar(r, 1);
        func = dbrew_rewrite(r, 0, 40);
        break;
    }
    case TRANSMODE_LLVM: {
        LLFunctionConfig llconfig = {
            .name = "exp",
            .stackSize = 128,
            .signature = 02772,
        };

        state = ll_engine_init();

        llfn = ll_decode_function(fast_exp, &llconfig, state);
        assert(llfn != NULL);

        llfn = ll_function_specialize(llfn, 1, 40, 0, state);
        ll_engine_optimize(state, 3);
        // ll_engine_dump(state);
        func = (ExpFunc) ll_function_get_pointer(llfn, state);
        break;
    }
    case TRANSMODE_DROB : {
        int ret = 0;
        drob_cfg *cfg = NULL;

        cfg = drob_cfg_new2(DROB_PARAM_TYPE_DOUBLE, DROB_PARAM_TYPE_DOUBLE,
                            DROB_PARAM_TYPE_ULONG);
        if (!cfg) {
            fprintf(stderr, "DROB config could not be created\n");
            exit(-1);
        }
        drob_cfg_fail_on_unmodelled(cfg, true);
        drob_cfg_set_error_handling(cfg, DROB_ERROR_HANDLING_ABORT);
        drob_cfg_set_simple_loop_unroll_count(cfg, 64);
        ret |= drob_cfg_set_param_ulong(cfg, 1, 40);
        if (ret) {
            fprintf(stderr, "DROB config could not be modified\n");
            exit(-1);
        }

        func = (uintptr_t) drob_optimize((void *) fast_exp, cfg);
        drob_cfg_free(cfg);
        break;
    }
    default:
        func = fast_exp;
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
    printf("trans=%d,ctime=%f;rtime=%f\n", mode, JTimerRead(&timerCompile), JTimerRead(&timerRun));

    if (state != NULL)
        ll_engine_dispose(state);

    if (r != NULL)
        dbrew_free(r);

    if (mode == TRANSMODE_DROB)
        drob_free((drob_f)func);
}

int
main(int argc, char** argv)
{
    if (argc < 4) {
        printf("Usage: %s [transmode] [compiles] [runs per compile]\n", argv[0]);
        return 1;
    }

    int mode = strtoul(argv[1], NULL, 0);
    size_t iter_count = strtoul(argv[2], NULL, 0);
    size_t run_count = strtoul(argv[3], NULL, 0);

    if (mode == TRANSMODE_DROB) {
        drob_setup();
    }

    for (size_t i = 0; i < iter_count; i++)
    {
        benchmark_run(run_count, mode);
    }

    if (mode == TRANSMODE_DROB) {
        drob_teardown();
    }

    return 0;
}
