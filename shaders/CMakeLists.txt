# All vertex/fragmentshaders with VS, PS entry points in a single file.
set (SHADERS PositionColor.hlsl)

add_custom_target(shaders)

foreach(SHADER ${SHADERS})
	set (SHADER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})

	set (SHADER_OUTPUT_FILENAME_VS ${SHADER}_VS.bin)
	set (SHADER_OUTPUT_FILENAME_PS ${SHADER}_PS.bin)

	set (SHADER_TARGET_VS ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_OUTPUT_FILENAME_VS})
	set (SHADER_TARGET_PS ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_OUTPUT_FILENAME_PS})

	add_custom_command(
		OUTPUT ${SHADER_TARGET_VS}
		DEPENDS ${SHADER_SRC}
		COMMAND dxc.exe -T vs_6_0 -E VS -Fo ${SHADER_TARGET_PS} ${SHADER_SRC}
	)

	target_sources(shaders PRIVATE ${SHADER_TARGET_VS})
endforeach()
