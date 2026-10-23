#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace ncc
{

using Bytes = std::vector<std::uint8_t>;
using ByteView = std::span<const std::uint8_t>;
using MutableByteView = std::span<std::uint8_t>;

[[nodiscard]] std::optional<Bytes> readFile(const std::filesystem::path& path);
[[nodiscard]] bool writeFile(const std::filesystem::path& path, ByteView data);

[[nodiscard]] constexpr bool fits(ByteView data, std::size_t offset, std::size_t count) noexcept
{
	return offset <= data.size() && data.size() - offset >= count;
}

template<std::integral T>
[[nodiscard]] constexpr T littleEndian(T value) noexcept
{
	if constexpr (std::endian::native == std::endian::big)
		return std::byteswap(value);
	else
		return value;
}

template<std::integral T>
[[nodiscard]] constexpr auto toBytes(T value) noexcept
{
	return std::bit_cast<std::array<std::uint8_t, sizeof(T)>>(littleEndian(value));
}

template<std::integral T>
[[nodiscard]] constexpr std::optional<T> load(ByteView data, std::size_t offset) noexcept
{
	if (!fits(data, offset, sizeof(T)))
		return std::nullopt;
	std::array<std::uint8_t, sizeof(T)> raw{};
	std::ranges::copy(data.subspan(offset, sizeof(T)), raw.begin());
	return littleEndian(std::bit_cast<T>(raw));
}

template<std::integral T>
constexpr void store(MutableByteView data, std::size_t offset, T value) noexcept
{
	std::ranges::copy(toBytes(value), data.subspan(offset, sizeof(T)).begin());
}

[[nodiscard]] inline std::optional<std::size_t> find(ByteView data, ByteView needle, std::size_t from = 0)
{
	if (from > data.size())
		return std::nullopt;
	const auto haystack = data.subspan(from);
	const auto hit = std::ranges::search(haystack, needle);
	if (hit.empty())
		return std::nullopt;
	return from + static_cast<std::size_t>(hit.begin() - haystack.begin());
}

}
