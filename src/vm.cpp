/*
 * Copyright (c) 2022, Roland Marchand <roland.marchand@protonmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "vm.hpp"
#include "debug/debug.hpp"

#include <stdio.h>

static struct virtual_machine vm;
static enum virtual_machine::result run();

enum virtual_machine::result virtual_machine::interpret(struct lump *lmp)
{
	vm.lump = lmp;
	vm.pc = lmp->array;
	return run();
}

static enum virtual_machine::result run()
{
#define READ_BYTE() (*vm.pc++)

	while (1) {
#ifdef DEBUG_TRACE_EXECUTION
		disassemble_instruction(vm.lump, (int)(vm.pc - vm.lump->array));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE()) {
		case op_code::RETURN:
			return virtual_machine::OK;
		case op_code::CONSTANT:
			printf("%f\n", vm.lump->constants->array[READ_BYTE()]);
			break;
		case op_code::CONSTANT_LONG: {
			uint8_t byte1 = READ_BYTE();
			uint8_t byte2 = READ_BYTE();
			printf("%f\n", vm.lump->constants->array[byte1 << 8 | byte2]);
			break;
		}
		default:
			break;
		}
	}
	
#undef READ_BYTE
}
