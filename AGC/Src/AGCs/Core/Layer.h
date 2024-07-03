#pragma once

namespace AGC {
	class Layer {
	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt = 1.0f) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};
}