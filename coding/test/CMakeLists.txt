set(CODE_FILE 
./main.cpp
./exampletest.cpp
./servercommon/commontooltest.cpp
./servercommon/safequeuetest.cpp
)

set(BIN_NAME test)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#输出路径设置
set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH_BASE_DIR}/${BIN_NAME})

#附加目录 头文件查找
include_directories(
	./
	../
	../include
	../servercommon
	${3RD_PATH}
)

#添加库搜索目录
link_directories(
	${STATIC_RC_LIB_PATH_3RD}
	${SHARED_RC_LIB_PATH_3RD}
	${SERVERCOMMON_LIB}
)

add_executable(${BIN_NAME} 
	${CODE_FILE}
)

if (WIN32)
	#link static lib
	target_link_libraries(${BIN_NAME} 
	servercommon
	gtest
	gmock
	lua
	hiredis
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(${BIN_NAME} 
	libservercommon.a
 	libgtest.a
 	libgmock.a
 	libfmt.a
	libssl.a
	libcrypto.a
 	libboost_thread.a
 	libboost_chrono.a
	liblua.a
	libhiredis.a
	)
endif()