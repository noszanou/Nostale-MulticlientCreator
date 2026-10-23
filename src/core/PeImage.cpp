#include "core/PeImage.h"
#include <ranges>

namespace ncc
{

namespace
{

constexpr std::size_t kPeOffsetField = 0x3C;
constexpr std::uint32_t kPeSignature = 0x00004550;
constexpr std::uint16_t kPe32Magic = 0x10B;
constexpr std::size_t kFileHeaderSize = 24;
constexpr std::size_t kSectionHeaderSize = 40;

}

std::optional<std::uint32_t> fileOffsetToVA(ByteView image, std::size_t offset)
{
	const auto peOffset = load<std::uint32_t>(image, kPeOffsetField);
	if (!peOffset || load<std::uint32_t>(image, *peOffset) != kPeSignature)
		return std::nullopt;

	const std::size_t pe = *peOffset;
	const std::size_t optionalHeader = pe + kFileHeaderSize;
	const auto sectionCount = load<std::uint16_t>(image, pe + 6);
	const auto optionalHeaderSize = load<std::uint16_t>(image, pe + 20);
	const auto imageBase = load<std::uint32_t>(image, optionalHeader + 28);
	if (!sectionCount || !optionalHeaderSize || !imageBase || load<std::uint16_t>(image, optionalHeader) != kPe32Magic)
		return std::nullopt;

	const std::size_t sectionTable = optionalHeader + *optionalHeaderSize;
	for (const std::size_t i : std::views::iota(std::size_t{ 0 }, std::size_t{ *sectionCount }))
	{
		const std::size_t section = sectionTable + i * kSectionHeaderSize;
		const auto virtualAddress = load<std::uint32_t>(image, section + 12);
		const auto rawSize = load<std::uint32_t>(image, section + 16);
		const auto rawPointer = load<std::uint32_t>(image, section + 20);
		if (!virtualAddress || !rawSize || !rawPointer)
			return std::nullopt;
		if (offset >= *rawPointer && offset - *rawPointer < *rawSize)
			return *imageBase + *virtualAddress + static_cast<std::uint32_t>(offset - *rawPointer);
	}
	return std::nullopt;
}

}
