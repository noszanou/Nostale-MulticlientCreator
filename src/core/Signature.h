#pragma once
#include "core/Binary.h"
#include <string_view>

namespace ncc
{

template<std::size_t N>
struct FixedString
{
	char chars[N]{};

	consteval FixedString(const char (&text)[N]) { std::ranges::copy(text, chars); }
	[[nodiscard]] consteval std::string_view view() const { return { chars, N - 1 }; }
};

template<std::size_t N>
struct Signature
{
	static_assert(N > 0, "empty signature");

	std::array<std::optional<std::uint8_t>, N> bytes{};

	[[nodiscard]] static constexpr std::size_t size() noexcept { return N; }

	[[nodiscard]] std::optional<std::size_t> find(ByteView data, std::size_t from = 0) const
	{
		if (from > data.size())
			return std::nullopt;
		const auto haystack = data.subspan(from);
		const auto hit = std::ranges::search(haystack, bytes,
			[](std::uint8_t byte, const std::optional<std::uint8_t>& expected) { return !expected || *expected == byte; });
		if (hit.empty())
			return std::nullopt;
		return from + static_cast<std::size_t>(hit.begin() - haystack.begin());
	}
};

namespace detail
{

consteval std::size_t tokenCount(std::string_view text)
{
	std::size_t count = 0;
	bool inToken = false;
	for (char c : text)
	{
		if (c == ' ')
			inToken = false;
		else if (!inToken)
		{
			inToken = true;
			++count;
		}
	}
	return count;
}

consteval std::uint8_t hexDigit(char c)
{
	if (c >= '0' && c <= '9')
		return static_cast<std::uint8_t>(c - '0');
	if (c >= 'A' && c <= 'F')
		return static_cast<std::uint8_t>(c - 'A' + 10);
	if (c >= 'a' && c <= 'f')
		return static_cast<std::uint8_t>(c - 'a' + 10);
	throw "invalid hex digit in signature";
}

}

inline namespace literals
{

template<FixedString S>
consteval auto operator""_sig()
{
	constexpr std::string_view text = S.view();
	Signature<detail::tokenCount(text)> sig;

	std::size_t index = 0;
	for (std::size_t pos = 0; pos < text.size();)
	{
		if (text[pos] == ' ')
		{
			++pos;
			continue;
		}
		const std::size_t end = std::min(text.find(' ', pos), text.size());
		const std::string_view token = text.substr(pos, end - pos);
		if (token == "?" || token == "??")
			sig.bytes[index++] = std::nullopt;
		else if (token.size() == 2)
			sig.bytes[index++] = static_cast<std::uint8_t>(detail::hexDigit(token[0]) << 4 | detail::hexDigit(token[1]));
		else
			throw "signature tokens must be two hex digits or '?'";
		pos = end;
	}
	return sig;
}

}

}
