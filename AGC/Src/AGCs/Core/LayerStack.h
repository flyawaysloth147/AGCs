#pragma once

#include "AGCs/Core/Layer.h"

#include <vector>

namespace AGC {
	class LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		inline std::vector<Layer*>::iterator begin() { return m_stack.begin(); }
		inline std::vector<Layer*>::iterator end() { return m_stack.end(); }

	private:
		std::vector<Layer*> m_stack;
		std::vector<Layer*>::iterator m_insertPoint;
	};
}