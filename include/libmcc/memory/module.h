#pragma once

#include "../common.h"

#include <cstddef>
#include <cstdint>

#define LIBMCC_CONCAT_IMPL(a, b) a##b
#define LIBMCC_CONCAT(a, b) LIBMCC_CONCAT_IMPL(a, b)
#define LIBMCC_NUMBEROF(array_) (sizeof(array_) / sizeof((array_)[0]))

#define HOOK_DECLARE_CALL_WITH_ADDRESS(ADDR, ADDR2, NAME) \
	inline static libmcc::c_hook_call LIBMCC_CONCAT(NAME##_hook, __LINE__)(#NAME, ADDR, { .address = ADDR2 })
#define HOOK_DECLARE_CALL(ADDR, NAME) \
	inline static libmcc::c_hook_call LIBMCC_CONCAT(NAME##_hook, __LINE__)(#NAME, ADDR, { .pointer = reinterpret_cast<void*>(&NAME) })
#define HOOK_DECLARE(ADDR, NAME) \
	static libmcc::c_hook NAME##_hook(#NAME, ADDR, { .pointer = reinterpret_cast<void*>(&NAME) })
#define HOOK_DECLARE_CLASS(ADDR, CLASS, NAME) \
	static libmcc::c_hook CLASS##_##NAME##_hook(#NAME, ADDR, { .pointer = reinterpret_cast<void*>(&CLASS::NAME) })

#define HOOK_INVOKE(RESULT, NAME, ...) \
	(RESULT) = reinterpret_cast<decltype(&NAME)>(NAME##_hook.get_original_pointer())(__VA_ARGS__)

#define HOOK_INVOKE_VOID(NAME, ...) \
	reinterpret_cast<decltype(&NAME)>(NAME##_hook.get_original_pointer())(__VA_ARGS__)

#define HOOK_INVOKE_CLASS(RESULT, CLASS, NAME, TYPE, ...) \
	(RESULT) = reinterpret_cast<TYPE>(CLASS##_##NAME##_hook.get_original_pointer())(__VA_ARGS__)

#define DATA_PATCH_DECLARE(ADDR, NAME, ...) \
	static libmcc::c_data_patch LIBMCC_CONCAT(NAME##_patch, __LINE__)(#NAME, ADDR, LIBMCC_NUMBEROF(__VA_ARGS__), __VA_ARGS__)
#define DATA_PATCH_DECLARE2(ADDR, NAME, SIZE, ...) \
	static libmcc::c_data_patch LIBMCC_CONCAT(NAME##_patch, __LINE__)(#NAME, ADDR, SIZE, __VA_ARGS__)

#define DATA_PATCH_ARRAY_DECLARE(ADDRS, NAME, ...) \
	static libmcc::c_data_patch_array LIBMCC_CONCAT(NAME##_patch, __LINE__)(#NAME, LIBMCC_NUMBEROF(ADDRS), ADDRS, LIBMCC_NUMBEROF(__VA_ARGS__), __VA_ARGS__)
#define DATA_PATCH_ARRAY_DECLARE2(ADDRS, NAME, ...) \
	static libmcc::c_data_patch_array NAME##_patch(#NAME, LIBMCC_NUMBEROF(ADDRS), ADDRS, LIBMCC_NUMBEROF(__VA_ARGS__), __VA_ARGS__)

namespace libmcc {
extern module_address global_module;

void* global_address_get(uint32_t rva);

void set_mountain_module(void* module_handle);
void* get_mountain_module();

void apply_all_hooks(bool revert);
void apply_all_patches(bool revert);

class c_hook {
public:
	c_hook(const char* name, std::uintptr_t address, module_address function, bool remove_base = true);

	bool apply(bool revert);

	const char* get_name() const;
	void* get_detour_pointer() const;
	void* get_original_pointer() const;

private:
	char m_name[128]{};
	module_address m_addr{};
	module_address m_orig{};
};

class c_hook_call {
#pragma pack(push, 1)
	struct call_instruction {
		byte opcode;
		uint32_t offset;
	};
#pragma pack(pop)
	static_assert(sizeof(call_instruction) == 0x5);

public:
	c_hook_call(const char* name, std::uintptr_t address, module_address function, bool remove_base = true);

	bool apply(bool revert);

	const char* get_name() const;
	void* get_site_pointer() const;

private:
	char m_name[128]{};
	module_address m_addr{};
	call_instruction m_call{};
	call_instruction m_call_original{};
};

class c_data_patch {
public:
	c_data_patch(const char* name, std::uintptr_t address, int patch_size, byte const(&patch)[], bool remove_base = true);

	bool apply(bool revert);

	const char* get_name() const;
	void* get_site_pointer() const;

private:
	char m_name[128]{};
	module_address m_addr{};
	const byte* m_bytes{};
	byte* m_bytes_original{};
	int m_byte_count{};
};

class c_data_patch_array {
public:
	c_data_patch_array(
		const char* name,
		int address_count,
		uint32_t const(&addresses)[],
		int patch_size,
		void* patch,
		bool remove_base = true);

	~c_data_patch_array();

	bool apply(bool revert);

	const char* get_name() const;

private:
	char m_name[128]{};
	int m_address_count{};
	const uint32_t* m_addresses{};
	int m_byte_count{};
	void* m_bytes{};
	byte** m_bytes_original{};
};

void buffer_as_byte_string(byte* buffer, uint32_t buffer_size, char* out_string, int out_string_size);

template<typename T, int k_string_size>
void type_as_byte_string(T* type, char(&out_string)[k_string_size]) {
	buffer_as_byte_string(reinterpret_cast<byte*>(type), sizeof(T), out_string, k_string_size);
}

bool patch_pointer(module_address address, const void* pointer);
} // namespace libmcc
