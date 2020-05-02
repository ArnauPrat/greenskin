
set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} \
    -std=c99 \
    -Wall \
    -Wno-ignored-attributes \
    -fstrict-aliasing \
    -Wstrict-aliasing")

set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} \
    -std=c++11 \
    -Wall \
    -Wno-ignored-attributes \
    -fno-exceptions \
    -fno-rtti \
    -fno-omit-frame-pointer \
    -fstrict-aliasing \
    -Wstrict-aliasing")

### Setting up furious
set(FURIOUS_ROOT "/home/aprat/projects/personal/furious/build/dist")
set(FURIOUS_INCLUDE_DIR "${FURIOUS_ROOT}/include")
set(FURIOUS_LIBRARIES_DIR "${FURIOUS_ROOT}/lib")
set(FURIOUS_LIBRARIES "libfurious.so")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_DEBUG_FLAGS} -O0 -pg -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_RELEASE_FLAGS} -O3 -g -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_DEBUG_FLAGS} -O0 -pg -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_RELEASE_FLAGS} -O3 -g -ggdb")

### Setting up bgfx
find_package(glfw3 3.2 REQUIRED)

find_package(LibClang REQUIRED)
if(NOT LIBCLANG_FOUND)
    message(FATAL_ERROR "LibClang could not be found")
endif()

set(CLANG_INCLUDES_DIR "${LIBCLANG_LIBDIR}/clang/${LIBCLANG_VERSION_STRING}/include")

message(STATUS "CLANG_INCLUDES_DIR: ${CLANG_INCLUDES_DIR}")

add_subdirectory(third_party/boost_context)

### Setting up GLM
#find_package(glm REQUIRED)

### Setting up Vulkan
set(Vulkan_SDK_ROOT "/opt/vulkan-sdk/1.1.101.0/x86_64")
set(Vulkan_INCLUDE_DIR "${Vulkan_SDK_ROOT}/include")
set(Vulkan_LIBRARIES_DIR "${Vulkan_SDK_ROOT}/lib")
set(Vulkan_LIBRARIES "libvulkan.so")

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${FURIOUS_INCLUDE_DIR})
include_directories(${BULLET_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR})
include_directories("third_party/boost_context/include")
include_directories("third_party/imgui")

link_directories(${BULLET_LIBRARY_DIR})
link_directories(${Vulkan_LIBRARIES_DIR})
link_directories(${FURIOUS_LIBRARIES_DIR})

message(STATUS "FURIOUS ROOT: ${FURIOUS_ROOT}")
message(STATUS "FURIOUS INCLUDE DIR: ${FURIOUS_INCLUDE_DIR}")
message(STATUS "FURIOUS LIBRARIES DIR: ${FURIOUS_LIBRARIES_DIR}")


