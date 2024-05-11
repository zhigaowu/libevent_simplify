cmake_minimum_required(VERSION 3.10)

PROJECT(test_cases VERSION 1.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 设置 CMAKE_BUILD_TYPE
IF ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

#设置Release版本的编译参数
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# 编译器参数
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    ADD_COMPILE_OPTIONS(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
    IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        ADD_COMPILE_OPTIONS(-g)
    ENDIF()
    IF ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 9)
        SET(GCC8OL TRUE)
    ENDIF()
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # note: not working for current version because C++17 support is poor
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -inline-forceinline")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    ADD_COMPILE_OPTIONS(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
    IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        ADD_COMPILE_OPTIONS(-g)
    ELSEIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        # C99 是允许使用的，但 Clang 依然可能会给出警告，因此屏蔽
        ADD_COMPILE_OPTIONS(-Wno-c99-extensions)
    ENDIF()
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    ADD_COMPILE_OPTIONS(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
    IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        ADD_COMPILE_OPTIONS(-g)
    ELSEIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        # C99 是允许使用的，但 Clang 依然可能会给出警告，因此屏蔽
        ADD_COMPILE_OPTIONS(-Wno-c99-extensions)
    ENDIF()
ELSE()
    MESSAGE(WARNING "Unsupported CXX compiler ${CMAKE_CXX_COMPILER_ID}")
ENDIF()

# 通用外部库依赖导入函数
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/PrintFlags.cmake)

# 打印项目信息
MESSAGE(STATUS "===== ${PROJECT_NAME} ====")
MESSAGE(STATUS "--------------------------------------------------")
MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 打印编译标志
PRINT_CPLUS_FLAGS()

# libevent
FIND_PACKAGE(Libevent REQUIRED COMPONENTS core pthreads)
PRINT_LIB_INFO("Libevent" "${LIBEVENT_INCLUDE_DIRS}" "${LIBEVENT_LIBRARIES}")

# OpenSSL
FIND_PACKAGE(OpenSSL REQUIRED)

# glog
SET(Glog_ROOT_DIR /home/chico/glog/glog)
FIND_PACKAGE(Glog REQUIRED)
PRINT_LIB_INFO("Glog" "${Glog_INCLUDE_DIRS}" "${Glog_LIBRARIES}")
#FIND_PACKAGE(Gflags REQUIRED)
#PRINT_LIB_INFO("Gflags" "${Gflags_INCLUDE_DIRS}" "${Gflags_LIBRARIES}")

# test1
IF (WIN32)
    set(test1_DIR C:/msys64/home/ZhiGao.Wu/gtest1/lib/cmake/test1)
    LIST(APPEND CMAKE_PREFIX_PATH /home/ZhiGao.Wu/gtest1)
ELSE()
    LIST(APPEND CMAKE_PREFIX_PATH /home/chico/gtest1)
ENDIF()
FIND_PACKAGE(test1 REQUIRED)
MESSAGE(STATUS "${test1_INCLUDE_DIRS}")
# test2
IF (WIN32)
    set(test2_DIR C:/msys64/home/ZhiGao.Wu/gtest2/lib/cmake/test2)
    LIST(APPEND CMAKE_PREFIX_PATH /home/ZhiGao.Wu/gtest2)
ELSE()
    LIST(APPEND CMAKE_PREFIX_PATH /home/chico/gtest2)
ENDIF()
FIND_PACKAGE(test2 REQUIRED)
MESSAGE(STATUS "${test2_INCLUDE_DIRS}")

# windows 下额外的库文件
IF (WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .dll .dll.a .a)
    FIND_LIBRARY(WSOCk "wsock32")
    FIND_LIBRARY(IPHLPAPI "iphlpapi")

    PRINT_LIB_INFO("Win32lib" "" "${WSOCk} ${IPHLPAPI}")
ENDIF()

# 打包
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

IF (NOT "${PACKAGE_NAME_SUFFIX}" STREQUAL "")
    MESSAGE(STATUS "Append '${PACKAGE_NAME_SUFFIX}' to the package file name")
    SET(CPACK_ARCHIVE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${PACKAGE_NAME_SUFFIX}")
ENDIF()

# 打印空行，结束项目信息
MESSAGE(STATUS "--------------------------------------------------")

INCLUDE(CPack)

# 将所有 src/*.cpp 文件列出来
FILE(GLOB PROJECT_CXX_SOURCE ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/tests/*.cpp)

# 所有的源文件+头文件需要在这个地方添加
ADD_EXECUTABLE(test_cases ${PROJECT_CXX_SOURCE})

# 库依赖
IF (WIN32)
    TARGET_INCLUDE_DIRECTORIES(test_cases PRIVATE ${LIBEVENT_INCLUDE_DIRS} ${Glog_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src C:/msys64/home/ZhiGao.Wu/gtest1/include C:/msys64/home/ZhiGao.Wu/gtest2/include)
    TARGET_LINK_DIRECTORIES(test_cases PRIVATE C:/msys64/home/ZhiGao.Wu/gtest1/lib C:/msys64/home/ZhiGao.Wu/gtest2/lib)
ELSE()
    TARGET_INCLUDE_DIRECTORIES(test_cases PRIVATE ${LIBEVENT_INCLUDE_DIRS} ${Glog_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src /home/chico/gtest1/include /home/chico/gtest2/include)
    TARGET_LINK_DIRECTORIES(test_cases PRIVATE /home/chico/gtest1/lib /home/chico/gtest2/lib)
ENDIF()
TARGET_LINK_LIBRARIES(test_cases PRIVATE ${LIBEVENT_LIBRARIES})
TARGET_LINK_LIBRARIES(test_cases PRIVATE OpenSSL::SSL OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(test_cases PRIVATE ${Glog_LIBRARIES})
TARGET_LINK_LIBRARIES(test_cases PRIVATE test1 test2)

IF (WIN32)
    TARGET_LINK_LIBRARIES(test_cases PRIVATE wsock32 ws2_32 iphlpapi)
ENDIF()

install(TARGETS test_cases
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install COMPONENT test_cases)

