#pragma once

#include "../native.h"

namespace libmcc::halo3 {
    inline void process_game_engine_globals_messages() {
        using t_process_game_engine_globals_messages = void(__fastcall*)();
        const auto function = MODULE_FUNCTION(
            t_process_game_engine_globals_messages,
            hModule,
            s_function_offset_table::process_game_engine_globals_messages);
        return function();
    }
}
