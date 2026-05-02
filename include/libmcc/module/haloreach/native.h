#pragma once

#include "../../common.h"

namespace libmcc::haloreach {
	inline HMODULE hModule;

    struct s_function_offset_table {
        FUNCTION_OFFSET_DECLARE(c_network_session__can_accept_any_join_request,	0x397354,   nullptr);
    };

    struct s_data_offset_table {
        DATA_OFFSET_DECLARE(c_splitscreen_config__m_config_table,			0xB43C40,	nullptr);
    };
}