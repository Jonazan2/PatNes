#include <iostream>
#include <fstream>

#include "Cartridge.h"


Cartridge::Cartridge( const char *fileName )
    : cartridgeSize( 0 )
    , romFileName( fileName )
    , rom( nullptr )
    , isLoaded( false )
{
    if ( fileName != nullptr )
    {
        isLoaded = TryLoad( fileName );
    }
}

Cartridge::~Cartridge()
{
    delete rom;
}

bool Cartridge::IsLoaded() const
{
    return isLoaded;
}

bool Cartridge::TryLoad( const char* romFile )
{
    if ( romFile == nullptr )
    {
        return false;
    }

    std::basic_ifstream<byte> cartridgeFile( romFile, std::ios::binary | std::ios::in | std::ios::ate );
    if ( cartridgeFile.is_open() )
    {
        cartridgeSize = static_cast< u32 >( cartridgeFile.tellg() );
        cartridgeFile.seekg( 0 );

        rom = new byte[ cartridgeSize ];
        cartridgeFile.read( rom, cartridgeSize );
    }
    cartridgeFile.close();

    if ( rom != nullptr )
    {
        return TryLoadHeader();
    }

    return false;
}

bool Cartridge::TryLoadHeader()
{
    static constexpr u64 ROM_CONSTANT_HEADER = 0x4E45531A;

    if ( rom == nullptr )
    {
        return false;
    }

    /* Check that the cartridge is as big as the header */
    if ( cartridgeSize < sizeof( byte ) * 16 )
    {
        return false;
    }

    u64 constant = 0;
    for ( byte i = 0x00; i <= 0x03; ++i )
    {
        constant <<= 8;
        constant = constant | rom[i];
    }

    if ( constant != ROM_CONSTANT_HEADER )
    {
        return false;
    }

    header.prgRomSizeKB = static_cast< u32 >( rom[ 0x04 ] ) * 16;
    header.chrRomSizeKB = static_cast< u32 >( rom[ 0x05 ] ) * 8;

    /* Flags 6 */
    header.mirroringType = MirroringType( rom[ 0x06 ] & 0b0000'0001 );
    header.hasPRGRam = rom[ 0x06 ] & 0b0000'0010;
    header.has512BTrainer = rom[ 0x06 ] & 0b0000'0100;
    header.ignoreMirroring = rom[ 0x06 ] & 0b0000'1000;
    const byte lowerNibbleMapper = ( rom[ 0x06 ] & 0b1111'0000 ) >> 4;
    
    /* Flags 7 */
    const byte upperNibbleMapper = rom[ 0x07 ] & 0b1111'0000;
    header.mapper = upperNibbleMapper | lowerNibbleMapper;

    // TODO (Jonathan): Implement support for iNES 2.0
    return true;
}

void Cartridge::PrintDetails() const
{
    std::cout << "Cartridge: " << romFileName << "\n"
        << "Cartridge file Size (KB) " << cartridgeSize << "\n" 
        << "PRG ROM Size (KB): " << header.prgRomSizeKB << "\n"
        << "CHR ROM Size (KB): " << header.chrRomSizeKB<< "\n"
        << "Mirroring: " << MirroringTypeString[ static_cast< byte >( header.mirroringType ) ] << "\n"
        << "Has persistent memory: " << header.hasPersistentMemory << "\n"
        << "Contains PRG RAM: " << header.hasPRGRam << "\n"
        << "Contains 512B trainer: " << header.has512BTrainer << "\n"
        << "Ignores Mirroring: " << header.ignoreMirroring << "\n"
        << "Mapper: " << static_cast< u32 >( header.mapper );

    std::cout << std::endl;
}

const Cartridge::Header& Cartridge::GetHeader() const
{
    return header;
}

const byte* const Cartridge::GetRom() const
{
    return rom;
}