
##################################################
# $RCSfile: CMakeLists.txt $
# $Source: src/CMakeLists.txt $
# $Revision: 1.13 $
# $Date: 31-jul-2007 11:48:37 $
#
# Author: Jorgen Bodde
# Licence: wxWindows licence
##################################################

# Add files to library common

ADD_LIBRARY(serialize wxSerialize.cpp 
            ${SERIALIZE_SOURCE_DIR}/include/wx/wxSerialize.h)
