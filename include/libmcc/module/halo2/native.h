#pragma once

#include "../../common.h"

namespace libmcc::halo2 {
	inline HMODULE hModule;

    struct s_function_offset_table {
        FUNCTION_OFFSET_DECLARE(game_options_verify, 0x6A6C80, nullptr);
    };
}
