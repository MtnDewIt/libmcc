#pragma once

#include "../../common.h"

namespace libmcc::halo3odst {
	inline HMODULE hModule;

    struct s_function_offset_table {
        FUNCTION_OFFSET_DECLARE(c_network_session__can_accept_any_join_request,	0x1258C,	nullptr);
    };

    struct s_data_offset_table {
        DATA_OFFSET_DECLARE(c_splitscreen_config__m_config_table,			0x8F1E20,	nullptr);
    };
}
