#include <libmcc/memory/module.h>

#include <Windows.h>
#include <detours.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace libmcc {
module_address global_module{};

namespace {
constexpr int k_maximum_individual_modification_count = 4096;
constexpr std::uintptr_t k_invalid_address_marker = 0xFEFEFEFEull;

void copy_name(char (&destination)[128], const char* source) {
	if (source == nullptr) {
		destination[0] = '\0';
		return;
	}

	std::strncpy(destination, source, sizeof(destination) - 1);
	destination[sizeof(destination) - 1] = '\0';
}

std::uintptr_t normalize_hook_target_rva(std::uintptr_t address, bool remove_base) {
	return normalize_offset(address, remove_base);
}
} // namespace

static void* mountain_module = nullptr;

void* global_address_get(uint32_t rva) {
	if (global_module.pointer == nullptr) {
		global_module.pointer = GetModuleHandleW(nullptr);
	}

	assert(global_module.pointer != nullptr);

	const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(global_module.pointer);
	return reinterpret_cast<byte*>(base + static_cast<std::uintptr_t>(rva));
}

void set_mountain_module(void* module_handle) {
	mountain_module = module_handle;
	if (module_handle != nullptr) {
		global_module.pointer = module_handle;
	}
}

void* get_mountain_module() {
	return mountain_module;
}

int g_call_hook_count = 0;
c_hook_call* call_hooks[k_maximum_individual_modification_count]{};

int g_detour_hook_count = 0;
c_hook* detour_hooks[k_maximum_individual_modification_count]{};

int g_data_patch_count = 0;
c_data_patch* data_patches[k_maximum_individual_modification_count]{};

int g_data_patch_array_count = 0;
c_data_patch_array* data_patch_arrays[k_maximum_individual_modification_count]{};

void apply_all_hooks(bool revert) {
	for (int call_index = 0; call_index < g_call_hook_count; call_index++) {
		if (c_hook_call* call_hook = call_hooks[call_index]; call_hook != nullptr) {
			if (!call_hook->apply(revert) && !revert) {
				std::printf(
					"call hook didn't apply: %p, %s\n",
					call_hook->get_site_pointer(),
					call_hook->get_name());
			}
		}
	}

	for (int detour_index = 0; detour_index < g_detour_hook_count; detour_index++) {
		if (c_hook* detour_hook = detour_hooks[detour_index]; detour_hook != nullptr) {
			if (!detour_hook->apply(revert) && !revert) {
				std::printf(
					"detour hook didn't apply: %p, %s\n",
					detour_hook->get_detour_pointer(),
					detour_hook->get_name());
			}
		}
	}
}

void apply_all_patches(bool revert) {
	for (int data_patch_index = 0; data_patch_index < g_data_patch_count; data_patch_index++) {
		if (c_data_patch* data_patch = data_patches[data_patch_index]; data_patch != nullptr) {
			if (!data_patch->apply(revert) && !revert) {
				std::printf(
					"data patch didn't apply: %p, %s\n",
					data_patch->get_site_pointer(),
					data_patch->get_name());
			}
		}
	}

	for (int data_patch_array_index = 0; data_patch_array_index < g_data_patch_array_count; data_patch_array_index++) {
		if (c_data_patch_array* data_patch_array = data_patch_arrays[data_patch_array_index]; data_patch_array != nullptr) {
			if (!data_patch_array->apply(revert) && !revert) {
				std::printf("data patch array didn't apply: %s\n", data_patch_array->get_name());
			}
		}
	}
}

c_hook::c_hook(const char* name, std::uintptr_t address, module_address function, bool remove_base)
	: m_addr(function)
	, m_orig({ .pointer = static_cast<byte*>(global_address_get(static_cast<uint32_t>(normalize_hook_target_rva(address, remove_base)))) }) {
	copy_name(m_name, name);
	assert(g_detour_hook_count >= 0 && g_detour_hook_count < k_maximum_individual_modification_count);
	detour_hooks[g_detour_hook_count++] = this;
}

bool c_hook::apply(bool revert) {
	if (m_orig.pointer == nullptr || m_addr.pointer == nullptr) {
		return false;
	}

	if (reinterpret_cast<std::uintptr_t>(m_orig.pointer) == k_invalid_address_marker
		|| reinterpret_cast<std::uintptr_t>(m_addr.pointer) == k_invalid_address_marker) {
		return false;
	}

	if (DetourTransactionBegin() != NO_ERROR) {
		return false;
	}

	if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
		DetourTransactionAbort();
		return false;
	}

	void* original = m_orig.pointer;
	const LONG attach_result = revert
		? DetourDetach(&original, m_addr.pointer)
		: DetourAttach(&original, m_addr.pointer);
	if (attach_result != NO_ERROR) {
		DetourTransactionAbort();
		return false;
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		return false;
	}

	m_orig.pointer = static_cast<byte*>(original);
	return true;
}

const char* c_hook::get_name() const {
	return m_name;
}

void* c_hook::get_detour_pointer() const {
	return m_addr.pointer;
}

void* c_hook::get_original_pointer() const {
	return m_orig.pointer;
}

c_hook_call::c_hook_call(const char* name, std::uintptr_t address, module_address function, bool remove_base)
	: m_addr({ .pointer = static_cast<byte*>(global_address_get(static_cast<uint32_t>(normalize_hook_target_rva(address, remove_base)))) }) {
	copy_name(m_name, name);

	const std::uintptr_t site = reinterpret_cast<std::uintptr_t>(m_addr.pointer);
	const std::uintptr_t target = reinterpret_cast<std::uintptr_t>(function.pointer);
	m_call = call_instruction{
		.opcode = 0xE8,
		.offset = static_cast<uint32_t>(target - site - sizeof(call_instruction)),
	};

	assert(g_call_hook_count >= 0 && g_call_hook_count < k_maximum_individual_modification_count);
	call_hooks[g_call_hook_count++] = this;
}

