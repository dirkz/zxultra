#include "FrameData.h"

namespace zxultra
{

FrameData::FrameData(ID3D12Device *device)
    : m_cbProjection{device, 1}, m_cbView{device, 1}, m_cbModel{device, 1}
{
}

} // namespace zxultra