add_library(cebreiro_shared)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS charconv system filesystem locale)
find_package(Eigen3 CONFIG REQUIRED)

target_link_libraries(cebreiro_shared PUBLIC
	Boost::boost
	TBB::tbb
    TBB::tbbmalloc
	spdlog::spdlog
    pugixml::shared pugixml::pugixml
    OpenSSL::Crypto
	OpenSSL::SSL
    Boost::charconv
    Boost::system
    Boost::filesystem
    Boost::locale
    Eigen3::Eigen
    fmt::fmt
    unofficial::argon2::libargon2
)

file(GLOB_RECURSE CEBREIRO_SHARED_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(cebreiro_shared PRIVATE ${CEBREIRO_SHARED_SRC_FILES})
