#pragma once

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Entities/Player.h"

class CustomScript : public Script
{
protected:
    CustomScript(const char* name);
    virtual ~CustomScript() = default;
protected:
    bool _isActive;
};