#set(VK_RENDERER_SOURCE_FILES 
#  engine/rendering/vulkan/gui/imgui_impl_vulkan.cpp
#  engine/rendering/vulkan/gui/imgui_impl_vulkan.h
#  engine/rendering/vulkan/gui/vkgui.cpp
#  engine/rendering/vulkan/gui/vkgui.h
#  engine/rendering/vulkan/vkbuffer.cpp
#  engine/rendering/vulkan/vkbuffer.h
#  engine/rendering/vulkan/vkbuffer_tools.cpp
#  engine/rendering/vulkan/vkbuffer_tools.h
#  engine/rendering/vulkan/vkdevice.cpp
#  engine/rendering/vulkan/vkdevice.h
#  engine/rendering/vulkan/vkmem_alloc.h
#  engine/rendering/vulkan/vkrenderer.cpp
#  engine/rendering/vulkan/vkrenderer.h
#  engine/rendering/vulkan/vkrenderer_tools.cpp
#  engine/rendering/vulkan/vkrenderer_tools.h
#  engine/rendering/vulkan/vkshader.cpp
#  engine/rendering/vulkan/vkshader.h
#  engine/rendering/vulkan/vkshader_tools.cpp
#  engine/rendering/vulkan/vkshader_tools.h
#  engine/rendering/vulkan/vkswapchain.cpp
#  engine/rendering/vulkan/vkswapchain.h
#  engine/rendering/vulkan/vkvertex_tools.cpp
#  engine/rendering/vulkan/vkvertex_tools.h
#  )
#
#set(TNA_ENGINE_COMPONENTS
#  engine/components/fps_camera.h
#  engine/components/fps_camera.cpp
#  engine/components/render_mesh_data.h
#  engine/components/render_mesh_data.cpp
#  engine/components/transform.h
#  engine/components/transform.cpp
#  engine/components/transform_matrix.h
#  )
#
#set(TNA_ENGINE_GUI
#  engine/gui/imconfig.h
#  engine/gui/imgui.h
#  engine/gui/imgui.cpp
#  engine/gui/imgui_demo.cpp
#  engine/gui/imgui_draw.cpp
#  engine/gui/imgui_internal.h
#  engine/gui/imgui_widgets.cpp
#  engine/gui/imstb_rectpack.h
#  engine/gui/imstb_textedit.h
#  engine/gui/imstb_truetype.h
#  engine/gui/imgui_impl_glfw.h
#  engine/gui/imgui_impl_glfw.cpp
#  engine/gui/widgets/tasking_widget.h
#  engine/gui/widgets/tasking_widget.cpp
#  )
#
#set(TNA_ENGINE_TRACE
#  engine/trace/trace.h
#  engine/trace/trace.cpp
#  )
#
#
#set(TNA_ENGINE_TASKING
#  engine/tasking/barrier.h
#  engine/tasking/barrier.cpp
#  engine/tasking/atomic_counter.h
#  engine/tasking/atomic_counter.cpp
#  engine/tasking/task.h
#  engine/tasking/task_pool.h
#  engine/tasking/task_pool.cpp
#  engine/tasking/tasking.h
#  engine/tasking/tasking.cpp
#  engine/tasking/mutex.h
#  engine/tasking/mutex.cpp
#  )
#
#set(TNA_ENGINE_FURIOUS
#  engine/furious/scheduler.h
#  engine/furious/scheduler.cpp
#  )
#
#set(TNA_ENGINE_SOURCE_FILES 
#  ${TNA_ENGINE_TRACE}
#  ${TNA_ENGINE_TASKING}
#  ${TNA_ENGINE_COMPONENTS}
#  ${TNA_ENGINE_FURIOUS}
#  ${VK_RENDERER_SOURCE_FILES}
#  ${TNA_ENGINE_GUI}
#  engine/config.cpp
#  engine/config.h
#  engine/engine.cpp
#  engine/engine.h
#  engine/error.cpp
#  engine/error.h
#  engine/furious_runtime.cpp
#  engine/game_app.h
#  engine/game_state.cpp
#  engine/game_state.h
#  engine/log.cpp
#  engine/log.h
#  engine/math/math_tools.cpp
#  engine/math/math_tools.h
#  engine/math/matrix.h
#  engine/math/vector.h
#  engine/shapes/aabb.cpp
#  engine/shapes/aabb.h
#  engine/rendering/buffer.h
#  engine/rendering/mesh_data.cpp
#  engine/rendering/mesh_data.h
#  engine/rendering/renderer.h
#  engine/rendering/renderer.cpp
#  engine/rendering/rendering_scene.h
#  engine/rendering/rendering_scene.cpp
#  engine/rendering/shader.h
#  engine/rendering/vertex.h
#  engine/resources/directory_registry.cpp
#  engine/resources/directory_registry.h
#  engine/resources/resource_registry.h
#  engine/resources/resource_registry.inl
#  engine/resources/resources.cpp
#  engine/resources/resources.h
#  engine/tools/colors.h
#  engine/tools/files.cpp
#  engine/tools/files.h
#  engine/tools/references.h
#  engine/types.h
#  engine/data/bvh.h
#  engine/data/bvh.cpp
#  engine/data/queue.h
#  engine/data/queue.inl
#  engine/data/lfqueue.h
#  engine/data/lfqueue.cpp
#	)
#
#set(TNA_ENGINE_SCRIPTS
#  engine/scripts/render_mesh.cpp
#  engine/scripts/transform_hierarchy.cpp
#  engine/scripts/set_proj_view.cpp
#  )
#
#set(TNA_SCRIPTS
#  scripts/rotator.cpp
#  scripts/update_unit_members.cpp
#  scripts/test.cpp
#  )
#
#add_library(tnalib STATIC 
#  ${TNA_ENGINE_SOURCE_FILES}
#  )
#
#target_link_libraries(tnalib
#  boost_context
#  ${FURIOUS_LIBRARIES} 
#  glfw
#  ${Vulkan_LIBRARIES}
#  pthread
#)
#
#set(TNA_SOURCE_FILES 
#  game.cpp
#  game.h
#  main.cpp
#  platform.h
#  scene.h
#  scene.cpp
#  prefabs.h
#  prefabs.cpp
#  game_references.h
#  game_tags.h
#  tnasdk/definitions.h
#	)

