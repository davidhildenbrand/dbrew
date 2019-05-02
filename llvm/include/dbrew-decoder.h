/**
 * \file
 **/

#ifndef DBREW_DECODER_H
#define DBREW_DECODER_H

#include <lldecoder.h>
#include <llinstr.h>

void lldbrew_convert_instr(void*, LLInstr*);
void lldbrew_convert_cbb(void*, LLInstr*);
void* dbrew_decoder(void* rewriter, uintptr_t address);

#endif
