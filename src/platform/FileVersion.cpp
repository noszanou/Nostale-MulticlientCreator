#include "platform/FileVersion.h"
#include <cstddef>
#include <format>
#include <vector>
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "version.lib")
#endif

namespace ncc::platform
{

std::optional<std::string> fileVersion(const std::filesystem::path& file)
{
	DWORD handle = 0;
	const DWORD size = GetFileVersionInfoSizeW(file.c_str(), &handle);
	if (size == 0)
		return std::nullopt;

	std::vector<std::byte> buffer(size);
	if (!GetFileVersionInfoW(file.c_str(), 0, size, buffer.data()))
		return std::nullopt;

	VS_FIXEDFILEINFO* info = nullptr;
	UINT length = 0;
	if (!VerQueryValueW(buffer.data(), L"\\", reinterpret_cast<void**>(&info), &length) || length < sizeof(VS_FIXEDFILEINFO))
		return std::nullopt;

	return std::format("{}.{}.{}.{}",
		HIWORD(info->dwProductVersionMS), LOWORD(info->dwFileVersionMS),
		HIWORD(info->dwFileVersionLS), LOWORD(info->dwFileVersionLS));
}

}
