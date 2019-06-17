/**
 * This file is part of DBrew, the dynamic binary rewriting library.
 *
 * (c) 2015-2016, Josef Weidendorfer <josef.weidendorfer@gmx.de>
 *
 * DBrew is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation, either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * DBrew is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DBrew.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 **/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Transforms/IPO.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Transforms/Vectorize.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>

#include <llengine.h>
#include <llengine-internal.h>

#include <rellume/decoder.h>

#include <llfunction.h>
#include <llfunction-internal.h>
#include <support-internal.h>

/**
 * \defgroup LLEngine Engine
 * \brief Common public APIs and State management
 *
 * @{
 **/

static
LLEngine*
ll_state_create(void)
{
    LLEngine* state;

    state = calloc(1, sizeof(LLEngine));
    state->context = LLVMContextCreate();

    return state;
}

static
bool
ll_state_init_common(LLEngine* state)
{
    state->functionCount = 0;
    state->functionsAllocated = 0;
    state->functions = NULL;

    LLVMSetTarget(state->module, "x86_64-pc-linux-gnu");
    LLVMLinkInMCJIT();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeTarget();

    char* outerr = NULL;
    if (dbll_support_create_mcjit_compiler(&state->engine, state->module, &outerr))
    {
        printf("CRITICAL Could not setup execution engine: %s", outerr);
        free(outerr);

        return true;
    }

    // Construct global variable to avoid inttoptr instructions
    LLVMTypeRef i8 = LLVMInt8TypeInContext(state->context);
    state->globalBase = LLVMAddGlobal(state->module, i8, "__ll_global_base__");
    LLVMAddGlobalMapping(state->engine, state->globalBase, (void*) 0x1000);

    return false;
}

/**
 * Initialize the LLVM module with the given configuration. This includes
 * setting up the MCJIT compiler and the LLVM module.
 *
 * \author Alexis Engelke
 *
 * \returns A new module
 **/
LLEngine*
ll_engine_init(void)
{
    LLEngine* state = ll_state_create();
    if (state == NULL)
        return NULL;

    state->module = LLVMModuleCreateWithNameInContext("<llengine>", state->context);

    if (ll_state_init_common(state))
    {
        free(state);
        return NULL;
    }

    return state;
}

LLEngine*
ll_engine_init_from_bc_file(char* fileName)
{
    LLEngine* state = ll_state_create();
    if (state == NULL)
        return NULL;

    LLVMMemoryBufferRef bc_membuf;
    char* outerr;
    if (LLVMCreateMemoryBufferWithContentsOfFile(fileName, &bc_membuf, &outerr))
    {
        printf("CRITICAL Could not read bc file %s: %s\n", fileName, outerr);
        free(state);

        return NULL;
    }

    if (LLVMParseBitcodeInContext(state->context, bc_membuf, &state->module, &outerr))
    {
        printf("CRITICAL Could not parse bc file: %s\n", outerr);
        LLVMDisposeMemoryBuffer(bc_membuf);
        free(state);

        return NULL;
    }

    LLVMDisposeMemoryBuffer(bc_membuf);

    // Set all imported functions to private linkage. This avoids code
    // generation and optimization for functions that are unused.
    LLVMValueRef llvmFunction = LLVMGetFirstFunction(state->module);
    while (llvmFunction != NULL)
    {
        // Function declarations, like memset, need to be public.
        if (!LLVMIsDeclaration(llvmFunction))
            LLVMSetLinkage(llvmFunction, LLVMPrivateLinkage);
        llvmFunction = LLVMGetNextFunction(llvmFunction);
    }

    if (ll_state_init_common(state))
    {
        free(state);
        return NULL;
    }

    return state;
}

/**
 * Dispose an engine. The functions generated will not be usable any longer.
 *
 * \author Alexis Engelke
 *
 * \param state The module state
 **/
void
ll_engine_dispose(LLEngine* state)
{
    // LLVMDisposeModule(state->module);
    // LLVMDisposeBuilder(state->builder);
    LLVMDisposeExecutionEngine(state->engine);
    // Disposing the context sometimes crashes LLVM (v6). Valgrind complains
    // about use-after-free and double-free errors...
    // LLVMContextDispose(state->context);

    free(state);
}

/**
 * Optimize all functions in the module.
 *
 * \author Alexis Engelke
 *
 * \param state The module state
 * \param level The optimization level
 **/
void
ll_engine_optimize(LLEngine* state, int level)
{
    LLVMPassManagerRef pm = LLVMCreatePassManager();
    LLVMPassManagerBuilderRef pmb = LLVMPassManagerBuilderCreate();

    // Run inliner early.
    LLVMAddAlwaysInlinerPass(pm);

    // Run some light-weight optimization passes before the main O3 pipeline
    if (level >= 1)
    {
        // Simple pass to remove trivially redundant instructions.
        LLVMAddEarlyCSEPass(pm);

        // Run an additional GVN pass to remove a lot of unused instructions.
        LLVMAddGVNPass(pm);

        // InstrCombine will also remove some bloat
        LLVMAddInstructionCombiningPass(pm);
    }

    LLVMPassManagerBuilderSetOptLevel(pmb, level);
    // dbll_support_pass_manager_builder_set_enable_vectorize(pmb, level >= 3);

    LLVMPassManagerBuilderPopulateModulePassManager(pmb, pm);
    LLVMPassManagerBuilderDispose(pmb);

    // Add clean-up passes
    LLVMAddStripSymbolsPass(pm);
    LLVMAddStripDeadPrototypesPass(pm);

    LLVMRunPassManager(pm, state->module);
    LLVMRunPassManager(pm, state->module);

    LLVMDisposePassManager(pm);
}

/**
 * Dump the LLVM IR of the module.
 *
 * \author Alexis Engelke
 *
 * \param state The module state
 **/
void
ll_engine_dump(LLEngine* state)
{
    char* module = LLVMPrintModuleToString(state->module);

    puts(module);

    LLVMDisposeMessage(module);
}

void
ll_engine_disassemble(LLEngine* state)
{
    FILE* llc = popen("llc -filetype=asm", "w");

    LLVMWriteBitcodeToFD(state->module, fileno(llc), false, false);

    pclose(llc);
}

/**
 * @}
 **/
