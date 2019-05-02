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

#ifndef LL_ENGINE_H
#define LL_ENGINE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

// #include <dbrew.h>

struct LLEngine;

typedef struct LLEngine LLEngine;

LLEngine* ll_engine_init(void);
LLEngine* ll_engine_init_from_bc_file(char*);
void ll_engine_dispose(LLEngine*);

void ll_engine_optimize(LLEngine*, int);
void ll_engine_dump(LLEngine*);
void ll_engine_disassemble(LLEngine*);

#endif
