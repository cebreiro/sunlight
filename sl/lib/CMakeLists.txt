add_library(cebreiro_sl_lib)

target_link_libraries(cebreiro_sl_lib PUBLIC
    cebreiro_sl_pch
)

file(GLOB_RECURSE CEBREIRO_SL_LIB_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(cebreiro_sl_lib PRIVATE ${CEBREIRO_SL_LIB_SRC_FILES})