set(IMGUI_SOURCES
  third_party/imgui/imconfig.h
  third_party/imgui/imgui.cpp
  third_party/imgui/imgui.h
  third_party/imgui/imgui_demo.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_impl_glfw.cpp
  third_party/imgui/imgui_impl_glfw.h
  third_party/imgui/imgui_impl_vulkan.cpp
  third_party/imgui/imgui_impl_vulkan.h
  third_party/imgui/imgui_internal.h
  third_party/imgui/imgui_widgets.cpp
  third_party/imgui/imstb_rectpack.h
  third_party/imgui/imstb_textedit.h
  third_party/imgui/imstb_truetype.h
  )

set(GSVK_SOURCES
  #vulkan/gui/vkgui.c
  #vulkan/gui/vkgui.h
  vulkan/vkbuffer.c
  vulkan/vkbuffer.h
  vulkan/vkdevice_tools.c
  vulkan/vkdevice_tools.h
  vulkan/vkimage.c
  vulkan/vkimage.h
  vulkan/vkpipeline.c
  vulkan/vkpipeline.h
  vulkan/vkrenderer.c
  vulkan/vkrenderer.h
  vulkan/vkshader_registry.c
  vulkan/vkshader_registry.h
  vulkan/vkswapchain_tools.c
  vulkan/vkswapchain_tools.h
  vulkan/vkvertex_tools.c
  vulkan/vkvertex_tools.h
  )

set(GS_SOURCE_FILES
  #  ${IMGUI_SOURCES}
  ${GSVK_SOURCES}
  atomic_counter.c
  atomic_counter.h
  barrier.c
  barrier.h
  config.c
  config.h
  cond_var.h
  cond_var.c
  error.c
  error.h
  game.c
  log.c
  log.h
  memory.c
  memory.h
  mutex.h
  platform.h
  platform.c
  renderer.h
  resources.c
  task.h
  task_pool.h
  task_pool.c
  tasking.h
  tasking.c
  trace.h
  trace.c
  thread.h
  thread.c
  queue.h
  queue.c
  )


#message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
#add_custom_command(
#	OUTPUT engine/furious_runtime.cpp
#	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/engine/
#	COMMAND ${FURIOUS_ROOT}/bin/fcc -o
#  ${CMAKE_CURRENT_BINARY_DIR}/engine/furious_runtime.cpp ${TNA_SCRIPTS} ${TNA_ENGINE_SCRIPTS} -- --std=c++17 -I${CLANG_INCLUDES_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${FURIOUS_INCLUDE_DIR} -I ${Vulkan_INCLUDE_DIR}
#  DEPENDS ${FURIOUS_ROOT}/bin/fcc ${TNA_SCRIPTS} ${TNA_ENGINE_SCRIPTS} ${TNA_COMPONENTS}
#	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#	)
#
#add_custom_target(
#  regen
#	COMMAND ${FURIOUS_ROOT}/bin/fcc -o
#  ${CMAKE_CURRENT_BINARY_DIR}/engine/furious_runtime.cpp ${TNA_SCRIPTS} ${TNA_ENGINE_SCRIPTS} -- --std=c++17 -I${CLANG_INCLUDES_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${FURIOUS_INCLUDE_DIR} -I ${Vulkan_INCLUDE_DIR}
#	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  COMMENT "Regenerating furious code"
#  )
#
add_executable(greenskin
  ${GS_SOURCE_FILES}
 )  

target_link_libraries(greenskin
  boost_context
  glfw
  furious
  pthread
  ${Vulkan_LIBRARIES}
  )

#add_subdirectory(test)

