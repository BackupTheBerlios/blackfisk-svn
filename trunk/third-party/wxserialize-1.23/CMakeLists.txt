##---------------------------------------------------------------------------
## Author:      Jorgen Bodde
## Copyright:   (c) Jorgen Bodde
## License:     see LICENSE for details
## Revision:    $Rev: 15 $
##---------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)

##---------------------------------------------------
## Please set your wxWidgets configuration here
##--------------------------------------------------- 

PROJECT( SERIALIZE )

SET(wxWidgets_USE_LIBS base)

FIND_PACKAGE(wxWidgets COMPONENTS base)
FIND_PACKAGE(Doxygen)    

IF(wxWidgets_FOUND)

        # Set flags for debug and release for compilation
        SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS}")
        
        # Options for building the stuff
        IF(Doxygen_FOUND)
                OPTION( MAKE_DOXYGEN         "Generate doxygen documentation" ON  )
        ENDIF(Doxygen_FOUND)
        
        OPTION( MAKE_SERIALIZESAMPLE "Make wxSerialize sample" ON  )

        # Doxygen example
        IF(MAKE_DOXYGEN)
            CONFIGURE_FILE(${SERIALIZE_SOURCE_DIR}/docs/Doxyfile.in
                        ${SERIALIZE_BINARY_DIR}/docs/Doxyfile
                        @ONLY IMMEDIATE)                  
            
            ADD_CUSTOM_TARGET(api-docs ALL ${DOXYGEN}
                              ${SERIALIZE_BINARY_DIR}/docs/Doxyfile DEPENDS
  		              ${SERIALIZE_SOURCE_DIR}/docs/Doxyfile.in )  
        ENDIF(MAKE_DOXYGEN)                   

        INCLUDE_DIRECTORIES(${SERIALIZE_SOURCE_DIR}/include/wx
                            ${wxWidgets_INCLUDE_DIRS})

        ADD_LIBRARY(serialize ${SERIALIZE_SOURCE_DIR}/src/wxSerialize.cpp ${SERIALIZE_SOURCE_DIR}/include/wx/wxSerialize.h)

        if(COMMAND cmake_policy)
                cmake_policy(SET CMP0003 NEW)
        endif(COMMAND cmake_policy)
    
        ADD_EXECUTABLE(archive_test ${SERIALIZE_SOURCE_DIR}/samples/archive_test.cpp)           
        TARGET_LINK_LIBRARIES(archive_test serialize ${wxWidgets_LIBRARIES})

ELSE(wxWidgets_FOUND)
        MESSAGE("wxWidgets not found!")
ENDIF(wxWidgets_FOUND)
