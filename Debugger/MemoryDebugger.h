#pragma once

#include <map>

#include "../Types.h"

class Memory;
class Video;
enum class DebuggerMode : byte;

class MemoryDebugger
{
    enum class CurrentSelectedView : byte
    {
        Memory = 0,
        Video
    };

public:
    MemoryDebugger();

    void ComposeView( const Memory *memory, const Video *video, DebuggerMode& mode );
    void UpdateWatcher( const Memory *memory, DebuggerMode& mode );

private:
    static constexpr u32 MEMORY_VIEW_ROWS = 16;
    static constexpr u32 MEMORY_VIEW_MEMORY_SIZE = 0x10000;
    static constexpr u32 MEMORY_VIEW_BASE_ADDRESS = 0x0000;

    std::map<word, byte>    watcher;
    bool                    watcherAsBreakpoint;
    CurrentSelectedView     currentView;

    void ComposeMemoryHexContentView( const byte *map, DebuggerMode& mode  );
    void ComposeMemoryWatcherView( const byte *map, DebuggerMode& mode  );
    bool HasWatcherDataChanged( const byte * const memory ) const;
    void UpdateWatcherData( const byte * const memory );
};