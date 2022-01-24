set(CODE_FILE 
./main.cpp
./src/proxyserver.h
./src/proxyserver.cpp
./config/proxyserverconfig.h
./config/proxyserverconfig.cpp
./export/global.h
./export/export.h
./export/export.cpp
)

set(BIN_NAME proxyserver)

#工程目录和文件目录一致
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

add_definitions(-DPROXYSERVER_LIB)

#附加目录 头文件查找
include_directories(
	./
	../
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
	boost_thread-vc140-mt
    boost_regex-vc140-mt
    boost_program_options-vc140-mt
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(${BIN_NAME} 
	libservercommon.a
	libboost_thread.a
	libboost_regex.a
	libspdlog.a
	libfmt.a
	libboost_program_options.a
 	libssl.a
 	libcrypto.a
 	libboost_filesystem.a
	dl
	)
endif()
