cmake_minimum_required(VERSION 3.24.0)
project("${VIVERNA_TARGET_NAME}_test" LANGUAGES CXX C)

target_sources("${VIVERNA_TARGET_NAME}_test" PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/test_0.cpp"
)