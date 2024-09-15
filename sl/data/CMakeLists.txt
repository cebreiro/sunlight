add_library(cebreiro_sl_data)

target_link_libraries(cebreiro_sl_data PUBLIC
    cebreiro_sl_pch
    cebreiro_sl_lib
)

file(GLOB_RECURSE CEBREIRO_SL_DATA_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(cebreiro_sl_data PRIVATE ${CEBREIRO_SL_DATA_SRC_FILES})
