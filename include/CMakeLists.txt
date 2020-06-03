include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_HEADERS"
	     CONFIGURE_DEPENDS
		 "${subdir}/*.h")
	message(STATUS "include/${subdir}/")
	if (NOT ("${${subdir}_HEADERS}" STREQUAL ""))
		target_sources("${subdir}lib" PUBLIC "${${subdir}_HEADERS}")
		target_include_directories("${subdir}lib" PUBLIC
			"${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/")
		FOREACH(HEADER_FILE_PATH ${${subdir}_HEADERS})
			string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/" ""
				HEADER_FILE_NAME ${HEADER_FILE_PATH})
			message(STATUS "\t${HEADER_FILE_NAME}")
		ENDFOREACH()
	endif()
ENDFOREACH()