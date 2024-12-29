add_library(cebreiro_sl_pch)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(cebreiro_sl_pch PUBLIC
    cebreiro_shared
    nlohmann_json
    nlohmann_json::nlohmann_json
    protobuf::libprotobuf
)

target_precompile_headers(cebreiro_sl_pch PUBLIC
    <cstdint>
    <cassert>

    <bitset>
    <numbers>
    <random>
    <unordered_map>
    <unordered_set>
    <chrono>

    <memory>
    <new>

    <vector>
    <list>
    <deque>
    <map>
    <set>
    <unordered_map>
    <unordered_set>
    <queue>
    <stack>

    <atomic>
    <mutex>
    <thread>

    <utility>
    <type_traits>

    <random>

    <optional>
    <variant>
    <any>
    <filesystem>

    <functional>
    <algorithm>
    <numeric>

    <format>
    <sstream>
    <fstream>
    <iostream>

    <source_location>
    <ranges>
    <concepts>
    <coroutine>

    <expected>
    <stacktrace>

    <nlohmann/json.hpp>

    <Eigen/Dense>
    <Eigen/Geometry>

    <fmt/core.h>
    <fmt/format.h>

    ${PROJECT_SOURCE_DIR}/shared/app/app_log.h
    ${PROJECT_SOURCE_DIR}/shared/execution/channel/channel.h
    ${PROJECT_SOURCE_DIR}/shared/execution/channel/async_enumerable.h
    ${PROJECT_SOURCE_DIR}/shared/execution/executor/executor.hpp
    ${PROJECT_SOURCE_DIR}/shared/execution/executor/static_thread_pool.h
    ${PROJECT_SOURCE_DIR}/shared/execution/future/future.hpp
    ${PROJECT_SOURCE_DIR}/shared/log/log_service_interface.h
    ${PROJECT_SOURCE_DIR}/shared/service/service_interface.h
    ${PROJECT_SOURCE_DIR}/shared/service/service_locator_log.h
    ${PROJECT_SOURCE_DIR}/shared/type/borrowed.h
    ${PROJECT_SOURCE_DIR}/shared/type/enum_class.h
    ${PROJECT_SOURCE_DIR}/shared/type/not_null_pointer.h
    ${PROJECT_SOURCE_DIR}/shared/type/runtime_type_id.h
    ${PROJECT_SOURCE_DIR}/shared/type/value_type.h
    ${PROJECT_SOURCE_DIR}/shared/type/concept.h
    ${PROJECT_SOURCE_DIR}/shared/network/buffer/buffer_reader.h
    ${PROJECT_SOURCE_DIR}/shared/network/buffer/buffer_writer.h
    ${PROJECT_SOURCE_DIR}/shared/network/buffer/packet_writer.h
)

target_sources(cebreiro_sl_pch PRIVATE dummy.cpp)
