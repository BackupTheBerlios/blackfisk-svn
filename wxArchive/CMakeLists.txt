##---------------------------------------------------------------------------
## $RCSfile$
## $Source$
## $Revision$
## $Date$
##---------------------------------------------------------------------------
## Author:      Jorgen Bodde
## Copyright:   (c) Jorgen Bodde
## License:     wxWidgets License
##---------------------------------------------------------------------------

##---------------------------------------------------
## Please set your wxWidgets configuration here
##--------------------------------------------------- 

SET(WXW_USE_DEBUG ON)
SET(WXW_USE_UNICODE ON)
SET(WXW_USE_SHARED OFF)
SET(WXW_USE_UNIV OFF)
SET(WXW_USE_MONO OFF)
SET(WXW_FILE_VERSION "26")
SET(WXW_VERSION "2.6")

PROJECT( ARCHIVE )

# The new FindWX script works better for  windows, but the 
# old script better for Linux (wx-config should be present
# for windows too)
IF(WIN32)
    INCLUDE (${ARCHIVE_SOURCE_DIR}/bin/FindwxW.cmake)
ELSE(WIN32)
    INCLUDE (${ARCHIVE_SOURCE_DIR}/bin/FindwxWin.cmake)
    
    # sync flags for new old flags
    SET(WXWIDGETS_FOUND            ${WXWINDOWS_FOUND})
    SET(WXWIDGETS_INCLUDE_DIR      ${WXWINDOWS_INCLUDE_DIR})
    SET(WXWIDGETS_LINK_DIRECTORIES ${WXWINDOWS_LINK_DIRECTORIES})
    SET(WXWIDGETS_LIBRARIES        ${WXWINDOWS_LIBRARIES})
ENDIF(WIN32)

# Finding Doxygen
INCLUDE (${CMAKE_ROOT}/Modules/FindDoxygen.cmake OPTIONAL)    

# Set flags for debug and release for compilation
SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__WXDEBUG__ -DWXDEBUG=1")

# Options for building the stuff
OPTION( MAKE_DOXYGEN       "Generate doxygen documentation"                                   ON  )
OPTION( MAKE_ARCHIVESAMPLE "Make wxArchive sample"                                            ON  )

INCLUDE_DIRECTORIES(${ARCHIVE_SOURCE_DIR}/include/wx
                    ${ARCHIVE_SOURCE_DIR}/src
                    ${WXWIDGETS_INCLUDE_DIR})

LINK_DIRECTORIES( ${WXWIDGETS_LINK_DIRECTORIES} )

# Doxygen example
IF(MAKE_DOXYGEN)
    CONFIGURE_FILE(${ARCHIVE_SOURCE_DIR}/docs/Doxyfile.in
                ${ARCHIVE_BINARY_DIR}/docs/Doxyfile
                @ONLY IMMEDIATE)                  
    
    ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN}
                      ${ARCHIVE_BINARY_DIR}/docs/Doxyfile DEPENDS
					  ${ARCHIVE_SOURCE_DIR}/docs/Doxyfile.in )  
ENDIF(MAKE_DOXYGEN)                   

SUBDIRS(src samples)
