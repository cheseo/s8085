/* Copyright (C) 2022 by boink */
/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <https://www.gnu.org/licenses/>. */
#ifndef GLUE_H
#define GLUE_H
#include <stdio.h>
#include <stdint.h>

#define EPRINTF(fmt, ...)				\
	fprintf(stderr, "%s:%u: " fmt, __FILE__ , __LINE__, ##__VA_ARGS__);

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
enum op{ OPCODE, OPERAND1, OPERAND2};
enum ins_mode { REGISTER, IMMEDIATE, IMPLIED, UNDEFINED};

struct registers{
	uint8_t a, b, c, d, e, h, l;
	_Bool carry, zero, sign, parity, auxiliary;
};

struct instruction{
	char *opcode;
	char *operand1;
	char *operand2;
	uint8_t tmp_int;
	enum ins_mode faddr;
	enum ins_mode mode;
};

int set_register(struct registers *s,char reg, uint8_t val);
int get_register(struct registers *s, char reg);
void call_ins(struct instruction *i, struct registers *r);
void print_registers(struct registers *r);
void initialize_instruction(struct instruction *r);
void set_ins_mode(struct instruction *i);
#endif
