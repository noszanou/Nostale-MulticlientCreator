#pragma once
#include "core/Binary.h"
#include <expected>
#include <string>
#include <string_view>

namespace ncc
{

inline constexpr std::string_view kSourceClient = "NostaleClientX.exe";
inline constexpr std::size_t kMaxIpLength = 15;

inline constexpr std::array<std::string_view, 9> kLanguages = {
	"English", "Deutsch", "Français", "Italiano", "Polski", "Español", "Čeština", "Русский", "Türkçe"
};

struct ClientSpec
{
	std::filesystem::path folder;
	std::filesystem::path name;
	std::string ip;
	std::uint16_t port = 4000;
	std::uint8_t language = 0;
};

enum class BuildError
{
	InvalidIp,
	InvalidPort,
	InvalidLanguage,
	SourceUnreadable,
	LauncherCheckNotFound,
	LanguageNotFound,
	IpNotFound,
	PortTableNotFound,
	WriteFailed,
};

template<class T = void>
using Result = std::expected<T, BuildError>;

[[nodiscard]] std::filesystem::path clientPath(const ClientSpec& spec);

[[nodiscard]] Result<> buildClient(const ClientSpec& spec);

[[nodiscard]] Result<> patchLauncherCheck(MutableByteView image);
[[nodiscard]] Result<> patchLanguage(MutableByteView image, std::uint8_t language);
[[nodiscard]] Result<std::size_t> patchIp(MutableByteView image, std::string_view ip);
[[nodiscard]] Result<> patchPort(MutableByteView image, std::size_t ipOffset, std::uint16_t port);

}
