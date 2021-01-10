#include "stdafx.h"
#include "DefaultBaseTuple.h"

namespace SmolEngine
{
    DefaultBaseTuple::DefaultBaseTuple()
    {

    }

    const HeadComponent& DefaultBaseTuple::GetInfo() const
    {
        return Info;
    }
}