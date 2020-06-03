# List all subdirectories in a given directory
# Example: SUBDIRLIST(SUBDIRS ${MY_CURRENT_DIR})
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children
       RELATIVE ${curdir}
	   CONFIGURE_DEPENDS
	   "${curdir}/*")
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()