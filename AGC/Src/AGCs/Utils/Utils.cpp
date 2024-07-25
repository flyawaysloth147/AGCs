#include<AGCpch.h>

#include "Utils.h"

namespace AGC {
	namespace Utils {
		std::string trimLeadingSpaces(const std::string& str) {
			size_t start = str.find_first_not_of(' ');
			return (start == std::string::npos) ? "" : str.substr(start);
		}

		constexpr glm::vec2 ImVec2Toglm(const ImVec2& val) {
			return { val.x, val.y };
		}
	}
}