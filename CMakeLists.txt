cmake_minimum_required(VERSION 3.22)
project(LearnOpenGL)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# windows output directory
if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif(WIN32)

# compile flags (optimize & release)
set(CMAKE_CXX_FLAGS "-Wall -Os")

# set(BUILD_SHARED_LIBS ON)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_FLAGS -Os)

# config files(root_dir)
configure_file(config/root_directory.h.in config/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/config)

# set libs and include directories
set(3rd-LIBS glfw glad glm assimp imgui)

# set(INCLUDES include/main.h include/linmath.h)
set(LIBS ${3rd-LIBS} OpenGL::GL)

foreach(lib ${3rd-LIBS})
  add_subdirectory(3rd-libs/${lib})
endforeach()

include_directories(include/)

set(Chapters lab1)

set(lab1 model)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest} DEPENDS ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

# create project from source files
function(create_project_from_src chapter demo)
  file(GLOB SOURCE
    "src/${chapter}/${demo}/*.h"
    "src/${chapter}/${demo}/*.cpp"
    "src/${chapter}/${demo}/*.vs"
    "src/${chapter}/${demo}/*.fs"
    "src/${chapter}/${demo}/*.tcs"
    "src/${chapter}/${demo}/*.tes"
    "src/${chapter}/${demo}/*.gs"
    "src/${chapter}/${demo}/*.cs"
  )

  if(demo STREQUAL "")
    SET(replaced "")
    string(REPLACE "/" "_" replaced ${chapter})
    set(NAME ${replaced})
  else()
    set(NAME "${chapter}__${demo}")
  endif()

  add_executable(${NAME} ${SOURCE})
  target_link_libraries(${NAME} ${LIBS})

  if(WIN32)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}/Debug")
  elseif(UNIX AND NOT APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
  elseif(APPLE)
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${chapter}")
  endif(WIN32)

  # Files
  # file(GLOB DLLS "dlls/*.dll")
  file(GLOB SHADERS
    "src/${chapter}/${demo}/*.vs"

    # "src/${chapter}/${demo}/*.frag"
    "src/${chapter}/${demo}/*.fs"
    "src/${chapter}/${demo}/*.tcs"
    "src/${chapter}/${demo}/*.tes"
    "src/${chapter}/${demo}/*.gs"
    "src/${chapter}/${demo}/*.cs"
  )

  file(GLOB FIGURES
    "resource/fig/*.jpg"
    "resource/fig/*.png"
    "resource/fig/*.bmp"
  )

  # copy shaders & figures into build dir
  foreach(SHADER ${SHADERS})
    if(WIN32)
      # configure_file(${SHADER} "test")
      add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
      add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FIGURES} $<TARGET_FILE_DIR:${NAME}>)

    # add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLLS} $<TARGET_FILE_DIR:${NAME}>)
    elseif(UNIX AND NOT APPLE)
      file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${chapter})
      file(COPY ${FIGURES} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${chapter})
    elseif(APPLE)
      # create symbolic link for *.vs *.fs *.gs
      get_filename_component(SHADERNAME ${SHADER} NAME)
      get_filename_component(RESOURCENAME ${FIGURES} NAME)
      makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${chapter}/${SHADERNAME} ${NAME})
      makeLink(${FIGURES} ${CMAKE_SOURCE_DIR}/bin/${chapter}/${RESOURCENAME} ${NAME})
    endif(WIN32)
  endforeach(SHADER)
endfunction(create_project_from_src chapter demo)

# create projects from source files in each chapter
foreach(chapter ${Chapters})
  foreach(demo ${${chapter}})
    create_project_from_src(${chapter} ${demo})
  endforeach(demo)
endforeach(chapter)
