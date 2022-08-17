#include "custom_script.h"

CustomScript::CustomScript(const char* name)
    : Script{}
    , _isActive{ false }
{
    Name = name;
}