bool c_hook_call::apply(bool revert) {
	if (m_addr.pointer == nullptr) {
		return false;
	}

	if (reinterpret_cast<std::uintptr_t>(m_addr.pointer) == k_invalid_address_marker) {
		return false;
	}

	if (!revert) {
		std::memcpy(&m_call_original, m_addr.pointer, sizeof(call_instruction));
	}

	DWORD protect = 0;
	if (!VirtualProtect(m_addr.pointer, sizeof(call_instruction), PAGE_READWRITE, &protect)) {
		return false;
	}

	std::memcpy(m_addr.pointer, revert ? &m_call_original : &m_call, sizeof(call_instruction));

	if (!VirtualProtect(m_addr.pointer, sizeof(call_instruction), protect, &protect)) {
		return false;
	}

	return true;
}

const char* c_hook_call::get_name() const {
	return m_name;
}

void* c_hook_call::get_site_pointer() const {
	return m_addr.pointer;
}

c_data_patch::c_data_patch(const char* name, std::uintptr_t address, int patch_size, byte const(&patch)[], bool remove_base)
	: m_addr({ .pointer = static_cast<byte*>(global_address_get(static_cast<uint32_t>(normalize_hook_target_rva(address, remove_base)))) })
	, m_bytes(patch)
	, m_bytes_original(new byte[static_cast<size_t>(patch_size)]{})
	, m_byte_count(patch_size) {
	copy_name(m_name, name);
	assert(g_data_patch_count >= 0 && g_data_patch_count < k_maximum_individual_modification_count);
	data_patches[g_data_patch_count++] = this;
}

bool c_data_patch::apply(bool revert) {
	if (m_addr.pointer == nullptr) {
		return false;
	}

	if (reinterpret_cast<std::uintptr_t>(m_addr.pointer) == k_invalid_address_marker) {
		return false;
	}

	if (!revert) {
		std::memcpy(m_bytes_original, m_addr.pointer, static_cast<size_t>(m_byte_count));
	}

	DWORD protect = 0;
	if (!VirtualProtect(m_addr.pointer, static_cast<SIZE_T>(m_byte_count), PAGE_READWRITE, &protect)) {
		return false;
	}

	std::memcpy(m_addr.pointer, revert ? m_bytes_original : m_bytes, static_cast<size_t>(m_byte_count));

	if (!VirtualProtect(m_addr.pointer, static_cast<SIZE_T>(m_byte_count), protect, &protect)) {
		return false;
	}

	return true;
}

const char* c_data_patch::get_name() const {
	return m_name;
}

void* c_data_patch::get_site_pointer() const {
	return m_addr.pointer;
}

c_data_patch_array::c_data_patch_array(
	const char* name,
	int address_count,
	uint32_t const(&addresses)[],
	int patch_size,
	void* patch,
	bool remove_base)
	: m_address_count(address_count)
	, m_addresses(addresses)
	, m_byte_count(patch_size)
	, m_bytes(patch)
	, m_bytes_original(new byte*[static_cast<size_t>(address_count)]{}) {
	(void)remove_base;
	copy_name(m_name, name);
	assert(g_data_patch_array_count >= 0 && g_data_patch_array_count < k_maximum_individual_modification_count);
	data_patch_arrays[g_data_patch_array_count++] = this;
}

c_data_patch_array::~c_data_patch_array() {
	if (m_bytes_original != nullptr) {
		for (int i = 0; i < m_address_count; i++) {
			if (m_bytes_original[i] != nullptr) {
				delete[] m_bytes_original[i];
				m_bytes_original[i] = nullptr;
			}
		}

		delete[] m_bytes_original;
		m_bytes_original = nullptr;
	}
}

bool c_data_patch_array::apply(bool revert) {
	for (int i = 0; i < m_address_count; i++) {
		module_address address{};
		address.pointer = global_address_get(m_addresses[i]);

		if (!revert) {
			m_bytes_original[i] = static_cast<byte*>(std::memcpy(new byte[static_cast<size_t>(m_byte_count)]{}, address.pointer, static_cast<size_t>(m_byte_count)));
		}

		DWORD protect = 0;
		if (!VirtualProtect(address.pointer, static_cast<SIZE_T>(m_byte_count), PAGE_READWRITE, &protect)) {
			continue;
		}

		std::memcpy(address.pointer, revert ? m_bytes_original[i] : m_bytes, static_cast<size_t>(m_byte_count));

		if (!VirtualProtect(address.pointer, static_cast<SIZE_T>(m_byte_count), protect, &protect)) {
			continue;
		}
	}

	return true;
}

const char* c_data_patch_array::get_name() const {
	return m_name;
}

void buffer_as_byte_string(byte* buffer, uint32_t buffer_size, char* out_string, int out_string_size) {
	std::memset(out_string, 0, static_cast<size_t>(out_string_size));
	for (uint32_t i = 0; i < buffer_size; i++) {
		std::snprintf(&out_string[3 * i], static_cast<size_t>(out_string_size - static_cast<int>(3 * i)), "%02X ", buffer[i]);
	}
}

bool patch_pointer(module_address address, const void* pointer) {
	DWORD protect = 0;
	if (!VirtualProtect(address.pointer, sizeof(void*), PAGE_READWRITE, &protect)) {
		return false;
	}

	std::memcpy(address.pointer, &pointer, sizeof(void*));

	if (!VirtualProtect(address.pointer, sizeof(void*), protect, &protect)) {
		return false;
	}

	return true;
}
} // namespace libmcc
