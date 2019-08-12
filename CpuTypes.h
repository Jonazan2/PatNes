#pragma once

#include <unordered_map>
#include <array>

#include "Types.h"


enum class CpuAddressMode : byte
{
    Implicit = 0,
    Accumulator,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndexedX,
    IndexedY,

    MAX
};

constexpr std::array<const char *, static_cast< byte > ( CpuAddressMode::MAX ) > ADDRESS_MODE_STRING =
{
    "Implicit",
    "Accumulator",
    "Immediate",
    "Zero Page",
    "Zero Page X",
    "Zero Page Y",
    "Relative",
    "Absolute",
    "Absolute X",
    "Absolute Y",
    "Indirect",
    "Indexed X",
    "Indexed Y",
};

constexpr std::array< byte, static_cast< byte > ( CpuAddressMode::MAX ) > ADDRESS_MODE_OPCODE_LENGTH =
{
    1,  /* CpuAddressMode::Implicit */
    1,  /* CpuAddressMode::Accumulator */
    2,  /* CpuAddressMode::Immediate */
    2,  /* CpuAddressMode::ZeroPage */
    2,  /* CpuAddressMode::ZeroPageX */
    2,  /* CpuAddressMode::ZeroPageY */
    2,  /* CpuAddressMode::Relative */
    3,  /* CpuAddressMode::Absolute */
    3,  /* CpuAddressMode::AbsoluteX */
    3,  /* CpuAddressMode::AbsoluteY */
    3,  /* CpuAddressMode::Indirect */
    2,  /* CpuAddressMode::IndexedX */
    2,  /* CpuAddressMode::IndexedY */
};

struct OpcodeInfo
{
    const char*     mnemonic;
    CpuAddressMode  addressMode;
};

