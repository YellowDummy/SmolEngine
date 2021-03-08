#pragma once

#ifdef PLATFORM_WIN
#include <Windows.h>
#endif

#include <memory>
#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include <utility>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>

#include "ECS/WorldAdmin.h"
#include "ECS/Actor.h"

#include "Core/Engine.h"

#include <../../SmolEngine/Libraries/cereal/include/cereal/cereal.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/types/polymorphic.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/archives/json.hpp>
#include <../../SmolEngine/Libraries/glm/glm/glm.hpp>


