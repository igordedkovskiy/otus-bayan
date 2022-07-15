cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "2" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(bayan VERSION ${PROJECT_VERSION})

option(WITH_GTEST "Whether to build google test" ON)

configure_file(version.h.in version.h)

add_library(cmp-files-lib
    "input.hpp"
    "input.cpp"
#    "mask.hpp"
#    "mask.cpp"
    "hash.hpp"
    "hash.cpp"
    "File.hpp"
    "File.cpp"
    "filter_files.hpp"
    "filter_files.cpp"
)

set_target_properties(cmp-files-lib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

add_executable(${PROJECT_NAME}
    "main.cpp"
#    "input.hpp"
#    "input.cpp"
#    "mask.hpp"
#    "mask.cpp"
#    "hash.hpp"
#    "hash.cpp"
#    "File.hpp"
#    "File.cpp"
#    "filter_files.hpp"
#    "filter_files.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)





add_library(hash-library
    "hash-library/crc32.h"
    "hash-library/crc32.cpp"
#    "hash-library/digest.cpp"
#    "hash-library/keccak.cpp"
    "hash-library/md5.h"
    "hash-library/md5.cpp"
#    "hash-library/sha1.cpp"
#    "hash-library/sha3.h"
#    "hash-library/sha3.cpp"
#    "hash-library/sha256.cpp"
)

set_target_properties(hash-library PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)


target_include_directories(cmp-files-lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/hash-library")
target_link_libraries(cmp-files-lib PRIVATE hash-library)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/hash-library")
target_link_libraries(${PROJECT_NAME} PRIVATE
    cmp-files-lib
    hash-library
)


#find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
    target_include_directories(cmp-files-lib PRIVATE
        ${Boost_INCLUDE_DIR}
    )
    target_link_libraries(cmp-files-lib PRIVATE
        ${Boost_LIBRARIES}
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
)
else()
    print("Boost was not found")
endif()


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodules: add and update")

        if(WITH_GTEST)
            if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
                execute_process(COMMAND ${GIT_EXECUTABLE}
                                submodule add https://github.com/google/googletest.git googletest
                                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                                RESULT_VARIABLE GIT_SUBMOD_RESULT)
                if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                    message(FATAL_ERROR "git submodule add https://github.com/google/googletest.git googletest failed
                            with ${GIT_SUBMOD_RESULT}, please checkout submodules")
                endif()
            endif()
        endif()

        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed
                    with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()


if(WITH_GTEST)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" "googletest")
    add_executable(test_utility "test_utility.cpp")

    target_include_directories(test_utility PRIVATE
        ${GTEST_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/hash-library"
    )

    set_target_properties(test_utility PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

#    target_compile_definitions(test_print_ip PRIVATE
#        $<$<CONFIG:Debug>:DEBUG_VER>
#        $<$<CONFIG:Release>:RELEASE_VER>
#    )

    target_link_libraries(test_utility PRIVATE
        ${GTEST_BOTH_LIBRARIES}
        gtest
        gtest_main
        cmp-files-lib
        hash-library
    )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4 /await
    )
#    target_compile_options(print_ip_lib PRIVATE
#        /W4 /await
#    )
    if(WITH_GTEST)
        target_compile_options(test_utility PRIVATE
            /W4 /await
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-Wall" "-Wextra" "-Werror" "-pedantic" "-fcoroutines"
    )
#    target_compile_options(print_ip_lib PRIVATE
#        "-Wall" "-Wextra" "-Werror" "-pedantic" "-fcoroutines"
#    )
    if(WITH_GTEST)
        target_compile_options(test_utility PRIVATE
            "-Wall" "-Wextra" "-Werror" "-pedantic" "-fcoroutines"
        )
    endif()
endif()

#if(WIN32)
#    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#elseif(UNIX)
#    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
#endif()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT divmy@yandex.ru)
include(CPack)

if(WITH_GTEST)
    include(CTest)
    enable_testing()
    list(APPEND CMAKE_CTEST_ARGUMENTS "--rerun-failed" "--output-on-failure")
    add_test(test_utility test_utility)# "-i" "../qwerty")
endif()
