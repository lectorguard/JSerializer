# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
cmake_minimum_required (VERSION 3.2)

# Add an executable target 
add_library(nlohmann_json INTERFACE)

target_include_directories(nlohmann_json INTERFACE /)
target_include_directories(nlohmann_json INTERFACE nlohmann/)
#target_sources(nlohmann_json INTERFACE ${HEADER_LIST})

## Require C++ 17 compiler
target_compile_features(nlohmann_json INTERFACE cxx_std_17)
target_compile_options(nlohmann_json INTERFACE 
        $<$<AND:$<PLATFORM_ID:Darwin>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,8.0>>:-mmacosx-version-min=10.15>)
