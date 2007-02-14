/**
 * OBProjectFile.h
 * definition of class OBProjectFile
 * <comment>
 * 2006-05-23
 ***/

#ifndef OBPROJECTFILE_H
#define OBPROJECTFILE_H

#include <wx/wx.h>
#include "wxArchive/include/wx/wxArchive.h"

/** wxArchiv is not an official wxWidget class
    that is why I do not like the wx-prefix
    jb is for the autor Jorgen Bode */
typedef wxArchive jbArchiv;


///
class OBProjectFile
{
    private:
        /// private members

    protected:
        /// proteced members

    public:
        /// constructor
        OBProjectFile ();

        /// virtual destructor
        virtual ~OBProjectFile ();

};    // class OBProjectFile

#endif    // OBPROJECTFILE_H
