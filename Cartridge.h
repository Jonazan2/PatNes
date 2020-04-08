#pragma once

#include "Types.h"


class Cartridge
{
public:

    enum class MirroringType : byte
    {
        Horizontal = 0,
        Vertical,

        Count
    };

    static constexpr const char* MirroringTypeString [ static_cast< size_t >( MirroringType::Count ) ] =
    {
        "Horizontal",
        "Vertical"
    };

    struct Header
    {
        u32                        prgRomSizeKB;
        u32                        chrRomSizeKB;
        Cartridge::MirroringType    mirroringType;
        bool                        hasPersistentMemory;
        bool                        hasPRGRam;
        bool                        has512BTrainer;
        bool                        ignoreMirroring;
        byte                        mapper;

        Header()
            : prgRomSizeKB( 0 )
            , chrRomSizeKB( 0 )
            , mirroringType( Cartridge::MirroringType::Horizontal )
            , hasPersistentMemory( false )
            , hasPRGRam( false )
            , has512BTrainer( false )
            , ignoreMirroring( false )
            , mapper( 0x00 )
        {
        }
    };


    Cartridge( const char *fileName );
    ~Cartridge();

    void PrintDetails() const;
    bool IsLoaded() const;    
    const Header& GetHeader() const;
    const byte * const GetRom() const;

private:

    u32                 cartridgeSize;
    const char*         romFileName;
    byte*               rom;
    bool                isLoaded;
    Cartridge::Header   header;

    bool TryLoad( const char *romFile );
    bool TryLoadHeader();
};