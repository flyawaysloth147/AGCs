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

		glm::vec4 clampRGBA(int r, int g, int b, int a) {
			glm::vec4 color{ r, g, b, a };
			color = glm::clamp(color, 0.0f, 255.0f);
			return color / 255.0f;
		}

        glm::vec4 hexToRGBA(const std::string& hexColor) {
            std::string hex = hexColor;
            // Remove the hash symbol if present
            if (hex[0] == '#') {
                hex = hex.substr(1);
            }

            // If no alpha is provided, assume fully opaque
            if (hex.length() == 6) {
                hex += "FF";
            }

            if (hex.length() != 8) {
                throw std::invalid_argument("Input should be a 6 (RGB) or 8 (RGBA) digit hex string");
            }

            // Convert hex to RGBA
            int r, g, b, a;
            std::stringstream ss;
            ss << std::hex << hex.substr(0, 2);
            ss >> r;
            ss.clear();
            ss << std::hex << hex.substr(2, 2);
            ss >> g;
            ss.clear();
            ss << std::hex << hex.substr(4, 2);
            ss >> b;
            ss.clear();
            ss << std::hex << hex.substr(6, 2);
            ss >> a;

            return glm::vec4{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
        }
	}
}