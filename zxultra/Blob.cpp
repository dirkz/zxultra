#include "Blob.h"

namespace zxultra
{

std::filesystem::path GetBasePath()
{
    std::wstring exeFilename{};
    exeFilename.resize(MAX_PATH);

    DWORD num = GetModuleFileName(NULL, exeFilename.data(), MAX_PATH);
    if (num == 0)
    {
        CheckLastError();
    }

    std::filesystem::path exeFilePath{exeFilename};

    return exeFilePath.parent_path();
}

ComPtr<ID3DBlob> LoadBlob(const std::filesystem::path &filePath)
{
    std::ifstream ifs{filePath.c_str(), std::ios::binary | std::ios::ate};
    if (!ifs)
    {
        throw std::runtime_error{"cannot open blob file"};
    }

    size_t size = ifs.tellg();
    ifs.seekg(0);

    ComPtr<ID3DBlob> blob;
    HR(D3DCreateBlob(size, blob.GetAddressOf()));

    ifs.read(reinterpret_cast<char *>(blob->GetBufferPointer()), size);
    if (!ifs)
    {
        auto numBytesRead = ifs.gcount();
        throw std::runtime_error{"cannot read bytes from blob file"};
    }

    return blob;
}

ComPtr<ID3DBlob> LoadBlob(const std::wstring &relativeBase, const std::wstring &filename)
{
    std::filesystem::path basePath = GetBasePath();
    std::filesystem::path filePath = basePath / relativeBase / filename;

    return LoadBlob(filePath);
}

D3D12_SHADER_BYTECODE ByteCode(ComPtr<ID3DBlob> blob)
{
    return D3D12_SHADER_BYTECODE{blob->GetBufferPointer(), blob->GetBufferSize()};
}

} // namespace zxultra