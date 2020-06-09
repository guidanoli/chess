include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp"
		 "${subdir}/*.h")
	message(STATUS "app/${subdir}/")
	if (NOT ("${${subdir}_SRC}" STREQUAL ""))
		add_executable("${subdir}app" "${${subdir}_SRC}")
		set_target_properties("${subdir}app" PROPERTIES
							  FOLDER applications)
		FOREACH(SOURCE_FILE_PATH ${${subdir}_SRC})
			string(REPLACE "${subdir}/" ""
				SOURCE_FILE_NAME ${SOURCE_FILE_PATH})
			message(STATUS "\t${SOURCE_FILE_NAME}")
		ENDFOREACH()
	endif()
	file (GLOB_RECURSE "${subdir}_CMAKELIST"
		  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	      CONFIGURE_DEPENDS
		  "${subdir}/CMakeLists.txt")
	if (NOT ("${${subdir}_CMAKELIST}" STREQUAL ""))
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()