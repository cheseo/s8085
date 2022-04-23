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
#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x
#define EPRINTF(...)				\
  fprintf(stderr, STRINGIFY(__FILE__) ":" STRINGIFY(__LINE__) ":" __VA_ARGS__)

#define ARRAY_SIZE(arr) ((int)(sizeof(arr)/sizeof(arr[0])))

enum ins_mode { ONEARG, TWOARG, IMPLIED};
enum ins_arg { REG_REG, NONE, REG_VAL, VAL, REG, VAL_VAL, LABEL, PAIR, PAIR_VAL };

struct registers{
	uint8_t a, b, c, d, e, h, l;
	_Bool carry, zero, sign, parity, auxiliary;
};

struct instruction{
	uint8_t arg1;
	uint8_t arg2;
	uint8_t arg3;
	unsigned int faddr;
	
};
extern int halt_exec;
extern size_t wait_pos;
extern size_t jmp_pos;
extern FILE *mem_fp;
int set_register(struct registers *s,char reg, uint8_t val);
int get_register(struct registers *s, char reg);
void call_ins(struct instruction *i, struct registers *r);
void print_reg(struct registers *r);
int set_ins(struct instruction *i);
void set_ins_mode(struct instruction *i, char **inst, int len);
int set_args(struct instruction *ins, int i, char **insv, int len);
void add_ins_mem(struct instruction *ins);
void change_exec(struct registers *r);
void strtopair(struct instruction *i, char *str);
int set_register_pair(struct registers *r, char regi, uint8_t higher, uint8_t lower);
void add_with_carry(struct registers *r, char reg, uint8_t val);
uint16_t get_register_pair(struct registers *s, char regi);
uint8_t get_line_mem(uint16_t pos);
void set_line_mem(uint16_t pos, uint8_t data);
uint8_t get_line_pair(struct registers *r, char reg);
void set_line_pair(struct registers *r, uint8_t data, char reg);
#endif
