#pragma once

#include <map>

#include "../Types.h"

class Memory;
enum class DebuggerMode : byte;

class MemoryDebugger
{
public:
    MemoryDebugger();

    void ComposeView( const Memory *memory, DebuggerMode& mode );
    void UpdateWatcher( const Memory *memory, DebuggerMode& mode );

private:
    static constexpr u32 MEMORY_VIEW_ROWS = 16;
    static constexpr u32 MEMORY_VIEW_MEMORY_SIZE = 0x10000;
    static constexpr u32 MEMORY_VIEW_BASE_ADDRESS = 0x0000;

    std::map<word, byte>    watcher;
    bool                    watcherAsBreakpoint;

    bool HasWatcherDataChanged( const byte * const memory ) const;
    void UpdateWatcherData( const byte * const memory );
};