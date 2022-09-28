cmake_minimum_required (VERSION 3.20)

set(PROJECT_NAME lbgs)
project (${PROJECT_NAME})

message("-- current system info: ${CMAKE_SYSTEM_NAME} : ${CMAKE_SYSTEM_VERSION}")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=c++14 \
-pthread \
-fPIC \
"
)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)	# should be equivalent to fPIC, generating position independent code
message("-- CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

#cmake : Custom variables for parameter settings
#{  -- start
# common
#ENABLE_DEBUG_LOG		ON/OFF			Whether to enable the debug log, if enabled, the log will be printed on the screen
#ENABLE_UNITEST			ON/OFF			Whether to import the unit test project

#windows
#OPEN_SIMULATION_CLIENT		ON/OFF		Whether to import the simulated client project

#linux
#ENABLE_DEBUG_MODE			ON/OFF		Whether to enable adjustable mode
#}  -- end

#common cmake
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE) #Without this CMAKE_BUILD_TYPE doesn't work
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LBGS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${LBGS_ROOT_DIR}/build/exe)		#Binary executable file generation path [all server-generated files are put together]
set(LIBRARY_OUTPUT_PATH_BASE_DIR ${LBGS_ROOT_DIR}/build/library) #library generation root path
set(CONFIG_BASE_DIR ${LBGS_ROOT_DIR}/config/)

