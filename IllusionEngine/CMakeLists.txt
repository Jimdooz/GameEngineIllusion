cmake_minimum_required (VERSION 3.6)
project (IllusionEngine)

find_package(OpenGL REQUIRED)

#Directories
include_directories(
    ../librairies/glfw/include/
	../librairies/Glad/include/
	include/
)
link_directories(
	../librairies/glfw/
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DILSENGINE_DEBUG -DILSENGINE_INTERNAL_DEBUG")

#files
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

#msvc specific
if(MSVC)
	set(ALL_LIBS
		lib-vc2019/glfw3
		Glad
	)
	add_definitions ( -D_MYSYMBOL1 -D_MYSYMBOL2 )
endif()

#compiling executable and linking libraries
add_executable(IllusionEngine
	${SRC_FILES}
	${INCLUDE_FILES}
)
target_link_libraries(IllusionEngine
	${ALL_LIBS}
)

#source groups to organize sources in your ide
SOURCE_GROUP(src REGULAR_EXPRESSION "src.*" )
SOURCE_GROUP(include REGULAR_EXPRESSION "include.*" )
