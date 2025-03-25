cmake_minimum_required(VERSION 3.10)
project(FluidSimulation)

# Add the "include" directory to the include paths
include_directories(${PROJECT_SOURCE_DIR}/include)


# Find OpenCV
# set the following absolute path to the OpenCVConfig.cmake file if OpenCV is not installed in the default location.
#set(OpenCV_DIR "C:/opencv/build")
find_package(OpenCV REQUIRED)

# Add executable with source and header files
add_executable(fluid_simulation 
    src/main.cpp 
    src/fluid_simulation.cpp 
    src/visualizer.cpp
    include/fluid_simulation.h
    include/visualizer.h
)

# Link OpenCV
target_link_libraries(fluid_simulation ${OpenCV_LIBS})

# Install OpenCV DLLs to the build and install directories
if(WIN32 AND OpenCV_FOUND)
    foreach(dll ${OpenCV_DLLS})
        add_custom_command(TARGET fluid_simulation POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll}
            $<TARGET_FILE_DIR:fluid_simulation>)
    endforeach()
endif()
