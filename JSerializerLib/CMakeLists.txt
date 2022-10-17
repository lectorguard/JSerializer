cmake_minimum_required (VERSION 3.2)

create_header_library(${lib_jser_name} ${lib_jser_source_path})
if (NOT ${JSERIALIZER_ONLY_LIBRARY})
	target_compile_features(${lib_jser_name} INTERFACE cxx_std_20)
endif()
target_include_directories(${lib_jser_name} INTERFACE ${lib_jser_source_path}/)


