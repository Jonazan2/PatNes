#pragma once

#include "Cpu.h"

#include <array>

#include "Memory.h"

Cpu::Cpu( Memory *memory )
    : memory( memory )
{
    Reset();
}

void Cpu::Reset()
{
    /* Load the reset vector into the PC register */
    PC.low = memory->Read( 0xFFFC );
    PC.hi = memory->Read( 0xFFFD );

    /* Power up state of registers */
    stackPointer = 0xFD;
    pRegister = 0x34;
    accumulator = 0x00;
    xRegisterIndex = 0x00;
    yRegisterIndex = 0x00;
}

word Cpu::Update()
{
    const byte opcode = GetNextOpcode();
    ExecuteInstruction( opcode );
   
    return 1;
}

short Cpu::ExecuteInstruction( byte opcode )
{
    /* Check if it is a branch instruction with form 0bxxy1'0000 */
    static constexpr byte BRANCH_OPERATION_MASK = 0b0001'0000;
    if ( ( opcode & 0b0001'1111 ) == BRANCH_OPERATION_MASK )
    {
        return ExecuteBranchInstruction( opcode );
    }
    
    /* Check if it is a regular aaabbbcc instruction */    
    const byte instructionMask = opcode & 0b0000'0011;
    if ( instructionMask != 0b0000'0011 )
    {
        return ExecuteMappableInstruction( opcode );
    }

    /* The instruction can't be mapped, it must be a single byte instruction */
    return ExecuteSingleByteInstruction( opcode );
}

short Cpu::ExecuteBranchInstruction( byte opcode )
{
    static constexpr std::array< Cpu::Flags, 4> OPERAND_TO_FLAG = 
    {
        Cpu::Flags::Negative,
        Cpu::Flags::Overflow,
        Cpu::Flags::Carry,
        Cpu::Flags::Zero
    };
    
    const byte firstOperand = (opcode & 0b1100'0000) >> 6;
    const byte secondOperand = (opcode & 0b0010'0000) >> 5;

    const bool shouldBranch = IsFlagSet( OPERAND_TO_FLAG[ firstOperand ] ) == static_cast< bool >( secondOperand );
    const byte relativeDisplacement = memory->Read( PC.value );
    ++PC.value;

    if ( shouldBranch )
    {
        PC.value += relativeDisplacement;
    }

    /* TODO (Jonathan): Implement proper cycle timing */
    return 0; 
}

short Cpu::ExecuteMappableInstruction( byte opcode )
{
    const byte instruction = ( ( ( opcode & 0b1110'0000 ) >> 3 ) | ( opcode & 0b0000'0011 ) );
    const byte addressingMode = ( opcode & 0b0001'1100 ) >> 2;

    /* TODO (Jonathan): Implement proper cycle timing */
    return 0; 
}

short Cpu::ExecuteSingleByteInstruction( byte opcode )
{

    /* TODO (Jonathan): Implement proper cycle timing */
    return 0;
}

bool Cpu::IsFlagSet( Flags flag ) const
{
    return pRegister & static_cast< byte >( flag );
}

void Cpu::RaiseFlag(Flags flag)
{
    pRegister = pRegister | static_cast< byte >( flag );
}

void Cpu::ToggleFlag( Flags flag )
{

    pRegister = pRegister ^ static_cast< byte >( flag );
}

word Cpu::GetAbsoluteStackAddress() const
{
    return 0x0100 + stackPointer;
}

byte Cpu::GetNextOpcode()
{
    return memory->Read( PC.value++ );
}