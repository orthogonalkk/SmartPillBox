include(${CMAKE_CURRENT_LIST_DIR}/nncaseruntimeTargets.cmake)
if(NOT TARGET mpark_variant)
    find_package(mpark_variant REQUIRED)
endif()

if(NOT TARGET gsl-lite)
    find_package(gsl-lite REQUIRED)
endif()
