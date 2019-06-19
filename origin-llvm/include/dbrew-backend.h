/**
 * \file
 **/

#ifndef DBREW_BACKEND_H
#define DBREW_BACKEND_H

#include <dbrew.h>

void dbrew_llvm_backend(Rewriter*);
uintptr_t dbrew_llvm_rewrite(Rewriter*, ...);

#endif
