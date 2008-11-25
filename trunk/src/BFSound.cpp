/**
 * Name:        BFSound.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-11-25
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "BFSound.h"

#include <wx/sound.h>

#include "BFEnvironment.h"

BFSound::BFSound ()
{
}


/*virtual*/ BFSound::~BFSound ()
{
}


/*static*/ void BFSound::Info ()
{
    wxSound(BFEnvironment::GetSoundDir() + "info.wav").Play();
}

/*static*/ void BFSound::Warning ()
{
    wxSound(BFEnvironment::GetSoundDir() + "warning.wav").Play();
}

/*static*/ void BFSound::Error ()
{
    wxSound(BFEnvironment::GetSoundDir() + "error.wav").Play();
}

/*static*/ void BFSound::Fatal ()
{
    wxSound(BFEnvironment::GetSoundDir() + "fatal.wav").Play();
}

/*static*/ void BFSound::Finish ()
{
    wxSound(BFEnvironment::GetSoundDir() + "finish.wav").Play();
}
