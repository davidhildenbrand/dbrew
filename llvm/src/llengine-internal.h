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

#ifndef LL_ENGINE_INTERNAL_H
#define LL_ENGINE_INTERNAL_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <llvm-c/ExecutionEngine.h>

#include <llengine.h>
#include <llfunction.h>


#define warn_if_reached() do { printf("!WARN %s: Code should not be reached.\n", __func__); __asm__("int3"); } while (0)

struct LLEngine {
    /**
     * \brief The LLVM Context
     **/
    LLVMContextRef context;
    /**
     * \brief The LLVM Module
     **/
    LLVMModuleRef module;
    /**
     * \brief The LLVM execution engine
     **/
    LLVMExecutionEngineRef engine;

    /**
     * \brief The function count
     **/
    size_t functionCount;
    /**
     * \brief The allocated size for function
     **/
    size_t functionsAllocated;
    /**
     * \brief The functions of the module
     **/
    LLFunction** functions;

    LLVMValueRef globalBase;
};

#endif
