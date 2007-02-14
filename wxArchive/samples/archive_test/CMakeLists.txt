##################################################
# $RCSfile$
# $Source$
# $Revision$
# $Date$
#
# Author: Jorgen Bodde
# Licence: wxWindows licence
##################################################

ADD_EXECUTABLE(archive_test archive_test.cpp)
           
LINK_DIRECTORIES(${COMMON_BINARY_DIR}/contrib/lib ${LINK_DIRECTORIES})
TARGET_LINK_LIBRARIES(archive_test archive ${WXWIDGETS_LIBRARIES})