static const std::unordered_map<byte, OpcodeInfo> NES_OPCODE_INFO = 
{
    { 0x00, { "BRK", CpuAddressMode::Implicit } },
    { 0x01, { "ORA", CpuAddressMode::IndexedX } },
    { 0x05, { "ORA", CpuAddressMode::ZeroPage } },
    { 0x06, { "ASL", CpuAddressMode::ZeroPage } },
    { 0x08, { "PHP", CpuAddressMode::Implicit } },
    { 0x09, { "ORA", CpuAddressMode::Immediate } },
    { 0x0A, { "ASL", CpuAddressMode::Accumulator } },
    { 0x0D, { "ORA", CpuAddressMode::Absolute } },
    { 0x0E, { "ASL", CpuAddressMode::Absolute } },

    { 0x10, { "BPL", CpuAddressMode::Relative } },
    { 0x11, { "ORA", CpuAddressMode::IndexedY } },
    { 0x15, { "ORA", CpuAddressMode::ZeroPageX } },
    { 0x16, { "ASL", CpuAddressMode::ZeroPageX } },
    { 0x18, { "CLC", CpuAddressMode::Implicit } },
    { 0x19, { "ORA", CpuAddressMode::AbsoluteY } },
    { 0x1D, { "ORA", CpuAddressMode::AbsoluteX } },
    { 0x1E, { "ASL", CpuAddressMode::AbsoluteX } },

    { 0x20, { "JSR", CpuAddressMode::Absolute } },
    { 0x21, { "AND", CpuAddressMode::IndexedX } },
    { 0x24, { "BIT", CpuAddressMode::ZeroPage } },
    { 0x25, { "AND", CpuAddressMode::ZeroPage } },
    { 0x26, { "ROL", CpuAddressMode::ZeroPage } },
    { 0x28, { "PLP", CpuAddressMode::Implicit } },
    { 0x29, { "AND", CpuAddressMode::Immediate } },
    { 0x2A, { "ROL", CpuAddressMode::Accumulator } },
    { 0x2C, { "BIT", CpuAddressMode::Absolute } },
    { 0x2D, { "AND", CpuAddressMode::Absolute } },
    { 0x2E, { "ROL", CpuAddressMode::Absolute } },

    { 0x30, { "BMI", CpuAddressMode::Relative } },
    { 0x31, { "AND", CpuAddressMode::IndexedY } },
    { 0x35, { "AND", CpuAddressMode::ZeroPageX } },
    { 0x36, { "ROL", CpuAddressMode::ZeroPageX } },
    { 0x38, { "SEC", CpuAddressMode::Implicit } },
    { 0x39, { "AND", CpuAddressMode::AbsoluteY } },
    { 0x3D, { "AND", CpuAddressMode::AbsoluteX } },
    { 0x3E, { "ROL", CpuAddressMode::AbsoluteX } },

    { 0x40, { "RTI", CpuAddressMode::Implicit } },
    { 0x41, { "EOR", CpuAddressMode::IndexedX } },
    { 0x45, { "EOR", CpuAddressMode::ZeroPage } },
    { 0x46, { "LSR", CpuAddressMode::ZeroPage } },
    { 0x48, { "PHA", CpuAddressMode::Implicit } },
    { 0x49, { "EOR", CpuAddressMode::Immediate } },
    { 0x4A, { "LSR", CpuAddressMode::Accumulator } },
    { 0x4C, { "JMP", CpuAddressMode::Absolute } },
    { 0x4D, { "EOR", CpuAddressMode::Absolute } },
    { 0x4E, { "LSR", CpuAddressMode::Absolute } },

    { 0x50, { "BVC", CpuAddressMode::Relative } },
    { 0x51, { "EOR", CpuAddressMode::IndexedY } },
    { 0x55, { "EOR", CpuAddressMode::ZeroPageX } },
    { 0x56, { "LSR", CpuAddressMode::ZeroPageX } },
    { 0x58, { "CLI", CpuAddressMode::Implicit } },
    { 0x59, { "EOR", CpuAddressMode::AbsoluteY } },
    { 0x5D, { "EOR", CpuAddressMode::AbsoluteX } },
    { 0x5E, { "LSR", CpuAddressMode::AbsoluteX } },

    { 0x60, { "RTS", CpuAddressMode::Implicit } },
    { 0x61, { "ADC", CpuAddressMode::IndexedX } },
    { 0x65, { "ADC", CpuAddressMode::ZeroPage } },
    { 0x66, { "ROR", CpuAddressMode::ZeroPage } },
    { 0x68, { "PLA", CpuAddressMode::Implicit } },
    { 0x69, { "ADC", CpuAddressMode::Immediate } },
    { 0x6A, { "ROR", CpuAddressMode::Accumulator } },
    { 0x6C, { "JMP", CpuAddressMode::Indirect } },
    { 0x6D, { "ADC", CpuAddressMode::Absolute } },
    { 0x6E, { "ROR", CpuAddressMode::Absolute } },

    { 0x70, { "BVS", CpuAddressMode::Relative } },
    { 0x71, { "ADC", CpuAddressMode::IndexedY } },
    { 0x75, { "ADC", CpuAddressMode::ZeroPageX } },
    { 0x76, { "ROR", CpuAddressMode::ZeroPageX } },
    { 0x78, { "SEI", CpuAddressMode::Implicit } },
    { 0x79, { "ADC", CpuAddressMode::AbsoluteY } },
    { 0x7D, { "ADC", CpuAddressMode::AbsoluteX } },
    { 0x7E, { "ROR", CpuAddressMode::AbsoluteX } },

    { 0x81, { "STA", CpuAddressMode::IndexedX } },
    { 0x84, { "STY", CpuAddressMode::ZeroPage } },
    { 0x85, { "STA", CpuAddressMode::ZeroPage } },
    { 0x86, { "STX", CpuAddressMode::ZeroPage } },
    { 0x88, { "DEY", CpuAddressMode::Implicit } },
    { 0x8A, { "TXA", CpuAddressMode::Implicit } },
    { 0x8C, { "STY", CpuAddressMode::Absolute } },
    { 0x8D, { "STA", CpuAddressMode::Absolute } },
    { 0x8E, { "STX", CpuAddressMode::Absolute } },

    { 0x90, { "BCC", CpuAddressMode::Relative } },
    { 0x91, { "STA", CpuAddressMode::IndexedY } },
    { 0x94, { "STY", CpuAddressMode::ZeroPageX } },
    { 0x95, { "STA", CpuAddressMode::ZeroPageX } },
    { 0x96, { "STX", CpuAddressMode::ZeroPageY } },
    { 0x98, { "TYA", CpuAddressMode::Implicit } },
    { 0x99, { "STA", CpuAddressMode::AbsoluteY } },
    { 0x9A, { "TXS", CpuAddressMode::Implicit } },
    { 0x9D, { "STA", CpuAddressMode::AbsoluteX } },

    { 0xA0, { "LDY", CpuAddressMode::Immediate } },
    { 0xA1, { "LDA", CpuAddressMode::IndexedX } },
    { 0xA2, { "LDX", CpuAddressMode::Immediate } },
    { 0xA4, { "LDY", CpuAddressMode::ZeroPage } },
    { 0xA5, { "LDA", CpuAddressMode::ZeroPage } },
    { 0xA6, { "LDX", CpuAddressMode::ZeroPage } },
    { 0xA8, { "TAY", CpuAddressMode::Implicit } },
    { 0xA9, { "LDA", CpuAddressMode::Immediate } },
    { 0xAA, { "TAX", CpuAddressMode::Implicit } },
    { 0xAC, { "LDY", CpuAddressMode::Absolute } },
    { 0xAD, { "LDA", CpuAddressMode::Absolute } },
    { 0xAE, { "LDX", CpuAddressMode::Absolute } },

    { 0xB0, { "BCS", CpuAddressMode::Relative } },
    { 0xB1, { "LDA", CpuAddressMode::IndexedY } },
    { 0xB4, { "LDY", CpuAddressMode::ZeroPageX } },
    { 0xB5, { "LDA", CpuAddressMode::ZeroPageX } },
    { 0xB6, { "LDX", CpuAddressMode::ZeroPageY } },
    { 0xB8, { "CLV", CpuAddressMode::Implicit } },
    { 0xB9, { "LDA", CpuAddressMode::AbsoluteY } },
    { 0xBA, { "TSX", CpuAddressMode::Implicit } },
    { 0xBC, { "LDY", CpuAddressMode::AbsoluteX } },
    { 0xBD, { "LDA", CpuAddressMode::AbsoluteX } },
    { 0xBE, { "LDX", CpuAddressMode::AbsoluteY } },

    { 0xC0, { "CPY", CpuAddressMode::Immediate } },
    { 0xC1, { "CMP", CpuAddressMode::IndexedX } },
    { 0xC4, { "CPY", CpuAddressMode::ZeroPage } },
    { 0xC5, { "CMP", CpuAddressMode::ZeroPage } },
    { 0xC6, { "DEC", CpuAddressMode::ZeroPage } },
    { 0xC8, { "INY", CpuAddressMode::Implicit } },
    { 0xC9, { "CMP", CpuAddressMode::Immediate } },
    { 0xCA, { "DEX", CpuAddressMode::Implicit } },
    { 0xCC, { "CPY", CpuAddressMode::Absolute } },
    { 0xCD, { "CMP", CpuAddressMode::Absolute } },
    { 0xCE, { "DEC", CpuAddressMode::Absolute } },

    { 0xD0, { "BNE", CpuAddressMode::Relative } },
    { 0xD1, { "CMP", CpuAddressMode::IndexedY } },
    { 0xD5, { "CMP", CpuAddressMode::ZeroPageX } },
    { 0xD6, { "DEC", CpuAddressMode::ZeroPageX } },
    { 0xD8, { "CLD", CpuAddressMode::Implicit } },
    { 0xD9, { "CMP", CpuAddressMode::AbsoluteY, } },    
    { 0xDD, { "CMP", CpuAddressMode::AbsoluteX } },
    { 0xDE, { "DEC", CpuAddressMode::AbsoluteX } },

    { 0xE0, { "CPX", CpuAddressMode::Immediate } },
    { 0xE1, { "SBC", CpuAddressMode::IndexedX } },
    { 0xE4, { "CPX", CpuAddressMode::ZeroPage } },
    { 0xE5, { "SBC", CpuAddressMode::ZeroPage } },
    { 0xE6, { "INC", CpuAddressMode::ZeroPage } },
    { 0xE8, { "INC", CpuAddressMode::Implicit } },
    { 0xE9, { "SBC", CpuAddressMode::Immediate } },
    { 0xEA, { "NOP", CpuAddressMode::Implicit } },
    { 0xEC, { "CPX", CpuAddressMode::Absolute } },
    { 0xEE, { "INC", CpuAddressMode::Absolute } },

    { 0xF0, { "BEQ", CpuAddressMode::Relative } },
    { 0xF1, { "SBC", CpuAddressMode::IndexedY } },
    { 0xF5, { "SBC", CpuAddressMode::ZeroPageX } },
    { 0xF6, { "INC", CpuAddressMode::ZeroPageX } },
    { 0xF8, { "SED", CpuAddressMode::Implicit } },
    { 0xF9, { "SBC", CpuAddressMode::AbsoluteY } },
    { 0xFD, { "SBC", CpuAddressMode::AbsoluteX } },
    { 0xFE, { "INC", CpuAddressMode::AbsoluteX } },
};