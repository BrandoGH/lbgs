set(CODE_FILE 
main.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${LBGS_ROOT_DIR}/build/exesimulationclient)		#二进制执行文件生成路径

#工程目录和文件目录一致
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

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

add_executable(simulationclient 
${CODE_FILE}
)

#link shared lib
# foreach(
# shared_name
# )
# 	if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll)
# 		file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll DESTINATION ${EXE_DIR})
# 	else()
# 		message("${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll not found")
# 	endif()

# 	if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb)
# 		file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb DESTINATION ${EXE_DIR})
# 	else()
# 		message("${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb not found")
# 	endif()
# endforeach()

# #link static lib
# target_link_libraries(gateserver 
# )
