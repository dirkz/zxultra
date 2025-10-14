add_library(zxultra-core OBJECT
	"Timer.h"
	"Timer.cpp"
	"VertexBuffer.h")

target_include_directories(zxultra-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})