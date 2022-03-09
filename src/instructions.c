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

#include "glue.h"
#include "instructions.h"

/* implied addressing modes */

void nop(){}

void rrc(struct registers *r){
	int overflow = r->a & 1;
	r->a>>= 1;
	if(overflow)
		/* r->a |=  0b10000000; */
		r-> a |= ( 1 << 7 ) ;
}

void inr(struct registers *r){
	r->a++;
}

void dcr(struct registers *r){
	r->a--;
}

/* immediate addressing modes */

void mvi(struct registers *r, char regi, uint8_t val){
	set_register(r, regi, val);
}


/* register addressing modes */

void mov(struct registers *r, char dest, char source){
	set_register(r, dest, get_register(r, source));
}

