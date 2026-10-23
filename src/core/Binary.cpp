#include "core/Binary.h"
#include <fstream>

namespace ncc
{

std::optional<Bytes> readFile(const std::filesystem::path& path)
{
	std::ifstream in(path, std::ios::binary | std::ios::ate);
	if (!in)
		return std::nullopt;
	const auto size = in.tellg();
	if (size < 0)
		return std::nullopt;

	Bytes data(static_cast<std::size_t>(size));
	in.seekg(0);
	if (!in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size())))
		return std::nullopt;
	return data;
}

bool writeFile(const std::filesystem::path& path, ByteView data)
{
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
	out.close();
	return !out.fail();
}

}
