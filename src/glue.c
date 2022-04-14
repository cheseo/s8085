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

int halt_exec = 0;
size_t wait_pos  = 0;
size_t jmp_pos;
FILE *mem_fp;
struct label {
	char name[10];
	size_t addr;
};

struct {
	struct instruction *ins;
	size_t ins_count, ins_size;

	uint8_t *data;

	struct label *lab;
	size_t lab_count, lab_size;

	char **waiting_label;
	size_t waiting_count, waiting_size;
} mem = {NULL, 0, 0,
	NULL,
	NULL, 0, 0,
	NULL, 0, 0};

struct {
	char *name;
	void (*fptr)(struct registers *, struct instruction *);
	enum ins_arg type;
}
	inst[] = { {"rrc", rrc, NONE},
		   {"nop", nop, NONE},
		   {"hlt", hlt, NONE},
		   {"xchg", xchg, NONE},
		   {"inr", inr, REG},
		   {"dcr", dcr, REG},
		   {"jmp", jmp, LABEL},
		   {"jnc", jnc, LABEL},
		   {"jc", jc, LABEL},
		   {"jz", jz, LABEL},
		   {"jnz", jnz, LABEL},
		   {"mvi", mvi, REG_VAL},
		   {"adi", adi, VAL},
		   {"lxi", lxi, PAIR_VAL},
		   {"inx", inx, PAIR},
		   {"dcx", dcx, PAIR},
		   {"mov", mov, REG_REG},
		   {"add", add, REG},
		   {"xchg", xchg, NONE}};

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
	case 'm':
		set_line_hl(r, val);
		break;
	default:
		EPRINTF("invalid register to set, %c\n", regi);
		return 0;
	}
	return 1;
}
int set_register_pair(struct registers *r, char regi, uint8_t higher, uint8_t lower){
	char next;
	switch(tolower(regi)){
	case 'b':
	case 'd':
		next = regi+1;
		break;
	case 'h':
		next = 'l';
		break;
	default:
		EPRINTF("invalid register pair to set, %c\n", regi);
		return 0;
	}
	set_register(r, regi, higher);
	set_register(r, next, lower);
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
	case 'm':
		return get_line_hl(s);
	default:
		EPRINTF("invalid register to get\n");
		return -1;
	}
}

uint16_t get_register_pair(struct registers *r, char regi){
	char next;
	uint16_t ret;
	switch(tolower(regi)){
	case 'b':
	case 'd':
		next = regi+1;
		break;
	case 'h':
		next = 'l';
		break;
	default:
		EPRINTF("invalid register pair to set, %c\n", regi);
		return 0;
	}
	ret = get_register(r, regi);
	ret <<= 8;
	ret += get_register(r, next);
	return ret;
}

void add_with_carry(struct registers *r, char reg, uint8_t val){
	uint8_t tmp = get_register(r, reg);
	if( tmp + val > 0xff)
		r->carry = 1;
	else
		r->carry = 0;
	set_register(r, reg, tmp+val);
}


void call_ins(struct instruction *ins, struct registers *regi){
	inst[ins->faddr].fptr(regi, ins);

}

void print_reg(struct registers *r){
	printf("Registers(in hex):\n"
	       "    A: %x\n"
	       "B: %x\t"
	       "C: %x\n"
	       "D: %x\t"
	       "E: %x\n"
	       "H: %x\t"
	       "L: %x\n", r->a, r->b, r->c, r->d, r->e, r->h, r->l);
}

	/* warning: non-standard bit */
#define INC_ARRAY(ptr, count, size) \
	do{ \
	void *tmp = reallocarray(ptr, count, size); \
	if(!tmp){ \
	perror("reallocarray"); \
	exit(EXIT_FAILURE);\
	} \
	ptr = tmp;\
	} while(0)

void set_waiting(char *label){
	size_t pos = mem.waiting_count;
	size_t total = mem.waiting_size;
	if((total == 0) || (total-1 < pos)){
		total+=100;
		/* where 10 is the max label length. should really use a macro. */
		INC_ARRAY(mem.waiting_label, total, sizeof *mem.waiting_label);
		mem.waiting_size = total;
	}
	INC_ARRAY(*(mem.waiting_label+pos), 10, 1);
	strcpy(*(mem.waiting_label+pos), label);
	mem.waiting_count++;
	EPRINTF("Unknown label is: %s\n", label);
}