if (WIN32)
	message("-- MSVC_VERSION : ${MSVC_VERSION}")
	add_definitions(-DWIN_OS)
	#visual studio optimization options£¬release mode can be debug¡¾Essentially reuses the value of FLAGS_DEBUG¡¿¡¾win development machine is debuggable by default¡¿
	# visual studio 2022 add hot code
	if(MSVC_VERSION EQUAL 1930)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /ZI /JMC")
		message("-- visual studio version is 2022, support hot reload !!")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
	endif()
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

	set(EXE_DIR ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
	set(3RD_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include)
	set(SERVERCOMMON_LIB ${LBGS_ROOT_DIR}/build/library/servercommon/${CMAKE_BUILD_TYPE})
	set(STATIC_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/lib)
	set(SHARED_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/bin)

	#Copy the required dynamic library to the executable folder [only for windows]
	file(GLOB DLL_PATH ${SHARED_RC_LIB_PATH_3RD}/*)
	file(COPY ${DLL_PATH} DESTINATION ${EXE_DIR})
	message("-- 3rd dll has copy to ${EXE_DIR}")

	file(GLOB BAT_SCRIPT_START_SERVER ${LBGS_ROOT_DIR}/coding/startserver.bat)
	file(COPY ${BAT_SCRIPT_START_SERVER} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${BAT_SCRIPT_START_SERVER} script has copy to ${LBGS_ROOT_DIR}/build")

	file(GLOB BAT_SCRIPT_STOP_SERVER ${LBGS_ROOT_DIR}/coding/stopserver.bat)
	file(COPY ${BAT_SCRIPT_STOP_SERVER} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${BAT_SCRIPT_STOP_SERVER} script has copy to ${LBGS_ROOT_DIR}/build")

	file(GLOB BAT_SCRIPT_VSGEN ${LBGS_ROOT_DIR}/coding/vsgen.bat)
	file(COPY ${BAT_SCRIPT_VSGEN} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${BAT_SCRIPT_VSGEN} script has copy to ${LBGS_ROOT_DIR}/build")

elseif(UNIX)
	add_definitions(-DLINUX_OS)
	#Set the server as needed, enable debug to debug, and default to maximum optimization
	if(ENABLE_DEBUG_MODE)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
    endif()
	message("-- LINUX_LBGS_ENABLE_DEBUG_MODE : ${ENABLE_DEBUG_MODE}")

	set(EXE_DIR ${EXECUTABLE_OUTPUT_PATH})
	set(3RD_PATH ${LBGS_ROOT_DIR}/3rd/x64-linux/include)
	set(SERVERCOMMON_LIB ${LBGS_ROOT_DIR}/build/library/servercommon)
	set(STATIC_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-linux/lib)

	file(GLOB SH_SCRIPT_START_SERVER ${LBGS_ROOT_DIR}/coding/startserver_linux.sh)
	file(COPY ${SH_SCRIPT_START_SERVER} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${SH_SCRIPT_START_SERVER} script has copy to ${LBGS_ROOT_DIR}/build")

	file(GLOB SH_SCRIPT_STOP_SERVER ${LBGS_ROOT_DIR}/coding/stopserver_linux.sh)
	file(COPY ${SH_SCRIPT_STOP_SERVER} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${SH_SCRIPT_STOP_SERVER} script has copy to ${LBGS_ROOT_DIR}/build")

	file(GLOB SH_SCRIPT_REMOVE_ERR_NOHUP_LOG ${LBGS_ROOT_DIR}/coding/removeserver_err_nohup_log.sh)
	file(COPY ${SH_SCRIPT_REMOVE_ERR_NOHUP_LOG} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${SH_SCRIPT_REMOVE_ERR_NOHUP_LOG} script has copy to ${LBGS_ROOT_DIR}/build")
endif()

#global macro definition
add_definitions(
-DCODE_ROOT_PATH=\"${LBGS_ROOT_DIR}\"
-DLOG_ROOT_PATH=\"${LBGS_ROOT_DIR}/build/log\"
-DCONFIG_BASE_DIR=\"${CONFIG_BASE_DIR}\"
-DEXE_DIR=\"${EXE_DIR}\"

#common
-DHANDLER=private
-DSIGNALS=public
-DSLOTS=public
)

if(ENABLE_DEBUG_LOG)
	add_definitions(-DDLOG)
endif()
message("-- ENABLE_DEBUG_LOG : ${ENABLE_DEBUG_LOG}")


message("-- If you wanna re-cmake,please delete[${LBGS_ROOT_DIR}/build/CMakeCache.txt],otherwise the result may be incorrect")
message("-- Please make sure you first compile the dependency library in compilation project")
message("-- you can set add_subdirectory order")
message("-- CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("-- LBGS_ROOT_DIR : ${LBGS_ROOT_DIR}")
message("-- 3RD_PATH : ${3RD_PATH}")
message("-- STATIC_RC_LIB_PATH_3RD : ${STATIC_RC_LIB_PATH_3RD}")
message("-- SHARED_RC_LIB_PATH_3RD : ${SHARED_RC_LIB_PATH_3RD}")
message("-- EXE_DIR : ${EXE_DIR}")
message("-- CONFIG_BASE_DIR : ${CONFIG_BASE_DIR}")
message("-- LIBRARY_OUTPUT_PATH_BASE_DIR : ${LIBRARY_OUTPUT_PATH_BASE_DIR}")
message("-- CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")
message("-- CMAKE_EXE_LINKER_FLAGS_RELEASE : ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message("-- CMAKE_SHARED_LINKER_FLAGS_RELEASE : ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

#The order is more important, compile the general library first
add_subdirectory(./servercommon)
add_subdirectory(./gateserver)
add_subdirectory(./logicserver)
add_subdirectory(./cacheserver)
add_subdirectory(./dbserver)
add_subdirectory(./proxyserver)

if(ENABLE_UNITEST)
	add_subdirectory(./test)
	message("-- ENABLE_UNITEST_PROJECT : ${ENABLE_UNITEST}")
endif()

#param input:  --SIMULATION_CLIENT project import
if (WIN32)
	if(OPEN_SIMULATION_CLIENT)
		
		#qt5
		set(Qt5_INCLUDE_DIR ${3RD_QT5_PATH})
		set(Qt5_DIR ${LBGS_ROOT_DIR}/3rd/x64-windows/share/cmake/Qt5)
		set(CMAKE_AUTOMOC ON)
		set(CMAKE_AUTOUIC ON)
		set(3RD_QT5_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include/qt5)
		#You need to put the [qt plug-in] in the exe directory, otherwise there will be problems with startup
		file(GLOB QT_PLUGIN_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/tools/qt5/bin/plugins/*)
		file(COPY ${QT_PLUGIN_PATH} DESTINATION ${EXE_DIR})

		file(GLOB QT_UI_FILE ${LBGS_ROOT_DIR}/coding/simulationclient/src/ui/*)
		file(COPY ${QT_UI_FILE} DESTINATION ${EXE_DIR}/ui/)

		add_subdirectory(./simulationclient)
		message("-- OPEN_SIMULATION_CLIENT : ${OPEN_SIMULATION_CLIENT}")
	endif()
endif()