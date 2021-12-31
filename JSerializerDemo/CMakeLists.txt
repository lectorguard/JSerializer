cmake_minimum_required (VERSION 3.2)
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
add_executable (JSerializerDemo ${SOURCE_LIST} ${HEADER_LIST})

target_compile_features(JSerializerDemo INTERFACE cxx_std_17)
target_compile_options(JSerializerDemo INTERFACE 
        $<$<AND:$<PLATFORM_ID:Darwin>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,8.0>>:-mmacosx-version-min=10.15>)

target_link_libraries (JSerializerDemo LINK_PUBLIC JSerializerLib)