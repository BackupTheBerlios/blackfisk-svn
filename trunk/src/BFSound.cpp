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

#if defined(__WXMSW__)
    #include <windows.h>
#endif

#include "BFEnvironment.h"
#include "BFSettings.h"

BFSound::BFSound ()
{
}


/*virtual*/ BFSound::~BFSound ()
{
}

/*static*/ void BFSound::Beep (int frqz, int ms)
{
    #if defined(__WXMSW__)
        ::Beep(frqz, ms);
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
        #include <stdlib.h>
        #include <fcntl.h>
        #include <linux/kd.h>

        int main(int argc, char *argv[])
        {
            int fd = open("/dev/tty10", O_RDONLY);
            if (fd == -1 || argc != 3) return -1;
            return ioctl(fd, KDMKTONE, (atoi(argv[2])<<16)+(1193180/atoi(argv[1])));
        }
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif
}

/*static*/ void BFSound::Info ()
{
    if ( BFSettings::Instance().Instance().GetSoundBehaviour() == 0
     || (BFSettings::Instance().GetSoundBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        wxSound(BFEnvironment::GetSoundDir() + "info.wav").Play();
    }

    if ( BFSettings::Instance().GetBeepBehaviour() == 0
     || (BFSettings::Instance().GetBeepBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        Beep(1700, 100);
        Beep(1800, 100);
    }
}

/*static*/ void BFSound::Warning ()
{
    if ( BFSettings::Instance().GetSoundBehaviour() == 0
     || (BFSettings::Instance().GetSoundBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        wxSound(BFEnvironment::GetSoundDir() + "warning.wav").Play();
    }

    if ( BFSettings::Instance().GetBeepBehaviour() == 0
     || (BFSettings::Instance().GetBeepBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        Beep(1700, 100);
        Beep(1400, 100);
    }
}

/*static*/ void BFSound::Error ()
{
    if ( BFSettings::Instance().GetSoundBehaviour() == 0
     || (BFSettings::Instance().GetSoundBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        wxSound(BFEnvironment::GetSoundDir() + "error.wav").Play();
    }

    if ( BFSettings::Instance().GetBeepBehaviour() == 0
     || (BFSettings::Instance().GetBeepBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        Beep(1500, 100);
        Beep(1000, 200);
    }
}

/*static*/ void BFSound::Fatal ()
{
    if ( BFSettings::Instance().GetSoundBehaviour() == 0
     || (BFSettings::Instance().GetSoundBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        wxSound(BFEnvironment::GetSoundDir() + "fatal.wav").Play();
    }

    if ( BFSettings::Instance().GetBeepBehaviour() == 0
     || (BFSettings::Instance().GetBeepBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        Beep(1500, 100);
        Beep(1000, 200);
        Beep(500, 300);
    }
}

/*static*/ void BFSound::Finish ()
{
    if ( BFSettings::Instance().GetSoundBehaviour() == 0
     || (BFSettings::Instance().GetSoundBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        wxSound(BFEnvironment::GetSoundDir() + "finish.wav").Play();
    }

    if ( BFSettings::Instance().GetBeepBehaviour() == 0
     || (BFSettings::Instance().GetBeepBehaviour() == 1 && BFEnvironment::IsProjectScheduled()) )
    {
        Beep(2000, 100);
        Beep(2200, 100);
        Beep(2000, 100);
        Beep(2200, 100);
        Beep(2500, 200);
        Beep(2500, 200);
        Beep(2500, 200);
    }
}
