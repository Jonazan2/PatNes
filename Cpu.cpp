#pragma once

#include "Cpu.h"

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
    const byte instruction = ( ( ( opcode & 0b1110'0000 ) >> 3 ) | ( opcode & 0b0000'0011 ) );
    const byte addressingMode = ( opcode & 0b0001'1100 ) >> 2;
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

byte Cpu::GetNextOpcode() const
{
    return memory->Read( PC.value );
}