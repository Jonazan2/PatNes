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
    return ExecuteInstruction( opcode );
}

short Cpu::ExecuteInstruction( byte opcode )
{
    /* Handle subroutine and interrupt instructions */
    if ( opcode == 0x00 && opcode == 0x20 && opcode == 0x40 && opcode == 0x60 )
    {
        return ExecuteSubroutineOrInterruptInstruction( opcode );
    }

    /* Check if it is a branch instruction with form 0bxxy1'0000 */
    static constexpr byte BRANCH_OPERATION_MASK = 0b0001'0000;
    if ( ( opcode & 0b0001'1111 ) == BRANCH_OPERATION_MASK )
    {
        return ExecuteBranchInstruction( opcode );
    }

    /* Check if the instruction can't be mapped so it must be a single byte instruction */
    const byte opcodeLowerNibble = opcode & 0b0000'1111;    
    if ( opcodeLowerNibble == 0x08 || opcodeLowerNibble == 0x0A )
    {
        return ExecuteSingleByteInstruction( opcode );  
    }

    /* It is a regular aaabbbcc instruction */
    return ExecuteMappableInstruction( opcode );
}

short Cpu::ExecuteSubroutineOrInterruptInstruction( byte opcode )
{
    static const std::unordered_map< byte, InstructionFunctionPtr > INSTRUCTION_MAP =
    {
        { 0x00, &Cpu::BRK },
        { 0x20, &Cpu::JSR },
        { 0x40, &Cpu::RTI },
        { 0x60, &Cpu::RTS },
    };

    const InstructionFunctionPtr instruction = INSTRUCTION_MAP.at( opcode );
    return ( this->*instruction )();
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
    /* These instructions follow the aaabbbcc pattern where 'aaa/cc' specifies the instruction and 'bbb' the address mode */
    switch ( opcode & 0b0000'0011 )
    {
        case 0b0000'0000: { return ExecuteInstructionCC00( opcode ); }
        case 0b0000'0001: { return ExecuteInstructionCC01( opcode ); }
        case 0b0000'0010: { return ExecuteInstructionCC10( opcode ); }

        default:
        {
            assert( false && "How did I get here?. The show where instructions say...How did I get here?");
        }
    }
}

short Cpu::ExecuteInstructionCC00( byte opcode )
{
    /* We only need the 'aaa' part of the instruction to map it since we already know that cc == 01 */
    static const std::unordered_map< byte, MappableInstructionFunctionPtr > INSTRUCTION_MAP =
    {
        { 0x01, &Cpu::BIT },
        { 0x02, &Cpu::JMP },
        { 0x03, &Cpu::JMPA },

        { 0x04, &Cpu::STY },
        { 0x05, &Cpu::LDY },
        { 0x06, &Cpu::CPY },
        { 0x07, &Cpu::CPX }
    };


    word address;
    const byte addressingMode = ( opcode & 0b0001'1100 ) >> 2;
    switch ( addressingMode )
    {
        case 0b0000'0000: { address = GetImmediateAddress();    break; }
        case 0b0000'0001: { address = GetZeroPageAddress();     break; }
        case 0b0000'0011: { address = GetAbsoluteAddress();     break; }
        case 0b0000'0101: { address = GetZeroPageAddressX();    break; }
        case 0b0000'0111: { address = GetAbsoluteAddressX();    break; }
    }


    const byte instruction = ( ( opcode & 0b1110'0000 ) >> 5 );
    const MappableInstructionFunctionPtr ptr = INSTRUCTION_MAP.at( instruction );
    return ( this->*ptr )( address );
}

short Cpu::ExecuteInstructionCC01( byte opcode )
{
    /* We only need the 'aaa' part of the instruction to map it since we already know that cc == 01 */
    static const std::unordered_map< byte, MappableInstructionFunctionPtr > INSTRUCTION_MAP =
    {
        { 0x00, &Cpu::ORA },
        { 0x01, &Cpu::AND },
        { 0x02, &Cpu::EOR },
        { 0x03, &Cpu::ADC },

        { 0x04, &Cpu::STA },
        { 0x05, &Cpu::LDA },
        { 0x06, &Cpu::CMP },
        { 0x07, &Cpu::SBC }
    };


    word address;
    const byte addressingMode = ( opcode & 0b0001'1100 ) >> 2;
    switch ( addressingMode )
    {
        case 0b0000'0000: { address = GetIndexedAddressX();     break; }
        case 0b0000'0001: { address = GetZeroPageAddress();     break; }
        case 0b0000'0010: { address = GetImmediateAddress();    break; }
        case 0b0000'0011: { address = GetAbsoluteAddress();     break; }
        case 0b0000'0100: { address = GetIndexedAddressY();     break; }
        case 0b0000'0101: { address = GetZeroPageAddressX();    break; }
        case 0b0000'0110: { address = GetAbsoluteAddressY();    break; }
        case 0b0000'0111: { address = GetAbsoluteAddressX();    break; }
    }

    const byte instruction = ( ( opcode & 0b1110'0000 ) >> 5 );
    const MappableInstructionFunctionPtr ptr = INSTRUCTION_MAP.at( instruction );
    return ( this->*ptr )( address );
}

short Cpu::ExecuteInstructionCC10( byte opcode )
{
    static const std::unordered_map< byte, MappableInstructionFunctionPtr > INSTRUCTION_MAP =
    {
        { 0x00, &Cpu::ASL },
        { 0x01, &Cpu::ROL },
        { 0x02, &Cpu::LSR },
        { 0x03, &Cpu::ROR },

        { 0x04, &Cpu::STX },
        { 0x05, &Cpu::LDX },
        { 0x06, &Cpu::DEC },
        { 0x07, &Cpu::INC }
    };

    static const std::unordered_map< byte, InstructionFunctionPtr > ACCUMULATOR_MODE_INSTRUCTION_MAP =
    {
        { 0x00, &Cpu::ASLA },
        { 0x01, &Cpu::ROLA },
        { 0x02, &Cpu::LSRA },
        { 0x03, &Cpu::RORA },
    };


    const byte instruction = ( ( opcode & 0b1110'0000 ) >> 5 );
    const byte addressingMode = ( opcode & 0b0001'1100 ) >> 2;
    if ( addressingMode == 0b0000'0010 )
    {
        /* we handle the accumulator mode with the map directly by having separated instructions */
        const InstructionFunctionPtr ptr = ACCUMULATOR_MODE_INSTRUCTION_MAP.at( instruction );
        return ( this->*ptr )();
    }
    else
    {
        word address;
        switch ( addressingMode )
        {
            case 0b0000'0000: { address = GetImmediateAddress();    break; }
            case 0b0000'0001: { address = GetZeroPageAddress();     break; }
            case 0b0000'0011: { address = GetAbsoluteAddress();     break; }

            case 0b0000'0110:
            { 
                if ( instruction == 0x04 || instruction == 0x05 )
                {
                    address = GetZeroPageAddressY();
                }
                else
                {
                    address = GetZeroPageAddressX();
                }
                break;
            }
            case 0b0000'0111:
            {
                if ( instruction == 0x04 || instruction == 0x05 )
                {
                    address = GetAbsoluteAddressY();
                }
                else
                {
                    address = GetAbsoluteAddressX();
                }
                break;
            }
        }

        const MappableInstructionFunctionPtr ptr = INSTRUCTION_MAP.at( instruction );
        return ( this->*ptr )( address );
    }
}

short Cpu::ExecuteSingleByteInstruction( byte opcode )
{
    static const std::unordered_map< byte, InstructionFunctionPtr > INSTRUCTION_MAP =
    {
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


/* ------------------- ADDRESSING MODES -------------------*/


word Cpu::GetImmediateAddress()
{
    return PC.value++;
}

word Cpu::GetZeroPageAddress()
{
    const byte pageDisplacement = memory->Read( PC.value );
    ++PC.value;

    return static_cast< word >( pageDisplacement );
}

word Cpu::GetZeroPageAddressX()
{
    const byte pageDisplacement = memory->Read( PC.value );
    ++PC.value;

    return static_cast< word >( pageDisplacement + xRegisterIndex );
}

word Cpu::GetZeroPageAddressY()
{
    const byte pageDisplacement = memory->Read( PC.value );
    ++PC.value;

    return static_cast< word >( pageDisplacement + yRegisterIndex );
}

word Cpu::GetAbsoluteAddress()
{
    Register address;
    address.low = memory->Read( PC.value );
    ++PC.value;
    address.hi = memory->Read( PC.value );
    ++PC.value;

    return address.value;
}

word Cpu::GetAbsoluteAddressX()
{
    Register address;
    address.low = memory->Read( PC.value );
    ++PC.value;
    address.hi = memory->Read( PC.value );
    ++PC.value;

    return address.value + xRegisterIndex;
}

word Cpu::GetAbsoluteAddressY()
{
    Register address;
    address.low = memory->Read( PC.value );
    ++PC.value;
    address.hi = memory->Read( PC.value );
    ++PC.value;

    return address.value + yRegisterIndex;
}

word Cpu::GetIndexedAddressX()
{
    const byte indexDisplacement = memory->Read( PC.value );
    ++PC.value;
    
    const word totalDisplacement = ( ( indexDisplacement + xRegisterIndex ) & 0xFF );
    Register address;
    address.low = memory->Read( totalDisplacement );
    address.hi = memory->Read( totalDisplacement + 1 );

    return address.value;
}

word Cpu::GetIndexedAddressY()
{
    const byte indexDisplacement = memory->Read( PC.value );
    ++PC.value;

    Register address;
    address.low = memory->Read( indexDisplacement );
    address.hi = memory->Read( indexDisplacement + 1 );
    
    return address.value + yRegisterIndex;
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

void Cpu::ClearFlag( Flags flag )
{
    pRegister = pRegister & ~( static_cast< byte >( flag ) );
}

word Cpu::GetAbsoluteStackAddress() const
{
    return 0x0100 + stackPointer;
}


/* ------------------- MAPPABLE INSTRUCTIONS -------------------*/

short Cpu::ORA( word address )
{
    const byte data = memory->Read( address );
    accumulator |= data;

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( ( accumulator & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::AND( word address )
{
    const byte data = memory->Read( address );
    accumulator &= data;

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( ( accumulator & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::EOR( word address )
{
    const byte data = memory->Read( address );
    accumulator ^= data;

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( ( accumulator & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::ADC( word address )
{
    const byte data = memory->Read( address );
    const byte carryValue = IsFlagSet( Cpu::Flags::Carry ) ? 0x01 : 0x00; 
 
    if ( accumulator + data + carryValue > 0xFF )
    {
        RaiseFlag( Cpu::Flags::Carry );
    }
    else
    {
        ClearFlag( Cpu::Flags::Carry );
    }
 
    if ( ( accumulator & 0b1000'0000 ) !=  ( ( accumulator + data + carryValue ) & 0b1000'0000 ) )
    {
        RaiseFlag( Cpu::Flags::Overflow );
    }
    else
    {
        ClearFlag( Cpu::Flags::Overflow );
    }

    accumulator = accumulator + data + carryValue;

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( ( accumulator & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::STA( word address )
{
    memory->Write( address, accumulator );
    return 2;
}

short Cpu::LDA( word address )
{
    accumulator = memory->Read( address );

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( (accumulator & 0b1000'0000) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::CMP( word address )
{
    const byte data = memory->Read( address );

    ( accumulator >= data ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    ( accumulator == data ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero ) ;

    const byte substraction =  accumulator - data;
    ( ( substraction & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::SBC( word address )
{
    const byte data = memory->Read( address );
    const byte carryValue = 0x01 - ( IsFlagSet( Cpu::Flags::Carry ) ? 0x01 : 0x00 ); 

    ( accumulator - data - carryValue > 0xFF ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    if ( ( accumulator & 0b1000'0000 ) !=  ( ( accumulator - data - carryValue ) & 0b1000'0000 ) )
    {
        RaiseFlag( Cpu::Flags::Overflow );
    }
    else
    {
        ClearFlag( Cpu::Flags::Overflow );
    }

    accumulator = accumulator - data - carryValue;

    ( accumulator == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( (accumulator & 0b1000'0000) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::ASL( word address )
{
    const byte data = memory->Read( address );

    const byte bit7 = data & 0b1000'0000;
    ( bit7 == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte result = data << 1;
    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    memory->Write( address, result );

    return 2;
}

short Cpu::ASLA()
{
    const byte bit7 = accumulator & 0b1000'0000;
    ( bit7 == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte result = accumulator << 1;
    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    accumulator = result;

    return 2;
}

short Cpu::ROL( word address )
{
    const byte data = memory->Read( address );

    const byte oldCarry = IsFlagSet( Cpu::Flags::Carry ) ? 0x01 : 0x00;
    
    const byte bit7 = data & 0b1000'0000;
    ( bit7 == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    byte result = data << 1;
    result &= oldCarry;

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    memory->Write( address, result );

    return 2;
}

short Cpu::ROLA()
{
    const byte oldCarry = IsFlagSet( Cpu::Flags::Carry ) ? 0x01 : 0x00;

    const byte bit7 = accumulator & 0b1000'0000;
    ( bit7 == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    byte result = accumulator << 1;
    result &= oldCarry;

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    accumulator = result;

    return 2;
}

short Cpu::LSR( word address )
{
    const byte data = memory->Read( address );

    const byte bit0 = data & 0b0000'0001;
    ( bit0 == 0b0000'0001 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte result = data >> 1;

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );
 
    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    memory->Write( address, result );

    return 2;
}

short Cpu::LSRA()
{
    const byte bit0 = accumulator & 0b0000'0001;
    ( bit0 == 0b0000'0001 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte result = accumulator >> 1;

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    accumulator = result;

    return 2;
}

short Cpu::ROR( word address )
{
    const byte data = memory->Read( address );
    const byte oldCarry = IsFlagSet( Cpu::Flags::Carry ) ? 0b1000'0000 : 0b0000'0000;
    const byte bit0 = data & 0b0000'0001;

    byte result = data >> 1;
    result &= oldCarry;

    ( bit0 == 0b0000'0001 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    memory->Write( address, result );

    return 2;
}

short Cpu::RORA()
{
    const byte oldCarry = IsFlagSet( Cpu::Flags::Carry ) ? 0b1000'0000 : 0b0000'0000;
    const byte bit0 = accumulator & 0b0000'0001;

    byte result = accumulator >> 1;
    result &= oldCarry;

    ( bit0 == 0b0000'0001 ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    accumulator = result;

    return 2;
}

short Cpu::STX( word address )
{
    memory->Write( address, xRegisterIndex );
    return 1;
}

short Cpu::LDX( word address )
{
    xRegisterIndex = memory->Read( address );

    ( xRegisterIndex == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = xRegisterIndex & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::DEC( word address )
{
    const byte data = memory->Read( address );
    const byte result = data - 1;
    
    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );
    
    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );
    
    return 3;
}

short Cpu::INC( word address )
{
    const byte data = memory->Read( address );
    const byte result = data - 1;

    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = result & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 3;
}

short Cpu::BIT( word address )
{
    const byte mask = memory->Read( address );
    
    const byte result = accumulator & mask;
    ( result == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    ( ( mask & 0b1000'0000 )  == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Overflow ) : ClearFlag( Cpu::Flags::Overflow );

    ( ( mask & 0b0100'0000 )  == 0b0100'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 2;
}

short Cpu::JMP( word address )
{
    PC.value = address;
    
    return 1;
}

short Cpu::JMPA( word address )
{
    Register finalAddress;
    finalAddress.low = memory->Read( address );
    finalAddress.hi = memory->Read( address + 1 );

    PC = finalAddress;
    return 3;
}

short Cpu::STY( word address )
{
    memory->Write( address, yRegisterIndex );
    return 2;
}

short Cpu::LDY( word address )
{
    yRegisterIndex = memory->Read( address );

    ( yRegisterIndex == 0x00 ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero );

    const byte bit7result = yRegisterIndex & 0b1000'0000;
    ( bit7result == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::CPY( word address )
{
    const byte data = memory->Read( address );

    ( yRegisterIndex >= data ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    ( yRegisterIndex == data ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero ) ;

    const byte substraction =  yRegisterIndex - data;
    ( ( substraction & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}

short Cpu::CPX( word address )
{
    const byte data = memory->Read( address );

    ( xRegisterIndex >= data ) ? RaiseFlag( Cpu::Flags::Carry ) : ClearFlag( Cpu::Flags::Carry );

    ( xRegisterIndex == data ) ? RaiseFlag( Cpu::Flags::Zero ) : ClearFlag( Cpu::Flags::Zero ) ;

    const byte substraction =  xRegisterIndex - data;
    ( ( substraction & 0b1000'0000 ) == 0b1000'0000 ) ? RaiseFlag( Cpu::Flags::Negative ) : ClearFlag( Cpu::Flags::Negative );

    return 1;
}


/* ------------------- SUBROUTINE & INTERRUPT INSTRUCTIONS -------------------*/

short Cpu::BRK()
{
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
}

short Cpu::JSR()
{
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0;
}

short Cpu::RTI()
{
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0;
}

short Cpu::RTS()
{
    assert( false, "INSTRUCTION NOT IMPLEMENTED" );
    return 0; 
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