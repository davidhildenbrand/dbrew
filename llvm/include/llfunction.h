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

#ifndef LL_FUNCTION_H
#define LL_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <llengine.h>


struct LLFunction;

typedef struct LLFunction LLFunction;

struct LLFunctionConfig {
    /**
     * \brief The name of the function
     **/
    const char* name;

    /**
     * \brief The size of the emulated stack
     **/
    size_t stackSize;

    bool fastMath;
    bool forceLoopUnroll;

    /**
     * \brief Bitwise representation of the function signature
     **/
    size_t signature;

    /**
     * \brief Whether the function is private
     **/
    bool internal;

    /**
     * \brief Whether to disable instruction deduplication.
     *
     * This option only has effect when used with #ll_decode_function.
     **/
    bool disableInstrDedup;
};

typedef struct LLFunctionConfig LLFunctionConfig;

LLFunction* ll_function_declare(uintptr_t, uint64_t, const char*, LLEngine* state);
LLFunction* ll_function_specialize(LLFunction*, uintptr_t, uintptr_t, size_t, LLEngine* state);
LLFunction* ll_function_wrap_external(const char*, LLEngine* state);
void ll_function_dispose(LLFunction*);
void ll_function_dump(LLFunction*);
void* ll_function_get_pointer(LLFunction*, LLEngine*);

LLFunction* ll_decode_function(uintptr_t, LLFunctionConfig*, LLEngine*);

#endif
