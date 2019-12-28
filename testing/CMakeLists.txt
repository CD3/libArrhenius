cmake_minimum_required( VERSION 3.10 ) 
message( "Configuring unit test build." )

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )
OPTION( BUILD_NONIUS_BENCHMARKS "Build Nonius-based benchmarks." OFF )
OPTION( BUILD_CELERO_BENCHMARKS "Build Celero-based benchmarks. Note that Nonius and Celero benchmarks cannot be enabled at the same time currently." OFF )
OPTION( BUILD_GOOGLE_BENCHMARKS "Build Google-based benchmarks." OFF )

if( BUILD_NONIUS_BENCHMARKS AND BUILD_CELERO_BENCHMARKS )
  message(SEND_ERROR "Nonius- and Celero-based benchmarks cannot be built at
  the some time. This will cause compile errors.")
endif()

#  ____      _       _       _   _       _ _     _____         _       
# / ___|__ _| |_ ___| |__   | | | |_ __ (_) |_  |_   _|__  ___| |_ ___ 
#| |   / _` | __/ __| '_ \  | | | | '_ \| | __|   | |/ _ \/ __| __/ __|
#| |__| (_| | || (__| | | | | |_| | | | | | |_    | |  __/\__ \ |_\__ \
# \____\__,_|\__\___|_| |_|  \___/|_| |_|_|\__|   |_|\___||___/\__|___/
                                                                      



include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include" )

if(BUILD_UNIT_TESTS)

set( binDir ${CMAKE_BINARY_DIR}/testBin )
set( buildDir ${CMAKE_BINARY_DIR} )

# Unit tests written with the Catch framework are
# automatically detected. Just put the source files
# in the `CatchTests` directory.

set( unitTest_DIR ./CatchTests)
# Search for sources.
file( GLOB_RECURSE unitTest_SOURCES
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      "${unitTest_DIR}/*.cpp" )
message(STATUS "Detected Catch-based Unit Test Sources:")
foreach(testSrc ${unitTest_SOURCES})
  message(STATUS "  ${testSrc}" )
endforeach()

set( testName ${PROJECT_NAME}_CatchTests )
add_executable(${testName} ${unitTest_SOURCES})
target_link_libraries( ${testName} libArrhenius::Arrhenius )
set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${binDir} )

add_test(NAME ${testName} 
         WORKING_DIRECTORY ${buildDir}
         COMMAND ${binDir}/${testName} )


#file(COPY data DESTINATION ${buildDir} )

endif()

# ____                  _                          _        
#| __ )  ___ _ __   ___| |__  _ __ ___   __ _ _ __| | _____ 
#|  _ \ / _ \ '_ \ / __| '_ \| '_ ` _ \ / _` | '__| |/ / __|
#| |_) |  __/ | | | (__| | | | | | | | | (_| | |  |   <\__ \
#|____/ \___|_| |_|\___|_| |_|_| |_| |_|\__,_|_|  |_|\_\___/
                                                           


set( binDir ${CMAKE_BINARY_DIR}/benchmarkBin )


#  ____     _                  ____                  _                          _        
# / ___|___| | ___ _ __ ___   | __ )  ___ _ __   ___| |__  _ __ ___   __ _ _ __| | _____ 
#| |   / _ \ |/ _ \ '__/ _ \  |  _ \ / _ \ '_ \ / __| '_ \| '_ ` _ \ / _` | '__| |/ / __|
#| |__|  __/ |  __/ | | (_) | | |_) |  __/ | | | (__| | | | | | | | | (_| | |  |   <\__ \
# \____\___|_|\___|_|  \___/  |____/ \___|_| |_|\___|_| |_|_| |_| |_|\__,_|_|  |_|\_\___/

if(BUILD_CELERO_BENCHMARKS)

set( benchmark_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CeleroBenchmarks)

if( EXISTS ${benchmark_DIR} )

set(CELERO_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/externals/Celero CACHE PATH "")
set(CELERO_INCLUDE_DIR ${CELERO_DIRECTORY}/include CACHE PATH "Celero's include directory.")
set(CELERO_LIBRARY celero CACHE STRING "")

if( NOT EXISTS ${CELERO_DIRECTORY} )
  message(SEND_ERROR "Celero directory not found. Please clone https://github.com/DigitalInBlue/Celero into ${CELERO_DIRECTORY} or disable Celero benchmarks." )
endif()


#If celero is "out of tree", we have to add the subdirectory differently.
add_subdirectory(${CELERO_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/celero)
include_directories(${CELERO_INCLUDE_DIR})


# Benchmarks written with the Celero framework are
# automatically detected. Just put the source files
# in the `CeleroBenchmarks` directory.

# Search for sources.
file( GLOB_RECURSE benchmark_SOURCES
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      "${benchmark_DIR}/*.cpp" )
    message(STATUS "Detected Celero-based Benchmark Sources:")
foreach(benchmarkSrc ${benchmark_SOURCES})
  message(STATUS "  ${benchmarkSrc}" )
endforeach()


foreach(benchmarkSrc ${benchmark_SOURCES})

get_filename_component(benchmarkName ${benchmarkSrc} NAME_WE)

# Add benchmark executable
add_executable(${benchmarkName} ${benchmarkSrc})
rarget_link_libraries( ${benchmarkName} libArrhenius::Arrhenius )

# Link dependencies
target_link_libraries(${benchmarkName} celero ${CELERO_EXTRA_LIBS} ) # If your project builds a library, link to it here

endforeach()

else(EXISTS ${benchmark_DIR})
  message(WARNING "Clero benchmarks were enabled, but the ${benchmark_DIR} directory was not found.")
endif(EXISTS ${benchmark_DIR})
endif()





