#pragma once

#include "Event.h"

namespace Haze 
{

	class  MouseMovedEvent : public Event {
		public:
			MouseMovedEvent(float x, float y) : _MouseX(x), _MouseY(y) { }

			inline float GetX() const { return _MouseX; }
			inline float GetY() const { return _MouseY; }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "MouseMovedEvent: " << _MouseX << ", " << _MouseY;
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
		
		private:
			float _MouseX, _MouseY;
	};

	class  MouseScrolledEvent : public Event {
		public:
			MouseScrolledEvent(float xOffset, float yOffset) : _XOffset(xOffset), _YOffset(yOffset) { }

			inline float GetXOffset() const { return _XOffset; }
			inline float GetYOffset() const { return _YOffset; }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
				return ss.str();
			}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

		private:
			float _XOffset, _YOffset;
	};

	class  MouseButtonEvent : public Event {
		public:
			inline int GetMouseButton() const { return m_Button; }

			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

		protected:
			MouseButtonEvent(int button) : m_Button(button) { }

			int m_Button;
	};

	class  MouseButtonPressedEvent : public MouseButtonEvent {
		public:
			MouseButtonPressedEvent(int button)	: MouseButtonEvent(button) { }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "MouseButtonPressedEvent: " << m_Button;
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class  MouseButtonReleasedEvent : public MouseButtonEvent {
		public:
			MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) { }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "MouseButtonReleasedEvent: " << m_Button;
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}