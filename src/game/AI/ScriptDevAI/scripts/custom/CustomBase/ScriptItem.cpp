#include "ScriptItem.h"

ScriptItem::ScriptItem(const char* name)
    : CustomScript{ name }
{
    _isActive = false;
}