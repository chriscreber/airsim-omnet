cmake_minimum_required(VERSION 3.5.0)
project(AirSim)

set(AIRSIM_ROOT $ENV{HOME}/AirSim)
set(UNREAL_ROOT $ENV{HOME}/UnrealEngine/Engine/Source/Runtime/Core/Public)
set (CMAKE_CXX_STANDARD 11)

add_subdirectory("${AIRSIM_ROOT}/cmake/rpclib_wrapper" ./lib/rpc)
add_subdirectory("${AIRSIM_ROOT}/cmake/AirLib" ./lib/airlib)
add_subdirectory("${AIRSIM_ROOT}/cmake/MavLinkCom" ./lib/mavlinkcom)

project(Car)
Set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -ljsoncpp -pthread")
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

## trying to add stuff for jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})


## Adding threads
find_package (Threads)


add_executable(${PROJECT_NAME}4001 car4001.cpp rotationUtility.cpp asyncSocketClient.cpp packet.cpp collisionUtility.cpp)
add_executable(${PROJECT_NAME}4002 car4002.cpp rotationUtility.cpp asyncSocketClient.cpp packet.cpp collisionUtility.cpp)
add_executable(${PROJECT_NAME}Test carTest.cpp rotationUtility.cpp asyncSocketClient.cpp packet.cpp collisionUtility.cpp)

# CommonTargetLink()
target_link_libraries(${PROJECT_NAME}4001 ${CMAKE_THREAD_LIBS_INIT} ${JSONCPP_LIBRARIES})
target_link_libraries(${PROJECT_NAME}4002 ${CMAKE_THREAD_LIBS_INIT} ${JSONCPP_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Test ${CMAKE_THREAD_LIBS_INIT} ${JSONCPP_LIBRARIES})

target_link_libraries(${PROJECT_NAME}4001 AirLib)
target_link_libraries(${PROJECT_NAME}4001 ${RPC_LIB})
target_link_libraries(${PROJECT_NAME}4002 AirLib)
target_link_libraries(${PROJECT_NAME}4002 ${RPC_LIB})
target_link_libraries(${PROJECT_NAME}Test AirLib)
target_link_libraries(${PROJECT_NAME}Test ${RPC_LIB})
