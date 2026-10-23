#include "core/ClientBuilder.h"
#include "core/PeImage.h"
#include "core/Signature.h"
#include <ranges>

namespace ncc
{

namespace
{

constexpr auto kOfficialIp = "37 39 2E 31 31 30 2E"_sig;
constexpr std::size_t kIpSearchStart = 0x025000;

constexpr auto kLauncherCheck = "0F 8C ? ? ? ? 8D 55 DC B8 ? ? ? ? E8 ? ? ? ?"_sig;
constexpr auto kLauncherCheckEnd = "E8 ? ? ? ? 0F 85 ? ? ? ? 33 C0 55 68 ? ? ? ?"_sig;

constexpr auto kLanguageSet = "A1 ? ? ? ? C6 00 00 C6 45 EB 01"_sig;
constexpr std::size_t kLanguageByte = 7;

}

std::filesystem::path clientPath(const ClientSpec& spec)
{
	auto path = spec.folder / spec.name;
	path += ".exe";
	return path;
}

Result<> buildClient(const ClientSpec& spec)
{
	using enum BuildError;

	if (spec.language >= kLanguages.size())
		return std::unexpected(InvalidLanguage);

	auto image = readFile(spec.folder / kSourceClient);
	if (!image)
		return std::unexpected(SourceUnreadable);

	return patchLauncherCheck(*image)
		.and_then([&] { return patchLanguage(*image, spec.language); })
		.and_then([&] { return patchIp(*image, spec.ip); })
		.and_then([&](std::size_t ipOffset) { return patchPort(*image, ipOffset, spec.port); })
		.and_then([&]() -> Result<>
		{
			if (!writeFile(clientPath(spec), *image))
				return std::unexpected(WriteFailed);
			return {};
		});
}

Result<> patchLauncherCheck(MutableByteView image)
{
	const auto start = kLauncherCheck.find(image);
	const auto end = kLauncherCheckEnd.find(image);
	if (!start || !end)
		return std::unexpected(BuildError::LauncherCheckNotFound);

	const auto target = static_cast<std::int64_t>(*end + 11);
	const auto next = static_cast<std::int64_t>(*start + 6);
	store(image, *start + 2, static_cast<std::int32_t>(target - next));
	return {};
}

Result<> patchLanguage(MutableByteView image, std::uint8_t language)
{
	const auto offset = kLanguageSet.find(image);
	if (!offset)
		return std::unexpected(BuildError::LanguageNotFound);
	image[*offset + kLanguageByte] = language;
	return {};
}

Result<std::size_t> patchIp(MutableByteView image, std::string_view ip)
{
	if (ip.empty() || ip.size() > kMaxIpLength)
		return std::unexpected(BuildError::InvalidIp);

	const auto offset = kOfficialIp.find(image, kIpSearchStart);
	if (!offset || *offset < 8 || !fits(image, *offset, kMaxIpLength + 1))
		return std::unexpected(BuildError::IpNotFound);

	const auto slot = image.subspan(*offset, kMaxIpLength + 1);
	const auto [_, written] = std::ranges::copy(ip, slot.begin());
	std::ranges::fill(written, slot.end(), std::uint8_t{ 0 });
	store(image, *offset - 4, static_cast<std::uint32_t>(ip.size()));
	return *offset;
}

Result<> patchPort(MutableByteView image, std::size_t ipOffset, std::uint16_t port)
{
	if (port == 0)
		return std::unexpected(BuildError::InvalidPort);
	const auto ipVA = fileOffsetToVA(image, ipOffset);
	if (!ipVA)
		return std::unexpected(BuildError::PortTableNotFound);

	const auto pointer = toBytes(*ipVA);
	const auto isIpPointer = [&](std::size_t off) { return load<std::uint32_t>(image, off) == ipVA; };
	const auto isPort = [&](std::size_t off)
	{
		const auto value = load<std::uint32_t>(image, off);
		return value && *value > 0 && *value <= 0xFFFF;
	};

	bool patched = false;
	for (auto hit = find(image, pointer); hit;)
	{
		if (*hit % 4 != 0)
		{
			hit = find(image, pointer, *hit + 1);
			continue;
		}

		std::size_t count = 0;
		while (isIpPointer(*hit + count * 4))
			++count;

		const std::size_t ports = *hit + count * 4;
		const auto slots = std::views::iota(ports, ports + count * 4) | std::views::stride(4);
		if (std::ranges::all_of(slots, isPort))
		{
			for (const std::size_t off : slots)
				store(image, off, std::uint32_t{ port });
			patched = true;
		}
		hit = find(image, pointer, ports);
	}
	if (!patched)
		return std::unexpected(BuildError::PortTableNotFound);
	return {};
}

}
