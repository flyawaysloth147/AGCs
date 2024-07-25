#pragma once

#include<string>
#include<glm/glm.hpp>
#include<imgui.h>

namespace AGC {
	namespace Utils {
		std::string trimLeadingSpaces(const std::string& str);
		constexpr glm::vec2 ImVec2Toglm(const ImVec2& val);
	}
}