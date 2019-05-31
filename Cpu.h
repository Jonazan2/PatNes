#pragma once

#include "Types.h"


class Memory;

class Cpu
{
public:

    enum class Flags : byte
    {
        Negative            = 0b1000'0000,
        Overflow            = 0b0100'0000,
        /* bit 0b0010'0000 in unused and is always set to 1 in the P register*/
        Break               = 0b0001'0000,
        DecimalMode         = 0b0000'1000,
        InterruptDisable    = 0b0000'0100,
        Zero                = 0b0000'0010,
        Carry               = 0b0000'0001,

    };


    Cpu( Memory *memory );

    void Reset();

    word Update();

    bool IsFlagSet( Flags flag ) const;
    void RaiseFlag( Flags flag );
    void ToggleFlag( Flags flag );
    void ClearFlag( Flags flag );

private:

    using InstructionFunctionPtr = short ( Cpu::* )(); 
    using MappableInstructionFunctionPtr = short ( Cpu::* )( word ); 

    /* Registers */
    Register    PC;
    byte        stackPointer;
    byte        pRegister;
    byte        accumulator;
    byte        xRegisterIndex;
    byte        yRegisterIndex;

    /* Systems */
    Memory      *memory;

    /* Stack operations */
    word GetAbsoluteStackAddress() const;

    /* Opcode handling */
    byte GetNextOpcode();
    short ExecuteInstruction( byte opcode );
    short ExecuteSubroutineOrInterruptInstruction( byte opcode );
    short ExecuteBranchInstruction( byte opcode );
    short ExecuteMappableInstruction( byte opcode );
    short ExecuteInstructionCC00( byte opcode );
    short ExecuteInstructionCC01( byte opcode );
    short ExecuteInstructionCC10( byte opcode );
    short ExecuteSingleByteInstruction( byte opcode );

    /* Addressing mode handling */
    word GetImmediateAddress();
    word GetZeroPageAddress();
    word GetZeroPageAddressX();
    word GetZeroPageAddressY();
    word GetAbsoluteAddress();
    word GetAbsoluteAddressX();
    word GetAbsoluteAddressY();
    word GetIndexedAddressX();
    word GetIndexedAddressY();

    /* Mappable instructions under the pattern aaabbbcc */
    short ORA( word address );
    short AND( word address );
    short EOR( word address );
    short ADC( word address );
    short STA( word address );
    short LDA( word address );
    short CMP( word address );
    short SBC( word address );

    short ASL( word address );
    short ROL( word address );
    short LSR( word address );
    short ROR( word address );
    short STX( word address );
    short LDX( word address );
    short DEC( word address );
    short INC( word address );
    short ASLA();
    short ROLA();
    short LSRA();
    short RORA();

    short BIT( word address );
    short JMP( word address );
    short JMPA( word address );
    short STY( word address );
    short LDY( word address );
    short CPY( word address );
    short CPX( word address );

    /* Interrupt and subroutine instructions */
    short BRK();
    short JSR();
    short RTI();
    short RTS();

    /* Individual instructions */

    short PHP();
    short PLP();
    short PHA();
    short PLA();

    short DEY();
    short DEX();
    short INY();
    short INX();

    short CLC();
    short SEC();
    short CLI();
    short SEI();

    short CLV();
    short CLD();
    short SED();

    short TAY();
    short TYA();
    short TXA();
    short TXS();
    short TAX();
    short TSX();

    short NOP();
};
