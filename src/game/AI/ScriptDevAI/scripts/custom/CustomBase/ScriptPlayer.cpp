#include "ScriptPlayer.h"

ScriptPlayer::ScriptPlayer(const char* name)
    : CustomScript{ name }
{
    _isActive = false;
}