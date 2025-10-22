#pragma once

#include "stdafx.h"

namespace zxultra
{

std::filesystem::path GetBasePath();

ComPtr<ID3DBlob> LoadBlob(const std::filesystem::path &filePath);
ComPtr<ID3DBlob> LoadBlob(const std::wstring &relativeBase, const std::wstring &filename);

} // namespace zxultra
