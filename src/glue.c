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

#include <string.h>
#include <stdlib.h>		/* for strtol */
#include <ctype.h>
#include "glue.h"
#include "instructions.h"

struct {
	char *name;
	void (*fptr)(struct registers *);
}
	imp[] = { {"rrc", rrc},
		  {"nop", nop},
		  {"inr", inr},
		  {"dcr", dcr} };

struct {
	char *name;
	void (*fptr)(struct registers *, char, uint8_t);
}
	imm[] = { {"mvi", mvi} };


struct {
	char *name;
	void (*fptr)(struct registers *, char, char);
}
	reg[] = { {"mov", mov} };

int set_register(struct registers *r,char regi, uint8_t val){
	switch(tolower(regi)){
	case 'a':
		r->a = val;
		break;
	case 'b':
		r->b = val;
		break;
	case 'c':
		r->c = val;
		break;
	case 'd':
		r->d = val;
		break;
	case 'e':
		r->e = val;
		break;
	case 'h':
		r->h = val;
		break;
	case 'l':
		r->l = val;
		break;
	default:
		EPRINTF("invalid register to set\n");
		return 0;
	}
	return 1;
}

int get_register(struct registers *s, char regi){
	switch(tolower(regi)){
	case 'a':
		return s->a;
		break;
	case 'b':
		return s->b;
		break;
	case 'c':
		return s->c;
		break;
	case 'd':
		return s->d;
		break;
	case 'e':
		return s->e;
		break;
	case 'h':
		return s->h;
		break;
	case 'l':
		return s->l;
		break;
	default:
		EPRINTF("invalid register to get\n");
		return -1;
	}
}

void sanitize_ins(struct instruction *ins){
	set_ins_mode(ins);
	if(ins->mode == IMMEDIATE){
		ins->tmp_int = strtol(ins->operand2, NULL, 16);
		EPRINTF("strtol said operand2 was: %u\n", ins->tmp_int);
	}

	
}

void call_ins(struct instruction *ins, struct registers *regi){
	int i = ins->faddr;
	if(ins->mode == REGISTER){
		/* void (*reg_fptr[])(struct registers *, char, char) */
		(*reg[i].fptr)(regi, *(ins->operand1), *(ins->operand2));
	}else if(ins->mode == IMMEDIATE){
		/* void (*imm_fptr[])(struct registers *, char, uint8_t) */
		(*imm[i].fptr)(regi, *(ins->operand1), ins->tmp_int);
	}else if(ins->mode == IMPLIED){
		/* void (*imp_fptr[])(struct registers *) */
		(*imp[i].fptr)(regi);
	}else{
		EPRINTF("Can't call instruction of mode: %u\n", ins->mode);
	}
	
}

void print_registers(struct registers *r){
	printf("Registers(in hex):\n"
	       "    A: %x\n"
	       "B: %x\t"
	       "C: %x\n"
	       "D: %x\t"
	       "E: %x\n"
	       "H: %x\t"
	       "H: %x\n", r->a, r->b, r->c, r->d, r->e, r->h, r->l);
}

void initialize_instruction(struct instruction *r){
	r->opcode = NULL;
	r->operand1 = NULL;
	r->operand2 = NULL;
}
	
	
		
void set_ins_mode(struct instruction *ins){
	for(size_t i = 0 ; i<ARRAY_SIZE(reg) ; i++)
		if(!strcasecmp(ins->opcode, reg[i].name)){
			ins->mode = REGISTER;
			ins->faddr = i;
			return;
		}
	for(size_t i = 0; i<ARRAY_SIZE(imm) ; i++)
		if(!strcasecmp(ins->opcode, imm[i].name)){
			ins->mode = IMMEDIATE ;
			ins->faddr = i;
			return ;
		}
	for(size_t i = 0; i<ARRAY_SIZE(imp) ; i++)
		if(!strcasecmp(ins->opcode, imp[i].name)){
			ins->mode = IMPLIED;
			ins->faddr = i;
			return;
		}
	ins->mode = UNDEFINED;
}
