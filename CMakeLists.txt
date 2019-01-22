cmake_minimum_required(VERSION 3.2)
project(A2D)

set(CMAKE_CXX_STANDARD 11)

# Shader init template file (here generated shader strings will be stored)
set(SHADERS_TEMPLATE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/a2d/graphics/shaders_builtin.cpp.in")
set(SHADERS_GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders_builtin.cpp")

set(A2D_SOURCES
        src/a2d/input/input.cpp
        src/a2d/core/engine.cpp
        src/a2d/core/object2d.cpp
        src/a2d/core/component.cpp
        src/a2d/core/components/sprite.cpp
        src/a2d/core/components/camera.cpp
        src/a2d/core/components/pixel_camera.cpp
        src/a2d/graphics/bitmap_font.cpp
        src/a2d/graphics/drawable.cpp
        src/a2d/audio/audio_clip.cpp
        src/a2d/audio/audio.cpp
        src/a2d/core/components/audio_source.cpp
        src/a2d/core/components/text.cpp
        src/a2d/core/components/animator.cpp
        src/a2d/filesystem/filesystem.cpp
        src/a2d/graphics/shader.cpp
        src/a2d/graphics/renderer.cpp
        src/a2d/graphics/texture_region.cpp
        src/a2d/graphics/texture_buffer.cpp
        src/a2d/graphics/texture.cpp
        src/a2d/graphics/sprite_batch.cpp
        src/a2d/core/resources.cpp
        src/a2d/graphics/animation.cpp
        ${SHADERS_GENERATED_FILE})

set(A2D_HEADERS
        src/a2d/a2d.hpp
        src/a2d/math/math.hpp
        src/a2d/math/vector.hpp
        src/a2d/math/matrix.hpp
        src/a2d/math/math_defines.hpp
        src/a2d/input/input.hpp
        src/a2d/core/engine.hpp
        src/a2d/core/component.hpp
        src/a2d/core/object2d.hpp
        src/a2d/core/ref_counter.hpp
        src/a2d/core/intrusive_ptr.hpp
        src/a2d/core/macro.hpp
        src/a2d/core/components/sprite.hpp
        src/a2d/core/components/camera.hpp
        src/a2d/core/components/animator.hpp
        src/a2d/core/components/events_test.hpp
        src/a2d/core/components/pixel_camera.hpp
        src/a2d/audio/audio.hpp
        src/a2d/graphics/drawable.hpp
        src/a2d/core/components/text.hpp
        src/a2d/graphics/bitmap_font.hpp
        src/a2d/core/components/audio_source.hpp
        src/a2d/audio/audio_clip.hpp
        src/a2d/physics/physics.hpp
        src/a2d/core/components/physics/physics_body.hpp
        src/a2d/core/components/line.hpp
        src/a2d/core/components/physics/physics_collider.hpp
        src/a2d/core/components/physics/circle_collider.hpp
        src/a2d/filesystem/filesystem.hpp
        src/a2d/graphics/gl.hpp
        src/a2d/graphics/sprite_batch.hpp
        src/a2d/graphics/texture_region.hpp
        src/a2d/graphics/shader.hpp
        src/a2d/graphics/texture_buffer.hpp
        src/a2d/graphics/texture.hpp
        src/a2d/graphics/renderer.hpp
        src/a2d/core/resources.hpp
        src/a2d/graphics/animation.h
        src/a2d/core/command.hpp
        src/a2d/core/commands/lambda_command.h)

# A2D library target
add_library(a2d_engine_library STATIC ${A2D_SOURCES} ${A2D_HEADERS})
target_include_directories(a2d_engine_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Prepare variables
if (ANDROID OR IOS)
    if (ANDROID)
        set(TARGET_ANDROID TRUE)
        set(SOLOUD_BACKEND_OPENSLES TRUE)
        target_compile_definitions(a2d_engine_library PUBLIC TARGET_ANDROID)
    elseif (IOS)
        set(TARGET_IOS TRUE)
        set(SOLOUD_BACKEND_COREAUDIO TRUE)
        target_compile_definitions(a2d_engine_library PUBLIC TARGET_IOS)
    endif ()

    set(TARGET_MOBILE TRUE)
    set(RENDERER_GL_ES TRUE)
    set(SHADERS_TARGET_LANG gles)
    set(SHADERS_TARGET_PROFILE 100)
    target_compile_definitions(a2d_engine_library PUBLIC TARGET_MOBILE)
elseif (WIN32 OR APPLE OR UNIX)
    if (WIN32)
        set(TARGET_WINDOWS TRUE)
        set(SOLOUD_BACKEND_WINMM TRUE)
        target_compile_definitions(a2d_engine_library PUBLIC TARGET_WINDOWS)
    elseif (APPLE)
        set(TARGET_MACOS TRUE)
        set(SOLOUD_BACKEND_COREAUDIO TRUE)
        target_compile_definitions(a2d_engine_library PUBLIC TARGET_MACOS)
    elseif (UNIX)
        set(TARGET_LINUX TRUE)
        set(SOLOUD_BACKEND_ALSA TRUE)
        target_compile_definitions(a2d_engine_library PUBLIC TARGET_LINUX)
    endif ()

    set(TARGET_DESKTOP TRUE)
    set(RENDERER_GL TRUE)
    set(SHADERS_TARGET_LANG glsl)
    set(SHADERS_TARGET_PROFILE 330)
    target_compile_definitions(a2d_engine_library PUBLIC TARGET_DESKTOP)
endif ()

set(SOLOUD_STATIC TRUE)

# Platform-independent dependencies
add_subdirectory(thirdparty/soloud)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/lodepng)
add_subdirectory(thirdparty/simplejson)
add_subdirectory(thirdparty/freetype)
add_subdirectory(thirdparty/box2d)

target_link_libraries(a2d_engine_library spdlog lodepng simplejson freetype soloud box2d)

# Desktop GL
if (TARGET_DESKTOP)
    find_package(OpenGL REQUIRED)

    add_subdirectory(thirdparty/glew)
    add_subdirectory(thirdparty/glfw)

    target_link_libraries(a2d_engine_library ${OPENGL_gl_LIBRARY} glew glfw)
endif ()

# Non-portable glslcc-executable path finding
# TODO fix
if (UNIX)
    set(GLSLCC_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/glslcc/glslcc")
elseif (WIN32)
    set(GLSLCC_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/glslcc/glslcc.exe")
endif ()

add_custom_target(build_shaders ALL
        COMMAND ${CMAKE_COMMAND}
        -DSHADERS_TEMPLATE_FILE=${SHADERS_TEMPLATE_FILE}
        -DSHADERS_GENERATED_FILE=${SHADERS_GENERATED_FILE}
        -DSHADERS_ROOT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src/a2d/graphics/shaders
        -DSHADERS_GENERATION_PATH=${CMAKE_CURRENT_BINARY_DIR}/shaders
        -DGLSLCC_EXECUTABLE=${GLSLCC_EXECUTABLE}#$<TARGET_FILE:glslcc>
        -DSHADERS_TARGET_LANG=${SHADERS_TARGET_LANG}
        -DSHADERS_TARGET_PROFILE=${SHADERS_TARGET_PROFILE}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake/build_shaders.cmake")

# Ensure that this file is created
file(WRITE ${SHADERS_GENERATED_FILE} "")

# Generate glslcc makefile
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glslcc")
execute_process(
        COMMAND ${CMAKE_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glslcc" -G ${CMAKE_GENERATOR}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glslcc")
# Compile it after
add_custom_target(build_glslcc ALL
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/glslcc")

add_dependencies(build_shaders build_glslcc)
add_dependencies(a2d_engine_library build_shaders)