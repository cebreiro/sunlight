add_executable(cebreiro_sl_tool_sox_code_generator)

target_link_libraries(cebreiro_sl_tool_sox_code_generator PRIVATE
    cebreiro_sl_data
)

target_precompile_headers(cebreiro_sl_tool_sox_code_generator REUSE_FROM cebreiro_sl_pch)

file(GLOB_RECURSE CEBREIRO_SL_TOOL_SOX_CODE_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(cebreiro_sl_tool_sox_code_generator PRIVATE ${CEBREIRO_SL_TOOL_SOX_CODE_GENERATOR_SRC_FILES})
