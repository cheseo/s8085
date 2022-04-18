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

#include <stdlib.h> 		/* strtol */
#include "glue.h"
#include "instructions.h"

#define SET_ZERO(val) do{			\
		if(val) r->zero =  0;		\
		else r-> zero = 1;		\
	} while(0)
#define SET_CARRY(val) do{		\
		if(val) r->carry = 1;	\
		else r->carry = 0;	\
	} while(0)

uint16_t ins_to_pos(struct instruction *ins){
	uint16_t tmp;
	tmp = ins->arg2;
	tmp <<= 8;
	tmp |= ins->arg1;
	return tmp;
}

void nop(){}
void hlt(){
	EPRINTF("hlt instruction. exiting.\n");
	exit(EXIT_SUCCESS);
}
void rrc(struct registers *r, struct instruction *ins){
	(void)ins;
	int overflow = r->a & 1;
	r->a>>= 1;
	if(overflow)
		/* r->a |=  0b10000000; */
		r-> a |= ( 1 << 7 ) ;
	SET_CARRY(overflow);
}


void xchg(struct registers *r, struct instruction *ins){
	(void)ins;
	uint8_t tmp1 = r->h, tmp2 = r->l;
	set_register_pair(r, 'h', r->d, r->e);
	r->d = tmp1;
	r->e = tmp2;
}

void jmp(struct registers *r, struct instruction *ins){
	if(!ins->arg2){
		halt_exec = 1;
		wait_pos = ins->arg3;
		EPRINTF("unknown label found, not executing any instruction henceforth.\n");
	}else{
		jmp_pos = ins->arg1;
		change_exec(r);
	}
}

void jnc(struct registers *r, struct instruction *ins){
	if(!(r->carry))
		jmp(r, ins);
}
void jc(struct registers *r, struct instruction *ins){
	if(r->carry)
		jmp(r,ins);
}
void jz(struct registers *r, struct instruction *ins){
	if(r->zero)
		jmp(r,ins);
}
void jnz(struct registers *r, struct instruction *ins){
	if(!(r->zero))
		jmp(r,ins);
}

void add(struct registers *r, struct instruction *ins){
	uint8_t tmp = get_register(r, ins->arg1);
	add_with_carry(r, 'a', tmp);
}

void inr(struct registers *r, struct instruction *ins){
	char reg=ins->arg1;
	set_register(r, reg, get_register(r, reg) + 1);
}
void dcr(struct registers *r, struct instruction *ins){
	char reg = ins->arg1;
	set_register(r, reg, get_register(r, reg) - 1);
	SET_ZERO(get_register(r, reg));
}

void mov(struct registers *r, struct instruction *ins){
	char dest = ins->arg1;
	char source = ins->arg2;
	set_register(r, dest, get_register(r, source));
}

void mvi(struct registers *r, struct instruction *ins){
	char regi = ins->arg1;
	uint8_t val = ins->arg2;
	set_register(r, regi, val);
}
void lxi(struct registers *r, struct instruction *ins){
	set_register_pair(r, ins->arg1, ins->arg2, ins->arg3);
}

void inx(struct registers *r, struct instruction *ins){
	uint16_t val = get_register_pair(r, ins->arg1);
	val++;
	set_register_pair(r, ins->arg1, val>>8, val);
	SET_ZERO(val);
	SET_CARRY((!val));
}
void dcx(struct registers *r, struct instruction *ins){
	uint16_t val = get_register_pair(r, ins->arg1);
	val--;
	set_register_pair(r, ins->arg1, val>>8, val);
	SET_ZERO(val);
}
void adi(struct registers *r, struct instruction *ins){
	add_with_carry(r, 'a', ins->arg1);
	SET_ZERO(get_register(r, 'a'));
}

void lda(struct registers *r, struct instruction *ins){
	r->a=get_line_mem(ins_to_pos(ins));
	SET_ZERO(r->a);
}

void ldax(struct registers *r, struct instruction *ins){
	r->a=get_line_pair(r, ins->arg1);
	SET_ZERO(r->a);
}

void lhld(struct registers *r, struct instruction *ins){
	uint16_t pos = ins_to_pos(ins);
	r->h=get_line_mem(pos++);
	r->l=get_line_mem(pos);
}
void sta(struct registers *r, struct instruction *ins){
	set_line_mem(ins_to_pos(ins), r->a);
	SET_ZERO(r->a);
}
void stax(struct registers *r, struct instruction *ins){
	set_line_pair(r, r->a, ins->arg1);
	SET_ZERO(r->a);
}
void shld(struct registers *r, struct instruction *ins){
	uint16_t pos = ins_to_pos(ins);
	set_line_mem(pos++, r->l);
	set_line_mem(pos, r->h);
}
void cpi(struct registers *r, struct instruction *ins){
	uint8_t val = ins->arg1, acc = r->a ;
	if(acc < val){
	  SET_CARRY(1);
	  SET_ZERO(0);
	}else if (acc == val){
	  SET_ZERO(1);
	  SET_CARRY(0);
	}else{
	  SET_CARRY(0);
	  SET_ZERO(0);
	}
}

void cmp(struct registers *r, struct instruction *ins){
	uint8_t reg = ins->arg1;
	ins->arg1 = get_register(r, reg);
	cpi(r, ins);
	ins->arg1 = reg;
}
