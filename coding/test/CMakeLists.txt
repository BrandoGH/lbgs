set(CODE_FILE 
./main.cpp
./exampletest.cpp
./servercommon/commontooltest.cpp
./servercommon/safequeuetest.cpp
)

set(BIN_NAME test)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#Additional directories -- Header file lookup
include_directories(
	./
	../
	../include
	../servercommon
	${3RD_PATH}
)

#Add library search directory
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