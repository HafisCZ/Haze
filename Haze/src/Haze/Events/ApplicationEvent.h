#pragma once

#include "Event.h"

#include <sstream>

namespace Haze {

	class HAZE_API WindowResizeEvent : public Event 
	{
		public:
			WindowResizeEvent(unsigned int width, unsigned int height) : _Width(width), _Height(height) { }

			inline unsigned int GetWidth() const { return _Width; }
			inline unsigned int GetHeight() const { return _Height; }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "WindowResizeEvent: " << _Width << ", " << _Height;
				return ss.str();
			}

			EVENT_CLASS_TYPE(WindowResize)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
		
		private:
			unsigned int _Width, _Height;
	};

	class HAZE_API WindowCloseEvent : public Event {
		public:
			WindowCloseEvent() {}

			EVENT_CLASS_TYPE(WindowClose)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZE_API AppTickEvent : public Event {
		public:
			AppTickEvent() {}

			EVENT_CLASS_TYPE(AppTick)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZE_API AppUpdateEvent : public Event {
		public:
			AppUpdateEvent() {}

			EVENT_CLASS_TYPE(AppUpdate)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class HAZE_API AppRenderEvent : public Event {
		public:
			AppRenderEvent() {}

			EVENT_CLASS_TYPE(AppRender)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}