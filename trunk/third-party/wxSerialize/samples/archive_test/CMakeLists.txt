##################################################
# $RCSfile: CMakeLists.txt $
# $Source: samples/archive_test/CMakeLists.txt $
# $Revision: 1.13 $
# $Date: 31-jul-2007 11:48:37 $
#
# Author: Jorgen Bodde
# Licence: wxWindows licence
##################################################

ADD_EXECUTABLE(archive_test archive_test.cpp)
           
LINK_DIRECTORIES(${COMMON_BINARY_DIR}/contrib/lib ${LINK_DIRECTORIES})
TARGET_LINK_LIBRARIES(archive_test serialize ${WXWIDGETS_LIBRARIES})
