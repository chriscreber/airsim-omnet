cmake_minimum_required(VERSION 3.5.0)
project(AirSim)

set(AIRSIM_ROOT $ENV{HOME}/AirSim)
set (CMAKE_CXX_STANDARD 11)

add_subdirectory("${AIRSIM_ROOT}/cmake/rpclib_wrapper" ./lib/rpc)
add_subdirectory("${AIRSIM_ROOT}/cmake/AirLib" ./lib/airlib)
add_subdirectory("${AIRSIM_ROOT}/cmake/MavLinkCom" ./lib/mavlinkcom)

project(Hello2Car)
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
)
# link_directories(
    # ${AIRSIM_ROOT}/AirLib/lib
    # ${AIRSIM_ROOT}/AirLib/deps/rpclib/lib
    # ${AIRSIM_ROOT}/build_debug/output/lib
    # )

# AddExecutableSource()
add_executable(${PROJECT_NAME} main.cpp)

			
CommonTargetLink()
target_link_libraries(${PROJECT_NAME} AirLib)
target_link_libraries(${PROJECT_NAME} ${RPC_LIB})
