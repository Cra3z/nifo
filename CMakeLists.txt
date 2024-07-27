cmake_minimum_required(VERSION 3.26)

project(nifo)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/permissive->")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

list(APPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")

find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT REQUIRED)
find_package(TBB REQUIRED)
find_package(
    Qt6
    COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
    OpenGLWidgets
    REQUIRED
)

file(GLOB_RECURSE NIFO_CORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/nifo/core/*.h")
file(GLOB_RECURSE NIFO_CORE_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/nifo/core/*.cpp")
add_library(
    nifo_core
    STATIC
    ${NIFO_CORE_HEADERS}
    ${NIFO_CORE_SRCS}
)

file(GLOB_RECURSE NIFO_GUI_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/nifo/ui/*.h")
file(GLOB_RECURSE NIFO_GUI_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/nifo/ui/*.cpp")
add_library(
    nifo_gui
    ${NIFO_GUI_HEADERS}
    ${NIFO_GUI_SRCS}
)

add_executable(
    nifo
    WIN32
    src/nifo/main.cpp
)

target_link_libraries(
    nifo_core
    PUBLIC
    assimp::assimp
    spdlog::spdlog
    glm::glm
    EnTT::EnTT
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    TBB::tbb
)

target_link_libraries(
    nifo_gui
    PUBLIC
    nifo_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
)

target_link_libraries(
    nifo
    PRIVATE
    nifo_gui
)

foreach(T IN ITEMS nifo_core nifo_gui nifo)
    target_include_directories(
        ${T}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    )
endforeach()

install(
    TARGETS nifo
    RUNTIME DESTINATION "nifo"
)

set(ASSETS_DIR "assets" CACHE PATH "the directory name of assets")

foreach(DIR IN ITEMS models shaders textures)
    file(
        COPY ${CMAKE_SOURCE_DIR}/assets/${DIR}
        DESTINATION ${CMAKE_BINARY_DIR}/${ASSETS_DIR}
    )
    install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/assets/${DIR}
        DESTINATION nifo/${ASSETS_DIR}
    )
endforeach()


set(ASSETS_DIR "R\"(${ASSETS_DIR}/)\"")

target_compile_definitions(
    nifo_core
    PUBLIC
    -DASSETS_DIR=${ASSETS_DIR}
    PUBLIC
    -DNIFO_GUI_USE_QT
)

macro(nifo_find_files NAME GLOB_EXPR)
    file(GLOB_RECURSE _ABSOLUTE_PATH_FILES CONFIGURE_DEPENDS "${GLOB_EXPR}")
    set(${NAME})
    foreach(_FILE IN LISTS _ABSOLUTE_PATH_FILES )
        file(RELATIVE_PATH _RELATIVE_PATH_FILE "${CMAKE_SOURCE_DIR}" ${_FILE})
        list(APPEND ${NAME} ${_RELATIVE_PATH_FILE})
    endforeach()
endmacro()

nifo_find_files(NIFO_QSS_FILES "styles/*.qss")
qt_add_resources(
    ${PROJECT_NAME} "styles"
    PREFIX "/"
    FILES ${NIFO_QSS_FILES}
)

nifo_find_files(NIFO_IMG_FILES "assets/images/*.*")
qt_add_resources(
    ${PROJECT_NAME} "images"
    PREFIX "/"
    BASE "assets"
    FILES ${NIFO_IMG_FILES}
)

############################################[FOR QT]##############################################

if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)
endif()

##################################################################################################