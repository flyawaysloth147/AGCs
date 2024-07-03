#include "AGCpch.h"

#include "LayerStack.h"

namespace AGC {
	LayerStack::LayerStack() {
		m_insertPoint = m_stack.begin();
	}

	LayerStack::~LayerStack() {
		for (auto layer : m_stack) {
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer) {
		m_insertPoint = m_stack.emplace(m_insertPoint, layer);
	}

	void LayerStack::PopLayer(Layer* layer) {
		std::vector<Layer*>::iterator it = std::find(m_stack.begin(), m_stack.end(), layer);
		if (it != m_stack.end())
			m_stack.erase(it);
	}

	void LayerStack::PushOverlay(Layer* layer) {
		m_stack.emplace_back(layer);
	}

	void LayerStack::PopOverlay(Layer* layer) {
		PopLayer(layer);
	}
}