set(CODE_FILE 
main.cpp
./src/dbserver.h
./src/dbserver.cpp
./src/dbserver_aid.h
./src/dbserver_aid.cpp
)
set(BIN_NAME dbserver)

#The project directory is the same as the file directory
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
	boost_thread-vc140-mt
	boost_regex-vc140-mt
	boost_program_options-vc140-mt
	libpq
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
	libboost_chrono.a
	libpq.a
	libpgcommon.a
	libpgport.a
	libssl.a
	dl
	)
endif()
