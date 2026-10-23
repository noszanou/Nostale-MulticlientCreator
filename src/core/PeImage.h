#pragma once
#include "core/Binary.h"

namespace ncc
{

[[nodiscard]] std::optional<std::uint32_t> fileOffsetToVA(ByteView image, std::size_t offset);

}
