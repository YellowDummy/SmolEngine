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

#include "ECS/Scene.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Actor.h"

#include "Core/Engine.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>


