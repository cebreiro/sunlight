add_executable(cebreiro_sl_generator)

target_include_directories(cebreiro_sl_generator PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(cebreiro_sl_generator PRIVATE ${LUA_LIBRARIES})
target_link_libraries(cebreiro_sl_generator PRIVATE sol2)

target_link_libraries(cebreiro_sl_generator PRIVATE
    cebreiro_sl_pch
    cebreiro_sl_data
)

target_precompile_headers(cebreiro_sl_generator REUSE_FROM cebreiro_sl_pch)

file(GLOB_RECURSE CEBREIRO_SL_GENERATOR_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

file(GLOB_RECURSE CEBREIRO_SL_GENERATOR_PROTOBUF_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

set_source_files_properties(${CEBREIRO_SL_GENERATOR_PROTOBUF_SRC_FILES} PROPERTIES COMPILE_FLAGS "/wd4100")

target_sources(cebreiro_sl_generator PRIVATE ${CEBREIRO_SL_GENERATOR_SRC_FILES} ${CEBREIRO_SL_GENERATOR_PROTOBUF_SRC_FILES})

set_target_properties(cebreiro_sl_generator PROPERTIES OUTPUT_NAME "SunlightGenerator")
