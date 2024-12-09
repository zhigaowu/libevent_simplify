cmake_minimum_required(VERSION 3.22)

PROJECT(client_tests VERSION 1.0.0 LANGUAGES CXX)

# 打印项目信息
MESSAGE(STATUS "===== ${PROJECT_NAME} ====")
MESSAGE(STATUS "--------------------------------------------------")
MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 打印编译标志
PRINT_CPLUS_FLAGS()

# 打印空行，结束项目信息
MESSAGE(STATUS "--------------------------------------------------")

# 将所有 src/*.cpp 文件列出来
FILE(GLOB_RECURSE 
    PROJECT_CXX_SOURCE 
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 所有的源文件+头文件需要在这个地方添加
ADD_EXECUTABLE(client_tests ${PROJECT_CXX_SOURCE})

TARGET_INCLUDE_DIRECTORIES(client_tests
    PRIVATE 
    ${LIBEVENT_INCLUDE_DIRS} 
    ${libssl_INCLUDE_DIRS} 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests)

TARGET_LINK_LIBRARIES(client_tests
    PRIVATE 
    ${LIBEVENT_LIBRARIES}
    ${libssl_LIBRARIES}
    crypto
    pthread)

IF (WIN32)
    TARGET_LINK_LIBRARIES(client_tests PRIVATE wsock32 ws2_32 iphlpapi)
ENDIF()

install(TARGETS client_tests
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install)

