cmake_minimum_required(VERSION 3.5.0)
project(AirSim)

set(AIRSIM_ROOT $ENV{HOME}/AirSim)
set(UNREAL_ROOT $ENV{HOME}/UnrealEngine/Engine/Source/Runtime/Core/Public)
set (CMAKE_CXX_STANDARD 11)

add_subdirectory("${AIRSIM_ROOT}/cmake/rpclib_wrapper" ./lib/rpc)
add_subdirectory("${AIRSIM_ROOT}/cmake/AirLib" ./lib/airlib)
add_subdirectory("${AIRSIM_ROOT}/cmake/MavLinkCom" ./lib/mavlinkcom)

project(Car)
LIST(APPEND CMAKE_MODULE_PATH "${AIRSIM_ROOT}/cmake/cmake-modules")
INCLUDE("${AIRSIM_ROOT}/cmake/cmake-modules/CommonSetup.cmake")
CommonSetup()

IncludeEigen()

SetupConsoleBuild()

## Specify additional locations of header files
include_directories(
    ${AIRSIM_ROOT}/AirLib/include
    ${RPC_LIB_INCLUDES}
    ${AIRSIM_ROOT}/MavLinkCom/include
    ${AIRSIM_ROOT}/MavLinkCom/common_utils
    ${PROJECT_SOURCE_DIR}
    ${UNREAL_ROOT}
    ${UNREAL_ROOT}/Math
    ${UNREAL_ROOT}/Containers
    ${UNREAL_ROOT}/Templates
    ${UNREAL_ROOT}/HAL
)

add_executable(${PROJECT_NAME}4001 car4001.cpp asyncSocketClient.cpp packet.cpp)
add_executable(${PROJECT_NAME}4002 car4002.cpp asyncSocketClient.cpp packet.cpp)
add_executable(${PROJECT_NAME}Test carTest.cpp asyncSocketClient.cpp packet.cpp)

# CommonTargetLink()
target_link_libraries(${PROJECT_NAME}4001 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}4002 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}Test ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(${PROJECT_NAME}4001 AirLib)
target_link_libraries(${PROJECT_NAME}4001 ${RPC_LIB})
target_link_libraries(${PROJECT_NAME}4002 AirLib)
target_link_libraries(${PROJECT_NAME}4002 ${RPC_LIB})
target_link_libraries(${PROJECT_NAME}Test AirLib)
target_link_libraries(${PROJECT_NAME}Test ${RPC_LIB})
