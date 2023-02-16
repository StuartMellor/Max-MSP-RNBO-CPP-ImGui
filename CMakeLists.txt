cmake_minimum_required(VERSION 3.24)

project(rhythmcollector)

find_package(OpenGL)

set(IMGUI_DIR "C:/Users/stuar/Documents/Software/CPP/Libraries/imgui")

add_executable(${PROJECT_NAME}
    WIN32 
    main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    #FOR OPENGL
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    #FOR WINDOWS
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
 )

set(GLEW_INCLUDE_DIR "C:/GL/GLEWbin/include/")
set(GLEW_LIB_BIN "C:/GL/GLEWbin/lib/Debug/x64")

set(GLFW_INCLUDE_DIR "C:/GL/GLFWx64/include")
find_library(GLFW_LIB REQUIRED
    NAMES glfw3
    PATHS "C:/GL/GLFWx64/lib-vc2022/")

find_library(GLEW_LIB
    NAMES glew32 glew32s glew32d glew32ds
    HINTS

    # "C:/GL/GLEWbin/lib/Release/x64"
    "C:/GL/GLEWbin/lib/Debug/x64"
)

# set(GLFW_LIB_BIN "C:/GL/GLFWx64/lib-vc2022/")
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${IMGUI_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${OPENGL_LIBRARIES}
    ${GLFW_LIB}
    ${GLEW_LIB}
)