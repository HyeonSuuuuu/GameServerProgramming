#pragma once

#include <glm/glm.hpp>
#include <cstdint>

namespace lve {
	// 헥사코드를 glm::vec3 컬러로 변환해주는 헬퍼함수
	inline glm::vec3 hexToColor(uint32_t hex) {
		glm::vec3 srgbColor{
			((hex >> 16) & 0xFF) / 255.0f,
			((hex >> 8) & 0xFF) / 255.0f,
			((hex) & 0xFF) / 255.0f
		};
		// sRGB -> Linear 변환 (대략 2.2 감마 역함수)
		return glm::pow(srgbColor, glm::vec3(2.2f));
	}
}
