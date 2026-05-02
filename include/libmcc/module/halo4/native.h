#pragma once

#include "../../common.h"

namespace libmcc::halo4 {
	inline HMODULE hModule;

    struct s_data_offset_table {
        DATA_OFFSET_DECLARE(c_splitscreen_config__m_config_table,    0xE84DB0,	nullptr);
    };
}
