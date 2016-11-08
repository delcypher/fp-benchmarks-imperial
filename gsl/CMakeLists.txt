###############################################################################
# Build GNU Scientific Library (GSL)and make available as a target to link
# against.
###############################################################################
include(ExternalProject)

if (("${CMAKE_VERSION}" VERSION_EQUAL "3.4") OR ("${CMAKE_VERSION}" VERSION_GREATER "3.4"))
  set(EXTERNAL_PROJECT_USE_TERMINAL_ARGS
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_INSTALL 1
  )
else()
  set(EXTERNAL_PROJECT_USE_TERMINAL_ARGS "")
endif()


# Unfortunately `BUILD_ALWAYS` only seems to be supported with the version of ExternalProject
# that shipped with CMake >= 3.1. Should we just avoid using this option? We don't really
# need it unless we are patching gsl itself and need to rebuild.
if (("${CMAKE_VERSION}" VERSION_EQUAL "3.1") OR ("${CMAKE_VERSION}" VERSION_GREATER "3.1"))
  option(GSL_ALWAYS_REBUILD "Always try to rebuild GSL" ON)
  if (GSL_ALWAYS_REBUILD)
    set(EXTERNAL_PROJECT_BUILD_ALWAYS_ARG BUILD_ALWAYS 1)
  else()
    set(EXTERNAL_PROJECT_BUILD_ALWAYS_ARG BUILD_ALWAYS 0)
  endif()
else()
  set(EXTERNAL_PROJECT_BUILD_ALWAYS_ARG "")
  message(WARNING "The GNU Scientific Library will not be automatically re-built. Try deleting the stamp files in BuildAndInstallGSL-stamp to trigger a rebuild")
endif()

set(GSL_AVAILABLE TRUE)
# Add a target to build and do a fake install of GSL
set(GSL_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gsl_install")
set(GSL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/gsl_build")

set(EXTERNAL_PROJECT_HAS_BYPRODUCTS_ARGS "")
if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
  if ("${CMAKE_VERSION}" VERSION_LESS "3.2")
    message(FATAL_ERROR "Need CMake >= 3.2 if using Ninja")
  else()
    # Need this when building with Ninja otherwise it will complain there is no rule to make these files.
    set(EXTERNAL_PROJECT_HAS_BYPRODUCTS_ARGS BUILD_BYPRODUCTS "${GSL_INSTALL_PREFIX}/lib/libgsl.a" "${GSL_INSTALL_PREFIX}/lib/libgslcblas.a")
  endif()
else()
endif()

if (DEFINED CMAKE_CONFIGURATION_TYPES)
  # FIXME: We can't set BINARY_DIR to use $<CONFIG> (it doesn't get expanded)
  # so we won't get separate builds for different build types. It's not worth
  # fixing this now so just don't support multi-configuration generators.
  message(FATAL_ERROR "Multi-configuration generators not supported")
endif()

# This is a hack.
# FIXME: CMake doesn't realise that GSL's configure step depends on this
# value so this option has to be set correctly on first build.
option(GSL_LONG_DOUBLE_AS_DOUBLE_HACK "Rewrite long double as double" ON)
if (GSL_LONG_DOUBLE_AS_DOUBLE_HACK)
  set(GSL_LONG_DOUBLE_FLAG "-DLONG_DOUBLE=double")
else()
  set(GSL_LONG_DOUBLE_FLAG "")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
set(GSL_CFLAGS "${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} ${CMAKE_C_FLAGS} ${GSL_LONG_DOUBLE_FLAG}")
set(GSL_CXXFLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} ${CMAKE_CXX_FLAGS} ${GSL_LONG_DOUBLE_FLAG}")
# Generate a script to invoke the configure script so we can pass environment variables.
set(AUTOGEN_MESSAGE "Automatically generated. DO NOT EDIT!")
set(configure_script "${CMAKE_CURRENT_SOURCE_DIR}/gsl-2.1/configure")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configure-wrapper.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/configure-wrapper.sh" @ONLY)
unset(AUTOGEN_MESSAGE)
unset(configure_script)


ExternalProject_Add(BuildAndInstallGSL
  # Configure Step
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gsl-2.1" # Do we need this?
  CONFIGURE_COMMAND sh "${CMAKE_CURRENT_BINARY_DIR}/configure-wrapper.sh"
  # Build step
  BINARY_DIR "${GSL_BUILD_DIR}"
  ${EXTERNAL_PROJECT_BUILD_ALWAYS_ARG}
  ${EXTERNAL_PROJECT_HAS_BYPRODUCTS_ARGS}
  BUILD_COMMAND
    make -j${EXTERNAL_PROJECT_JOBS} && make -j${EXTERNAL_PROJECT_JOBS} install
  # Install step
  # Logging
  #LOG_CONFIGURE 1
  #LOG_BUILD 1
  #LOG_INSTALL 1
  # Output to Terminal
  ${EXTERNAL_PROJECT_USE_TERMINAL_ARGS}
)

set(GSL_INCLUDE_DIR "${GSL_INSTALL_PREFIX}/include")
# Import the built libraries
add_library(gsl_lib STATIC IMPORTED)
set_target_properties(gsl_lib PROPERTIES
  IMPORTED_LOCATION "${GSL_INSTALL_PREFIX}/lib/libgsl.a"
)
add_library(gslcblas_lib STATIC IMPORTED)
set_target_properties(gslcblas_lib PROPERTIES
  IMPORTED_LOCATION "${GSL_INSTALL_PREFIX}/lib/libgslcblas.a"
)
set(GSL_LIBS gsl_lib gslcblas_lib)
# Try to make sure that when targets link against these imported libraries
# That build and install is always done first.
add_dependencies(gsl_lib BuildAndInstallGSL)
add_dependencies(gslcblas_lib BuildAndInstallGSL)

# Benchmarks
add_subdirectory(benchmarks)
