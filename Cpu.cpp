#pragma once

#include "Cpu.h"

#include <assert.h>
#include <array>
#include <unordered_map>

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
    const byte mappableInstructionMask = opcode & 0b0000'0011;
    if ( mappableInstructionMask != 0b0000'0011 )
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
    static const std::unordered_map< byte, InstructionFunctionPtr > INSTRUCTION_MAP =
    {
        { 0x08, &Cpu::PHP },
        { 0x18, &Cpu::CLC },
        { 0x28, &Cpu::PLP },
        { 0x38, &Cpu::SEC },

        { 0x48, &Cpu::PHA },
        { 0x58, &Cpu::CLI },
        { 0x68, &Cpu::PLA },
        { 0x78, &Cpu::SEI },

        { 0x88, &Cpu::DEY },
        { 0x8A, &Cpu::TXA },
        { 0x98, &Cpu::TYA },
        { 0x9A, &Cpu::TXS },

        { 0xA8, &Cpu::TAY },
        { 0xAA, &Cpu::TAX },
        { 0xB8, &Cpu::CLV },
        { 0xBA, &Cpu::TSX },

        { 0xC8, &Cpu::INY },
        { 0xCA, &Cpu::DEX },
        { 0xD8, &Cpu::CLD },
        { 0xE8, &Cpu::INX },

        { 0xEA, &Cpu::NOP },
    };

    const InstructionFunctionPtr instruction = INSTRUCTION_MAP.at( opcode );
    return ( this->*instruction )();
}

byte Cpu::GetNextOpcode()
{
    return memory->Read( PC.value++ );
}


/* ------------------- FLAGS -------------------*/

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


/* ------------------- INSTRUCTIONS -------------------*/

short Cpu::PHP() 
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::PLP()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::PHA()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::PLA()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::DEY()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::DEX()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::INY()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::INX()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::CLC()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::SEC()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::CLI()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::SEI()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::CLV()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::CLD()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::SED()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TAY()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TYA()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TXA()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TXS()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TAX()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::TSX()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::NOP()
{ 
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}