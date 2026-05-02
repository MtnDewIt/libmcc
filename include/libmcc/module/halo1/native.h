#pragma once

#include "../../common.h"

namespace libmcc::halo1 {
	inline HMODULE hModule;

    struct s_function_offset_table {
        FUNCTION_OFFSET_DECLARE(dsSTATE_MGR__SetState,       0x18AE40,   nullptr);
        FUNCTION_OFFSET_DECLARE(dsSTATE_MGR__RegisterState,  0x5DB40,    nullptr);
        FUNCTION_OFFSET_DECLARE(dsSTATE_MGR__GetState,       0x18B150,   nullptr);
    };
}