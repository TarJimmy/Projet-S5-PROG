/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>
#include <string.h>
struct registers_data {
    uint32_t regs[15];
    uint32_t cpsr;
    uint32_t spsr;
};

registers registers_create() {
    registers r = malloc(sizeof(struct registers_data));
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t get_mode(registers r) {
    //5 premiers bits du CPSR
    //0b11111
    uint64_t mode = (uint8_t)(r->cpsr & 0x0000001f);
    return mode;
} 

int current_mode_has_spsr(registers r) {
    uint8_t mode = get_mode(r);
    if (strcmp(arm_get_mode_name(mode),"IRQ") || strcmp(arm_get_mode_name(mode), "FIQ")){
        return 1;
    } else {
        return 0;
    }
}

int in_a_privileged_mode(registers r) {
    uint8_t mode = get_mode(r);
    return !strcmp(arm_get_mode_name(mode), "USR");
}

uint32_t read_register(registers r, uint8_t reg) {
    uint32_t value = r->regs[reg];
    return value;
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    uint32_t value=0;
    if (!in_a_privileged_mode(r)) {
        value = read_register(r, reg);
    }
    return value;
}

uint32_t read_cpsr(registers r) {
    uint32_t value = r->cpsr;
    return value;
}

uint32_t read_spsr(registers r) {
    uint32_t value = 0;
    if (current_mode_has_spsr(r)) {
        value = r->spsr;
    }
    return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    r->regs[reg] = value;
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if (!in_a_privileged_mode(r)) {
        write_register(r, reg, value);
    }
}

void write_cpsr(registers r, uint32_t value) {
    r->cpsr = value;
}

void write_spsr(registers r, uint32_t value) {
    if (current_mode_has_spsr(r)) {
        r->spsr = value;
    }
}
