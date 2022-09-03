cmake_minimum_required(VERSION 3.2)

# Find all files 
file(GLOB_RECURSE ALL_FILES CONFIGURE_DEPENDS ${lib_demo_source_path}/**)
add_executable(${lib_demo_name} ${ALL_FILES})
# Creates folder structure in Visual Studio
source_group(TREE "${lib_demo_source_path}/" PREFIX "Source" FILES ${ALL_FILES})

target_compile_features(${lib_demo_name} INTERFACE cxx_std_20)
target_link_libraries(${lib_demo_name} LINK_PUBLIC  ${lib_ut_name} ${lib_json_name} ${lib_jser_name})
target_include_directories(${lib_demo_name} PUBLIC ${lib_demo_source_path}/)
