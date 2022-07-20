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

#include "vm.h"
#include "src/compiler/compiler.h"
#include "debug/debug.h"

#include <stdio.h>

static struct vm vm;

static enum interpret_result run();

enum interpret_result interpret(char *source) {
	struct lump *lmp = lump_init();
	vm.lump = lmp;

	if (compile(source) != COMPILE_OK) {
		lump_free(lmp);
#ifdef DEBUG_TRACE_EXECUTION
		fprintf(stderr, "INTERPRET_COMPILE_ERROR\n");
#endif
		return INTERPRET_COMPILE_ERROR;
	}

	vm.pc = lmp->array;
	vm.stack_top = vm.stack;

	enum interpret_result result = run();

	lump_free(lmp);
	return result;
}

static enum interpret_result run()
{
#define READ_BYTE() (*vm.pc++)

	while (1) {
#ifdef DEBUG_TRACE_EXECUTION
		disassemble_instruction(vm.lump, (int)(vm.pc - vm.lump->array));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE()) {
		case OP_RETURN:
			return INTERPRET_OK;
		case OP_CONSTANT:
			READ_BYTE();
			break;
		case OP_CONSTANT_LONG: {
			READ_BYTE();
			READ_BYTE();
			break;
		}
		default:
			break;
		}
	}
#undef READ_BYTE
}

void vm_push_value(struct value v)
{
	if (vm.stack_top >= vm.stack + VM_STACK_SIZE)
		return;

	*vm.stack_top = v;
	vm.stack_top++;
}

struct value *vm_pop_value()
{
	if (vm.stack_top <= vm.stack)
		return NULL;

	vm.stack_top--;
	return vm.stack_top + 1;
}

void vm_add_constant(double value)
{
	lump_add_constant(vm.lump, value);
}

void vm_add_code(enum op_code code)
{
	lump_add_code(vm.lump, code);
}
