add_executable(cebreiro_sl_server_emulator)

target_link_libraries(cebreiro_sl_server_emulator PRIVATE
    cebreiro_sl_pch
    cebreiro_sl_data
)

target_precompile_headers(cebreiro_sl_server_emulator REUSE_FROM cebreiro_sl_pch)

file(GLOB_RECURSE CEBREIRO_SL_SERVER_EMULATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(cebreiro_sl_server_emulator PRIVATE ${CEBREIRO_SL_SERVER_EMULATOR_SRC_FILES})
