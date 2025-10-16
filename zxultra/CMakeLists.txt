add_library(zxultra OBJECT
	"AppWindow.h"
	"AppWindow.cpp"
	"Error.h"
	"Fence.h"
	"Fence.cpp"
	"Swapchain.h"
	"Swapchain.cpp"
	"DescriptorHandleSizes.h"
	"CommandList.h"
	"CommandList.cpp"
	"DefaultBufferCreator.h"
	"DefaultBufferCreator.cpp"
	"VertexWithColor.h"
	"Constants.h")

target_precompile_headers(zxultra INTERFACE stdafx.h)

target_include_directories(zxultra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(core)
target_link_libraries(zxultra PUBLIC zxultra-core)

target_include_directories(zxultra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../DirectX-Headers/include/directx)
target_include_directories(zxultra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../DirectXMath/Inc)

target_link_libraries(zxultra PUBLIC d3d12.lib dxgi.lib dxguid.lib D3DCompiler.lib)