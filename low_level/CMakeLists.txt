cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "low_level")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(${PROJECT_NAME})

option(CPR_ENABLE_SSL "" OFF)

include(FetchContent)
FetchContent_Declare(
  json
  DOWNLOAD_EXTRACT_TIMESTAMP OFF
  URL "https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz")
FetchContent_Declare(
  cpr
  DOWNLOAD_EXTRACT_TIMESTAMP OFF
  URL "https://github.com/libcpr/cpr/archive/refs/tags/1.11.0.tar.gz")
FetchContent_MakeAvailable(json cpr)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json cpr::cpr)
