cmake_minimum_required (VERSION 3.2)
project (JSerializer VERSION 0.1)

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Json Library
set(lib_json_name nlohmann_json)
set(lib_json_destination_path ${CMAKE_SOURCE_DIR}/JSerializerLib/ThirdParty/${lib_json_name})
set(lib_json_include_path ${lib_json_destination_path}/include/)
set(lib_json_source_path ${lib_json_destination_path}/include/nlohmann)
set(lib_json_download_path "https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip")

#Boost Unit Tests
set(lib_ut_name boost_ut)
set(lib_ut_destination_path ${CMAKE_SOURCE_DIR}/JSerializerDemo/ThirdParty/${lib_ut_name})
set(lib_ut_include_path ${lib_ut_destination_path}/include/)
set(lib_ut_source_path ${lib_ut_destination_path}/include/boost)
set(lib_ut_download_path "https://github.com/boost-ext/ut/archive/refs/tags/v1.1.9.zip")

#Json Serializer Library
set(lib_jser_name JSerializerLib)
set(lib_jser_source_path ${CMAKE_SOURCE_DIR}/${lib_jser_name}/Include)

#JSerializer Demo
set(lib_demo_name JSerializerDemo)
set(lib_demo_source_path ${CMAKE_SOURCE_DIR}/${lib_demo_name}/Source)


include(FetchContent)

# Package Name : Name of the package/library
# Download PATH : URL to the package/library
# Destination PATH : Path to the source code of the package/library
function(TRY_DOWNLOAD_ZIP PACKAGE_NAME DOWNLOAD_PATH DESTINATION_PATH)
	if(EXISTS ${DESTINATION_PATH})
		message(STATUS "Package with name ${PACKAGE_NAME} and path ${DESTINATION_PATH} already exists, no download executed")
	else()
		message(STATUS "Download Package ${PACKAGE_NAME} ...")
		FetchContent_Declare(
						${PACKAGE_NAME} 
						URL ${DOWNLOAD_PATH} 
						SOURCE_DIR ${DESTINATION_PATH}
						DOWNLOAD_EXTRACT_TIMESTAMP false
						)
		FetchContent_MakeAvailable(${PACKAGE_NAME})
		message(STATUS "Done.")
	endif()
endfunction()

# Library name : A library with this name will be added, please add parenthesis around the name 
# Library path : The path to the downloaded library, without a "/" at the end
function(CREATE_HEADER_LIBRARY LIBRARY_NAME LIBRARY_PATH)
	get_property(use_folder_is_set GLOBAL PROPERTY USE_FOLDERS SET)
	if (NOT use_folder_is_set)
		message(WARNING "Please activate folder view : set_property(GLOBAL PROPERTY USE_FOLDERS ON)") 
	endif()
	
	message(STATUS "Find all files in library path : ${LIBRARY_PATH}")
	# Find all source files in library path
	file(GLOB_RECURSE LIBRARY_FILES CONFIGURE_DEPENDS ${LIBRARY_PATH}/**)
	message(STATUS "Found files for library ${LIBRARY_NAME} with path ${LIBRARY_PATH}")
	#message(STATUS "${LIBRARY_FILES}")
	add_library(${LIBRARY_NAME} INTERFACE)
	# Creating a dummy library to force the visual studio project creation
	add_library(${LIBRARY_NAME}_Preview STATIC ${LIBRARY_FILES})
	set_target_properties(${LIBRARY_NAME}_Preview PROPERTIES LINKER_LANGUAGE CXX)
	
	source_group(TREE "${LIBRARY_PATH}/" PREFIX "Include" FILES ${LIBRARY_FILES})
	message(STATUS "Created successfully library with name: ${LIBRARY_NAME}")
	
	unset(LIBRARY_FILES)
endfunction()

# Create Json Lib 
try_download_zip(${lib_json_name} ${lib_json_download_path} ${lib_json_destination_path})
create_header_library(${lib_json_name} ${lib_json_source_path})
target_compile_features(${lib_json_name} INTERFACE cxx_std_17)
target_include_directories(${lib_json_name} INTERFACE ${lib_json_include_path})

# Create ut Lib
try_download_zip(${lib_ut_name} ${lib_ut_download_path} ${lib_ut_destination_path})
create_header_library(${lib_ut_name} ${lib_ut_source_path})
target_compile_features(${lib_ut_name} INTERFACE cxx_std_20)
target_include_directories(${lib_ut_name} INTERFACE ${lib_ut_include_path})

# Add projects
add_subdirectory(JSerializerLib)
add_subdirectory(JSerializerDemo)



