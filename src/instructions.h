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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


void rrc(struct registers *r, struct instruction *ins);
void nop();
void hlt();
void xchg(struct registers *r, struct instruction *ins);
void inr(struct registers *r, struct instruction *ins);
void dcr(struct registers *r, struct instruction *ins);
void jmp(struct registers *r, struct instruction *ins);
void jnc(struct registers *r, struct instruction *ins);
void jc(struct registers *r, struct instruction *ins);
void jz(struct registers *r, struct instruction *ins);
void jnz(struct registers *r, struct instruction *ins);

void mvi(struct registers *r, struct instruction *ins);
void adi(struct registers *r, struct instruction *ins);
void lxi(struct registers *r, struct instruction *ins);
void inx(struct registers *r, struct instruction *ins);
void dcx(struct registers *r, struct instruction *ins);

void mov(struct registers *r, struct instruction *ins);
void add(struct registers *r, struct instruction *ins);
void lda(struct registers *r, struct instruction *ins);
void ldax(struct registers *r, struct instruction *ins);
void lhld(struct registers *r, struct instruction *ins);
void sta(struct registers *r, struct instruction *ins);
void stax(struct registers *r, struct instruction *ins);
void shld(struct registers *r, struct instruction *ins);
void cpi(struct registers *r, struct instruction *ins);
void cmp(struct registers *r, struct instruction *ins);
#endif
