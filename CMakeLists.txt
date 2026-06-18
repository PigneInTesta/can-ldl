cmake_minimum_required(VERSION 3.10)
project(can_logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

include_directories(include)

set(COMMON_SRC src/can_config.cpp)

# Sender
add_executable(can_logger
    ${COMMON_SRC}
    src/can_sender.cpp
    src/can_reader.cpp
    src/main.cpp
)
target_link_libraries(can_logger pthread)

# Receiver
add_executable(can_receiver
    ${COMMON_SRC}
    src/can_receiver.cpp
    src/main_receiver.cpp
)
target_link_libraries(can_receiver pthread)