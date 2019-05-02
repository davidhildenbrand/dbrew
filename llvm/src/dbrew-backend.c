
#include <common.h>
#include <engine.h>

#include <dbrew-backend.h>

#include <dbrew-decoder.h>
#include <llbasicblock.h>
#include <llfunc.h>
#include <llinstr.h>

#include <llengine.h>
#include <llengine-internal.h>
#include <llfunction.h>
#include <llfunction-internal.h>

/**
 * Code generation back-end for DBrew.
 *
 * \author Alexis Engelke
 *
 * \param rewriter The DBrew rewriter
 **/
void
dbrew_llvm_backend(Rewriter* rewriter)
{
    LLEngine* state = ll_engine_init();

    LLFunctionConfig config = {
        .stackSize = 128,
        .signature = 026, // 6 pointer params, returns i64
        .name = "__dbrew__"
    };

    LLFunction* function = ll_function_new_definition(rewriter->func, &config, state);

    LLInstr instr;
    for (int i = 0; i < rewriter->capBBCount; i++)
    {
        CBB* cbb = rewriter->capBB + i;
        LLBasicBlock* bb = ll_func_add_block(function->func);

        cbb->generatorData = bb;

        for (int j = 0; j < cbb->count; j++)
        {
            lldbrew_convert_instr(cbb->instr + j, &instr);
            ll_basic_block_add_inst(bb, &instr);
        }
        lldbrew_convert_cbb(cbb, &instr);
        if (instr.type != LL_INS_RET)
            ll_basic_block_add_inst(bb, &instr);
    }
    for (int i = 0; i < rewriter->capBBCount; i++)
    {
        CBB* cbb = rewriter->capBB + i;
        LLBasicBlock* branch = NULL;
        LLBasicBlock* fallThrough = NULL;

        if (cbb->nextBranch != NULL)
            branch = cbb->nextBranch->generatorData;
        if (cbb->nextFallThrough != NULL)
            fallThrough = cbb->nextFallThrough->generatorData;

        ll_basic_block_add_branches(cbb->generatorData, branch, fallThrough);
    }

    function->llvmFunction = ll_func_lift(function->func);

    if (function->llvmFunction == NULL)
    {
        warn_if_reached();
        rewriter->generatedCodeAddr = 0;

        return;
    }

    ll_engine_optimize(state, 3);

    if (rewriter->showOptSteps)
        ll_engine_dump(state);

    rewriter->generatedCodeAddr = (uintptr_t) ll_function_get_pointer(function, state);
    rewriter->generatedCodeSize = 0;
}

/**
 * Rewrite a function using DBrew and the LLVM optimization and code generation.
 *
 * \author Alexis Engelke
 *
 * \param r The DBrew rewriter
 **/
uintptr_t
dbrew_llvm_rewrite(Rewriter* r, ...)
{
    va_list argptr;

    va_start(argptr, r);
    vEmulateAndCapture(r, argptr);
    va_end(argptr);

    // runOptsOnCaptured(r);
    dbrew_llvm_backend(r);

    return r->generatedCodeAddr;
}
