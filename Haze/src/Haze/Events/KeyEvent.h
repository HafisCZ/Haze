#pragma once

#include "Event.h"

namespace Haze 
{

	class  KeyEvent : public Event
	{
		public:
			inline int GetKeyCode() const { return _KeyCode; }
			
			EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

		protected:
			KeyEvent(int keycode) : _KeyCode(keycode) { }

			int _KeyCode;
	};

	class  KeyPressedEvent : public KeyEvent
	{
		public:
			KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), _RepeatCount(repeatCount) { }

			inline int GetRepeatCount() const { return _RepeatCount; }

			std::string ToString() const override 
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << _KeyCode << " (" << _RepeatCount << " repeats)";
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyPressed)

		private:
			int _RepeatCount;
	};

	class  KeyTypedEvent : public KeyEvent {
		public:
			KeyTypedEvent(int keycode) : KeyEvent(keycode) { }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "KeyTypedEvent: " << _KeyCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyTyped)
	};

	class  KeyReleasedEvent : public KeyEvent {
		public:
			KeyReleasedEvent(int keycode) : KeyEvent(keycode) { }

			std::string ToString() const override {
				std::stringstream ss;
				ss << "KeyReleasedEvent: " << _KeyCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyReleased)
	};
}