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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
#include <assert.h>
#include "decode.h"
int number_set_bits_in(uint16_t n) {
	int count = 0;
	int mask = 1;
	for (int i = 0; i < 16; i++)
	{
		if ((mask & n) == mask)
			count++;
		mask = mask << 1;
	}
	return count;
}

void decode_instruction(uint32_t ins, uint8_t* cond, uint8_t* I_bit, uint8_t* P_bit, uint8_t* U_bit, uint8_t* B_bit, uint8_t* W_bit, uint8_t* L_bit, uint8_t* rn, uint8_t* rd, uint16_t* data) {
    *cond = get_bits(ins, 31, 28);
    if (I_bit != NULL) {
        *I_bit = get_bit(ins, 25);
    }
    *P_bit = get_bit(ins, 24);
    *U_bit = get_bit(ins, 23);
    *B_bit = get_bit(ins, 22);
    *W_bit = get_bit(ins, 21);
    *L_bit = get_bit(ins, 20);
    *rn = (uint8_t)get_bits(ins, 19, 16);
    if (rd != NULL) {
        *rd = (uint8_t)get_bits(ins, 15, 12);
        *data = (uint16_t)get_bits(ins, 11, 0);
    } else {
        *data = get_bits(ins, 15, 0);
    }
}

int arm_load_store(arm_core p, uint32_t ins) {
    int result;
    if (get_bits(ins, 27,26) == 0b01) {
        //Load and store word or unsigned byte instructions
        result = word_byte_load_store(p, ins);
    } else if (get_bits(ins, 27, 25) == 0b000 && get_bit(ins,7) == 1 && get_bit(ins, 4) == 1) {
        //Load and store halfword or doubleword, and load signed byte instructions
        result = miscellaneous_load_store(p, ins);
    }
    return result;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    uint8_t cond, P_bit, U_bit, S_bit, W_bit, L_bit, Rn;
	  uint16_t register_list;

    decode_instruction(ins, &cond, NULL, &P_bit, &U_bit, &S_bit, &W_bit, &L_bit, &Rn, NULL, &register_list);

	uint32_t cpsr;
	if (L_bit == 1 && Rn == 15) { // Si c'est un load de PC le bit S indique si CPSR vient de SPSR
		cpsr = arm_read_spsr(p);
    } else {
        cpsr = arm_read_cpsr(p);
    }

    uint32_t start_address,end_address;
    // Before
    if(P_bit) {
        // decrement before
        if (U_bit) {
            start_address = Rn - (number_set_bits_in(register_list) * 4);
            end_address = Rn - 4;
            if (condition(cpsr, cond) && W_bit == 1) {
                Rn = Rn - (number_set_bits_in(register_list) * 4);
            }
        // Increment before
        } else {
            start_address = Rn + 4;
            end_address = Rn + (number_set_bits_in(register_list) * 4);
            if (condition(cpsr, cond) && W_bit == 1) {
                Rn = Rn + (number_set_bits_in(register_list) * 4);
            }
        }

    // After
    } else {
        // decrement after
        if (U_bit) {
            start_address = Rn - (number_set_bits_in(register_list) * 4) + 4;
            end_address = Rn;
            if (condition(cpsr, cond) && W_bit == 1) {
                Rn = Rn - (number_set_bits_in(register_list) * 4);
            }
        // Increment after
        } else {
            start_address = Rn;
            end_address = Rn + (number_set_bits_in(register_list) * 4) - 4;
            if (condition(cpsr, cond) && W_bit == 1) {
                Rn = Rn + (number_set_bits_in(register_list) * 4);
            }
        }
    }

    
    uint32_t address;
    uint32_t value; // Pour LDM
    // LDM Page A4-36 Doc
    if(L_bit) {
        debug("LDM :");
        if (condition(cpsr, cond)) {
            address = start_address;
            for (int i = 0; i <= 14; i++) {
                if (get_bit(register_list, i) == 1) {
                    arm_read_word(p, address, &value);
                    if (S_bit && get_bit(register_list, 15) == 0 && W_bit == 0) {
                        // LDM (2)
                        arm_write_usr_register(p, i, value);
                    } else {
                        // LDM (1) et (3)
                        arm_write_register(p, i, value);
                    }
                    address = address + 4;
                }
            }
            // LDM (3)
            if (S_bit && get_bit(register_list, 15) == 1) {
                if (arm_current_mode_has_spsr(p)) {
                    cpsr = arm_read_spsr(p);
                } else {
                    return UNDEFINED_INSTRUCTION;
                }
                arm_read_word(p, address, &value);
                arm_write_register(p, 15, value); // PC = value
                address = address + 4;
            // LDM (1) et (2)
            } else if (get_bit(register_list, 15) == 1) {
                value = arm_read_word(p, address, &value);
                arm_write_register(p, 15, value & 0xFFFFFFFE);
                //T Bit = value[0]; 
                address = address + 4;
            }

            assert(end_address == (address - 4));
        }

        
    // STM Page A4-189 Doc
    } else {
        debug("STM :");
        if (condition(cpsr, cond)) {
            address = start_address;
            for (int i = 0; i <= 15; i++) {
                    if (get_bit(register_list, i) == 1) {
                        if (!S_bit && !L_bit) {
                        // STM (1)
                        value = arm_read_register(p, i);
                        } else {
                        // STM (2)
                        value = arm_read_usr_register(p, i);
                        }
                    arm_write_word(p, address, value);
                    address = address + 4;
                }
            }
            assert(end_address == (address - 4));
        }
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

uint32_t handling_offset(uint32_t address, int offset,uint8_t U) {
	if(U){
		return address + offset;
	} else {
		return address - offset;
	}
}

//Call write_register associate at W_bit
//(W_bit == 0) normal memory acces
//(W_bit == 1) unprivileged (User mode) memory access 
void write_register_mode(arm_core p, uint8_t reg, uint32_t value, uint8_t W_bit) {
    if (W_bit == 0) {
        arm_write_register(p, reg, value);
    } else {
        arm_write_usr_register(p, reg, value);
    }
}

//Call read_register associate at W_bit
//(W_bit == 0) normal memory acces
//(W_bit == 1) unprivileged (User mode) memory access 
uint32_t read_register_mode(arm_core p, uint8_t reg, uint8_t W_bit) {
    if (W_bit == 0) {
        return arm_read_register(p, reg);
    } else {
        return arm_read_usr_register(p, reg);
    }
}

/**
 *  arm_core p
 *  P_bit  : Use of post-indexed addressing or offset addressing.
 *  U_bit : Indicates whether the offset is added to the base (U == 1) or is subtracted from the base (U == 0)
 *  W_bit : -> P == 0, normal memory access is performed (W ==0) 
 *              or an unprivileged (User mode) memory access (W ==1)
 *          -> p == 1  the base register is not updated (W == 0)
 *              or the calculated memory address is written back to the base register (W == 1)
 *  effet de bord
 *  L_bit : Distinguishes between a Load (L==1) and a Store instruction (L==0)
 *  B_bit : Distinguishes between an unsigned byte (B==1) and a word (B==0) access
 *  data : bit 0 at 11 
**/ 
int word_byte_load_store(arm_core p, uint32_t instruction) {
    uint8_t cond, I_bit, P_bit, U_bit, B_bit, W_bit, L_bit, rn, rd;
    uint16_t data;
    decode_instruction(instruction, &cond, &I_bit, &P_bit, &U_bit, &B_bit, &W_bit, &L_bit, &rn, &rd, &data);
     
    uint32_t address;
    uint16_t offset;
    int error = 0;
    int val_rn = arm_read_register(p, rn);
    if (I_bit != 0) {
        //register offset or scaled register offset
        uint8_t rm =  get_bits(data, 3, 0);
        offset = arm_read_register(p, rm);
        if (get_bits(data, 11, 4) != 0) {
            //scaled register
            offset = shift(p, instruction);
        }
    } else {
        //immediate offset
        offset = data;
    }
    if (P_bit == 0) {
        //post-indexed
        address = val_rn;
        if (condition(arm_read_cpsr(p), cond)) {
            val_rn = handling_offset(val_rn, offset, U_bit);
            arm_write_register(p, rn, val_rn);	
        }
        if (L_bit == 0) {
            //Store instruction
            if (B_bit == 0) {
                //str
                debug("STR rd: r%d, addr: %x\n", rd, address);
                uint32_t value;
                value = read_register_mode(p, rd, W_bit);
                error = arm_write_word(p, address, value);
            } else {
                //strb
                debug("STRB rd: r%d, addr: %x\n", rd, address);
                uint32_t value = read_register_mode(p, rd, W_bit);
                error = arm_write_byte(p, address, (uint8_t)value);
            }
        } else {
            //Load instruction
            if (B_bit == 0) {
                //ldr
                debug("LDR rd: r%d, addr: %x\n", rd, address);
                uint32_t value;
                error = arm_read_word(p, address, &value);
                if (error == NO_EXCEPTION) {
                    write_register_mode(p, rd, value, W_bit);
                }
            } else {
                //ldrb
                debug("LDRB rd: r%d, addr: %x\n", rd, address);
                uint8_t value;
                error = arm_read_byte(p, address, &value);
                if (error == NO_EXCEPTION) {
                    write_register_mode(p, rd, (uint32_t)value, W_bit);
                }
            }
        }
    } else {
        //offset adressing or Pre-adrressing 
        address = handling_offset(val_rn, offset, U_bit);
        if (W_bit == 1 && condition(arm_read_cpsr(p), cond)) {
            //pre-addressing address is written back
            arm_write_register(p ,rn ,address);
        } //else base register is not updated
        if (L_bit == 0) {
            //Store instruction
            if (B_bit == 0) {
                //str
                debug("STR rd: r%d, addr: %x\n", rd, address);
                uint32_t value = arm_read_register(p, rd);
                error = arm_write_word(p, address, value);
            } else {
                //strb
                debug("STRB rd: r%d, addr: %x\n", rd, address);
                uint32_t value = arm_read_register(p, rd);
                error = arm_write_byte(p, address, ((uint8_t)value));
            }
        } else {
            //Load instruction
            if (B_bit == 0) {
                //ldr
                debug("LDR rd: r%d, addr: %x\n", rd, address);
                uint32_t value;
                error = arm_read_word(p, address, &value);
                if (error == NO_EXCEPTION) {
                    arm_write_register(p, rd, value);
                }
            } else {
                //ldrb
                debug("LDRB rd: r%d, addr: %x\n", rd, address);
                uint8_t value;
                error = arm_read_byte(p, address, &value);
                if (error == NO_EXCEPTION) {
                    arm_write_register(p, rd, (uint32_t)value);
                }
            }
        }
    }
    return error;
}

int miscellaneous_load_store(arm_core p, uint32_t ins) {
    //Constante
    uint8_t H_bit = get_bit(ins, 5);
    uint8_t S_bit = get_bit(ins, 6);

    uint8_t cond, I_bit, P_bit, U_bit, B_bit, W_bit, L_bit, rn, rd;
    uint16_t data;
    decode_instruction(ins, &cond, &I_bit, &P_bit, &U_bit, &B_bit, &W_bit, &L_bit, &rn, &rd, &data);

    //Variable
    uint16_t offset;
    uint32_t address;
    int val_rn = arm_read_register(p, rn);
    int error = NO_EXCEPTION;

    if (I_bit == 0) {
        offset = ((get_bits(data, 11, 8) << 4) | get_bits(data, 3, 0));
    } else {
        uint8_t rm = get_bits(data, 3, 0);
        offset = arm_read_register(p, rm);
    }
    if (P_bit == 0) {
        //post indexed adressing
        if (W_bit == 1) {
            address = val_rn;
            if (condition(arm_read_cpsr(p), cond)) {
                val_rn = handling_offset(val_rn, offset, U_bit);
                arm_write_register(p, rn, val_rn);
            }
            // LDRH, STRH, LDRSH, STRSH, LDRSB, STRSB, LDRD, STRD
            error = load_store_half_double_byte(p, rd, address, L_bit, S_bit, H_bit, cond);
        }
        else {
            //if P ==0 and W == 0 then the instruction is UNPREDICTABLE.
            error = UNDEFINED_INSTRUCTION;
        }
    } else {
        //offset adressing or Pre-adrressing 
        address = handling_offset(val_rn, offset, U_bit);
        if (W_bit == 1 && condition(arm_read_cpsr(p), cond)) {
            //pre-addressing address is written back
            error = arm_write_register(p, rn, address);
        } //else base register is not updated
        // LDRH, STRH, LDRSH, STRSH, LDRSB, STRSB, LDRD, STRD
        load_store_half_double_byte(p, rd, address, L_bit, S_bit, H_bit, cond);
    }
    return error;
}

// LDRH, STRH, LDRSH, STRSH, LDRSB, STRSB, LDRD, STRD
int load_store_half_double_byte (arm_core p, uint8_t rd, uint32_t address, uint8_t L_bit, uint8_t S_bit, uint8_t H_bit, uint8_t cond) {
    int error = NO_EXCEPTION;
    switch (((L_bit << 3) + (S_bit << 2) + H_bit)) { 
        case 0b001: //strh: Store halfword
            debug("STRH rd: r%d, addr: %x\n", rd, address);
            if (condition(arm_read_cpsr(p), cond)) {
                uint16_t value16 = (get_bits(arm_read_register(p, rd), 15, 0));
                error = arm_write_half(p, address, value16);
            }
            break;
        case 0b010: //ldrd: Load doubleword
            debug("LDRD rd: r%d, addr: %x\n", rd, address);
            if (condition(arm_read_cpsr(p), cond)) {
                if (rd != R14 && rd % 2 == 0 && get_bits(address, 1, 0) != 0b00) {
                    //read first word
                    uint32_t value32;
                    error = arm_read_word(p, address, &value32);
                    if (error == NO_EXCEPTION) {
                        //write first word in rd
                        arm_write_register(p, rd, value32);
                        //read second word
                        error = arm_read_word(p, address + 4, &value32);
                        if (error == NO_EXCEPTION) {
                            //write second word
                            arm_write_register(p, rd+1, value32);
                        }
                    }
                }
            } else {
                error = UNDEFINED_INSTRUCTION; 
            }
            break;
        case 0b011: //strd: Store doubleword
            debug("STRD rd: r%d, addr: %x\n", rd, address);
            if (rd % 2 == 0 && rd != R14 && get_bits(address, 1, 0) != 0b00 && get_bit(address, 2) == 0) {
                uint32_t value32 = arm_read_register(p, rd);
                error = arm_write_word(p, address, value32);
                if (error == NO_EXCEPTION) {
                    value32 = arm_read_register(p, rd + 1);
                    error = arm_write_word(p, address + 4, value32);
                }
            } else {
                error = UNDEFINED_INSTRUCTION;
            }
            break;
        case 0b101: //ldrh: Load unsigned halfword
            debug("LDRH rd: r%d, addr: %x\n", rd, address);
            //Pas sûr sûr
            uint16_t value16;
            error = arm_read_half(p, address, &value16);
            if (error == NO_EXCEPTION) {
                arm_write_register(p, rd, (uint32_t)value16);
            }
            break;
        case 0b110: //ldrsb: Load signed byte
            debug("LDRSB rd : r%d, addr: %x\n", rd, address);
            if (condition(arm_read_cpsr(p), cond)) {
                //Pas sûr sûr
                uint8_t value8;
                error = arm_read_byte(p, address, &value8);
                if (error == NO_EXCEPTION) {
                    if (value8 & (1 << 7)) {
                        value8 |= 0xFFFFFF00;
                    } else {
                        value8 &= 0X0000F00FF;
                    }
                    arm_write_register(p, rd, (uint32_t)value8);
                }
            }
            break;
        case 0b111: //ldrsh: Load signed halfword
            debug("LDRSH rd : r%d, addr: %x\n", rd, address);
            uint32_t value32;
            error = arm_read_half(p, address, (uint16_t*)&value32);
            if (error == NO_EXCEPTION) {
                if (value32 & (1 << 15)) {
                    value32 |= 0xFFFF0000; 
                } else {
                    value32 &= 0X0000FFFFF;
                } 
                arm_write_register(p, rd, value32);
            }
            break;
        default:
            break;
    }
    return error;
}