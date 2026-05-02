#pragma once

#include "../../common.h"

namespace libmcc::halo3 {
	inline HMODULE hModule;

	struct s_function_offset_table {
		FUNCTION_OFFSET_DECLARE(main_thread_start,								0xA120,		nullptr);
		FUNCTION_OFFSET_DECLARE(process_game_engine_globals_messages,			0xB468,		nullptr);
		FUNCTION_OFFSET_DECLARE(c_rasterizer_set_explicit_shaders,				0x278C34,	nullptr);
		FUNCTION_OFFSET_DECLARE(c_rasterizer_set_pixel_shader_constant,			0x2AF478,	nullptr);
		FUNCTION_OFFSET_DECLARE(c_rasterizer_draw_primitive_up,					0x2A32B4,	nullptr);
		FUNCTION_OFFSET_DECLARE(c_rasterizer_set_z_buffer_mode,					0x277F90,	nullptr);
		FUNCTION_OFFSET_DECLARE(c_network_session__can_accept_any_join_request,	0x11DF8,	"33 D2 F6 81 E4 41 00 00 01 74 ?? 44 8A 81 E4 41 00 00");
	};

	struct s_data_offset_table {
		DATA_OFFSET_DECLARE(c_splitscreen_config__m_config_table,			0x8ADFC0,	nullptr);
		DATA_OFFSET_DECLARE(g_player_profile_globals,						0x2D3ED70,	nullptr);
		DATA_OFFSET_DECLARE(global_scenario,									0xA46A08,	nullptr);
		DATA_OFFSET_DECLARE(physical_memory_globals,							0xA4F170,	nullptr);
		DATA_OFFSET_DECLARE(saved_film_globals,								0x2127110,	nullptr);
		DATA_OFFSET_DECLARE(g_resource_runtime_manager,						0x8F4018,	nullptr);
		DATA_OFFSET_DECLARE(g_cache_file_tags_name,							0xA49148,	nullptr);
		DATA_OFFSET_DECLARE(g_cache_file_globals,							0x20A9040,	nullptr);
		DATA_OFFSET_DECLARE(g_current_cull_mode,								0x968DC0,	nullptr);
		DATA_OFFSET_DECLARE(g_cull_mode_changed,								0x8AC580,	nullptr);
		DATA_OFFSET_DECLARE(c_visible_items_m_item,							0x917C50,	nullptr);
	};
}
