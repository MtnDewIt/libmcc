#pragma once

#include "./win32/win32.h"
#include "./xdk/xdk.h"
#include "./math/math.inl"

#include <array>
#include <cstdint>

namespace libmcc {
	typedef int string_id;

	typedef unsigned int datum_index;

	typedef uint64_t network_id;

	union module_address {
		std::uintptr_t address;
		byte* data;
		void* pointer;
	};

	struct s_offset_table_item {
		constexpr s_offset_table_item(std::uintptr_t address)
			: address(address) {
		}

		std::uintptr_t address;
	};

	inline constexpr std::uintptr_t k_default_image_base = 0x140000000ull;

	inline constexpr std::uintptr_t normalize_offset(std::uintptr_t address, bool remove_base = true) {
		if (!remove_base) {
			return address;
		}

		return address >= k_default_image_base
			? address - k_default_image_base
			: address;
	}

	inline module_address resolve_module_address(HMODULE hModule, std::uintptr_t address, bool remove_base = true) {
		return { .pointer = reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(hModule) + normalize_offset(address, remove_base)) };
	}

	template <typename T>
	inline T* TLS(HMODULE hModule) {
		return reinterpret_cast<T*>(get_tls(get_tls_index(hModule)));
	}

	template <typename T>
	inline T FUNCTION(HMODULE hModule, const s_offset_table_item& func, bool remove_base = true) {
		return reinterpret_cast<T>(resolve_module_address(hModule, func.address, remove_base).pointer);
	}

	template <typename T>
	inline T* GLOBAL(HMODULE hModule, const s_offset_table_item& data, bool remove_base = true) {
		return static_cast<T*>(resolve_module_address(hModule, data.address, remove_base).pointer);
	}

	class i_unknown {
	public:
		virtual void __fastcall free() = 0;
	};

	enum e_unknown {};

	template<typename T = int, typename E = e_unknown>
	struct s_flags {
		s_flags() {}
		s_flags(T n) : n(n) {}

		bool bit_test(E position) {
			return n & (1 << position);
		}

		void bit_set(E position, bool value) {
			if (value) {
				n |= (1 << position);
			} else {
				n &= ~(1 << position);
			}
		}

		bool bit_test(T position) {
			return bit_test(static_cast<E>(position));
		}

		void bit_set(T position, bool value) {
			bit_set(static_cast<E>(position), value);
		}

		void operator=(T value) {
			n = value;
		}

		operator T() const {
			return n;
		}

		T n;
	};
}

#define byte_swap(x) _byteswap_ulong(x)

#define OFFSET_DECLARE(name, offset) constexpr static const s_offset_table_item name = s_offset_table_item(offset)
#define FUNCTION_OFFSET_DECLARE(name, offset) OFFSET_DECLARE(name, offset)
#define DATA_OFFSET_DECLARE(name, offset) OFFSET_DECLARE(name, offset)

#define MODULE_TLS(type, module) libmcc::TLS<type>(module)
#define MODULE_FUNCTION(type, module, func) libmcc::FUNCTION<type>(module, func)
#define MODULE_GLOBAL(type, module, data) libmcc::GLOBAL<type>(module, data)

#define MAKE_OFFSET_TABLE_ITEM(name, offset) OFFSET_DECLARE(name, offset)

#define DEF_PVF(ret, name, ...) virtual ret __fastcall name(__VA_ARGS__) = 0

#define DEF_VFT(ret, name, ...) ret (__fastcall* name)(__VA_ARGS__)
