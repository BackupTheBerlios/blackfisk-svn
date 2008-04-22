##################################################
# $RCSfile: CMakeLists.txt $
# $Source: samples/CMakeLists.txt $
# $Revision: 1.13 $
# $Date: 31-jul-2007 11:48:37 $
#
# Author: Jorgen Bodde
# Licence: wxWindows licence
##################################################

# Make our examples when needed

IF (MAKE_SERIALIZESAMPLE)
  SUBDIRS(archive_test)
ENDIF (MAKE_SERIALIZESAMPLE)
