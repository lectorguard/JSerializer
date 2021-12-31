cmake_minimum_required (VERSION 3.2)
add_subdirectory(ThirdParty)

# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "include/**.h")

# Add an executable target 
add_library(JSerializerLib ${SOURCE_LIST} ${HEADER_LIST} ${PUBLIC_HEADER_LIST})
## Require C++ 20 compiler
target_compile_features(JSerializerLib PRIVATE cxx_std_20)
target_compile_options(JSerializerLib PRIVATE
        $<$<AND:$<PLATFORM_ID:Darwin>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,8.0>>:-mmacosx-version-min=10.15>)
#
## Link the executable with required libraries
target_include_directories(JSerializerLib PUBLIC src/)
target_link_libraries(JSerializerLib PUBLIC nlohmann_json)