void set_label(char *label){
	size_t pos = mem.lab_count;
	size_t total = mem.lab_size;
	if((total == 0) || (total-1 < pos)){
		total+=100;
		INC_ARRAY(mem.lab, total, sizeof *mem.lab);
		mem.lab_size = total;
	}
	struct label l;
	l.addr = mem.ins_count;
	strcpy(l.name, label);
	*(mem.lab+pos) = l;
	mem.lab_count++;

	if(halt_exec){
		if(!strcmp(label, *(mem.waiting_label + wait_pos)))
			EPRINTF("label %s found, continuing execution.\n", label);
			halt_exec = 0;
	}

}

int find_label(char *label, size_t *var){
	for(size_t i=0; i<mem.lab_count ; i++){
		if(!strcmp(label, ((mem.lab+i)->name))){
			/* *var = i; */
			*var = (mem.lab+i)->addr;
			return 1;
		}
	}
	return 0;
}

void set_ins_mode(struct instruction *ins, char **insv, int len){
	EPRINTF("setting ins_mode.\n");
	if(len>1){
		int l = strlen(*insv) - 1;
		if(*(*insv + l) == ':'){
			*(*insv+l) = '\0';
			set_label(*insv);
			len--;
			insv++;
		}
	}
	EPRINTF("checked for labels.\n");
	for(int i=0;i<ARRAY_SIZE(inst) ; i++){
		if(!strcasecmp(*insv, inst[i].name)){
			ins->faddr=i;
			if(set_args(ins, i, insv, len))
				return;
			break;
		}
	}

	EPRINTF("Invalid inst: %s, len=%d\n", *insv, len);
	exit(EXIT_FAILURE);
}

#define INCOMPLETE_INST(name) do {		    \
	EPRINTF("Incomplete instruction: %s\n", name); \
	return 0; \
	} while(0)

#define REG_PAIR_P(pair) do{			\
		switch(pair){		\
		case 'b':			\
		case 'h':			\
		case 'd':			\
			break;						\
		default:						\
			printf("inst must be either b, h or d. got: %c\n", lowered); \
			INCOMPLETE_INST(*insv);				\
		}							\
	} while(0)
#define CHECK_LEN(leng) do{\
	if(leng != len) \
		INCOMPLETE_INST(*insv);\
	} while(0)

int set_args(struct instruction *ins, int i, char **insv, int len){
	switch(inst[i].type){
	case REG_REG:
		CHECK_LEN(3);
		ins->arg1 = **(insv + 1);
		ins->arg2 = **(insv + 2);
		break;
	case REG:
		CHECK_LEN(2);
		ins->arg1 = **(insv + 1);
		break;
	case REG_VAL:
		CHECK_LEN(3);
		ins->arg1 = **(insv + 1);
		ins->arg2 = strtol(*(insv+2),NULL, 16);
		break;
	case VAL:
		CHECK_LEN(2);
		ins->arg1 = strtol(*(insv+1), NULL, 16);
		break;
	case VAL_VAL:
		CHECK_LEN(3);
		ins->arg1 = strtol(*(insv+1), NULL, 16);
		ins->arg2 = strtol(*(insv+2), NULL, 16);
		break;
	case LABEL:
		CHECK_LEN(2);
		size_t tmp=0;
		if(find_label(*(insv+1), &tmp)){
			ins->arg1 = tmp;
			ins->arg2 = 1;
		}else{
			ins->arg2 = 0;
			set_waiting(*(insv+1));
			ins->arg3 = mem.waiting_count-1;
		}
		break;
	case PAIR_VAL:
		CHECK_LEN(3);
		strtopair(ins, *(insv+2));
		__attribute__((fallthrough)); /* use gcc or remove this. */
	case PAIR:
		if(inst[i].type == PAIR)
			CHECK_LEN(2);
		char lowered = tolower(**(insv+1));
		REG_PAIR_P(lowered);
		ins->arg1 = lowered;
		break;
	case NONE:
		if(len != 1 )
			INCOMPLETE_INST(*insv);
		break;
	default:
		EPRINTF("should be impossible.");
	}
	return 1;
}

