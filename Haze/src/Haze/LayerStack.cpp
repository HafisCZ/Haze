#include "hzpch.h"
#include "LayerStack.h"

namespace Haze 
{

	LayerStack::LayerStack() 
	{
		_LayerInsert = _Layers.begin();
	}

	LayerStack::~LayerStack() 
	{
		for (Layer* layer : _Layers) 
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer) 
	{
		_LayerInsert = _Layers.emplace(_LayerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer) 
	{
		auto it = std::find(_Layers.begin(), _Layers.end(), layer);
		
		if (it != _Layers.end()) 
		{
			_Layers.erase(it);
			_LayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay) 
	{
		auto it = std::find(_Layers.begin(), _Layers.end(), overlay);

		if (it != _Layers.end()) {
			_Layers.erase(it);
		}
	}
}