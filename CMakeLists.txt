cmake_minimum_required(VERSION 3.14)

project(includes_scanner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.70.0 REQUIRED program_options regex)
set(Boost_NO_WARN_NEW_VERSIONS 1)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(includes_scanner ${SOURCES} ${HEADERS})

target_link_libraries(includes_scanner ${Boost_LIBRARIES})

# some msvc magic
if (WIN32)
    add_compile_definitions(BOOST_ALL_DYN_LINK)
    set_target_properties(includes_scanner PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:includes_scanner>")
    #add_custom_command(TARGET includes_scanner POST_BUILD
    #  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_RUNTIME_DLLS:includes_scanner>" "$<TARGET_FILE_DIR:includes_scanner>"
    #  COMMENT "Daploying libraris"
    #)
endif()