void strtopair(struct instruction *i, char *str){
	long long val = strtol(str, NULL, 16);
	i->arg3 = val;
	i->arg2 = val >> 8;
}

int set_ins(struct instruction *i){
	int ret=1;
	char buf[20];
	if(!fgets(buf, 20, stdin)){
		perror("freads");
		return 0;
	}
	int len=4;
	int orig_len = len;
	char **ins = NULL;
	INC_ARRAY(ins, len, sizeof *ins);

	for(int j=0;j<len;j++){
		*(ins+j) = NULL;
		INC_ARRAY(*(ins+j), 10, sizeof **ins);
	}
	if(sscanf(buf, "%s %s %s %s", *ins, *(ins+1), *(ins+2), *(ins+3)) == len)
		goto end;
	else if(sscanf(buf, "%s %s %s", *ins, *(ins+1), *(ins+2)) == --len)
		goto end;
	else if(sscanf(buf, "%s %s", *ins, *(ins+1)) == --len)
		goto end;
	else if(sscanf(buf, "%s", *ins) == --len)
		goto end;
	else{
		EPRINTF("Can't parse the instruction.");
		ret = 0;
	}
 end:
	/* EPRINTF("got:\n"); */
	/* for(int j=orig_len;j>0;j--){ */
	/* 	printf("len = %d, arg = %s\n", j, *(ins+j-1)); */
	/* } */
	set_ins_mode(i, ins, len);
	for(int j=0;j<orig_len;j++){
		free(*(ins+j));
		*(ins+j) = NULL;
	}
	free(ins);
	ins = NULL;
	if(ret)
		add_ins_mem(i);
	return ret;
}

void add_ins_mem(struct instruction *ins){
	size_t count = mem.ins_size;
	size_t pos = mem.ins_count;
	if((pos == 0) || (pos+1 > count)){
		count+=100;
		INC_ARRAY(mem.ins, count, sizeof *mem.ins);
		mem.ins_size = count;
	}
	*(mem.ins+pos) = *ins;
	mem.ins_count++;
}
void change_exec(struct registers *r){
	size_t pos = jmp_pos;
	if(pos >= mem.ins_count)
		return;
	if(!halt_exec){
		struct instruction *ins = mem.ins+pos;
		int i = ins->faddr;
		if(inst[i].type == LABEL){
			if(ins->arg2 == 0){
				size_t j = ins->arg3;
				size_t tmp;
				if(find_label(*(mem.waiting_label+j), &tmp)){
					ins->arg1 = tmp;
					ins->arg2 = 1;
				}
			}
		}
		call_ins(ins, r);
	}
	jmp_pos++;
	change_exec(r);
}

uint8_t get_line_mem(uint16_t pos){
	FILE *fp = mem_fp;
	pos = pos*3;
	if(fseek(fp, pos, SEEK_SET))
		perror("fseek");
	char buf[3] = { 0 } ;
	if(fread(buf, sizeof *buf, 3, fp) != 3){
		EPRINTF("fread read less than 3 bytes\n"
			"position: %d (%x h)\n", pos, pos);
	}
	if(buf[2] != '\n'){
		EPRINTF("Expected \\n but got %c at %d (%x h) pos\n", buf[2], pos, pos);
		exit(0);
	}
	buf[2] = '\0';
	return strtol(buf, NULL, 16);
}

void set_line_mem(uint16_t pos, uint8_t data){
	pos *= 3;
	if(fseek(mem_fp, pos, SEEK_SET))
		perror("fseek");
	fprintf(mem_fp, "%2x", data);
	fflush(mem_fp);
}

uint16_t hl_to_pos(struct registers *r){
	EPRINTF("converting hl to position. h=%x, l=%x\n", r->h, r->l);
	uint16_t pos = 0;
	pos = r->h;
	EPRINTF("pos = %x\n", pos);
	pos <<=8;
	EPRINTF("pos = %x\n", pos);
	pos |= r->l;
	EPRINTF("pos = %x\n", pos);
	return pos;
}

uint8_t get_line_hl(struct registers *r){
	return get_line_mem(hl_to_pos(r));
}

void set_line_hl(struct registers *r, uint8_t data){
	set_line_mem(hl_to_pos(r), data);
}
