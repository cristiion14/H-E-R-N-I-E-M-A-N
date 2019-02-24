#include"KeyboardEvent.h"


//-constuctor which generates an invalid argument definition
KeyboardEvent::KeyboardEvent()
	:
	type(EventType::Invalid),
	key(0u)
{}
//-default constructor definition
KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char key)
	:
	type(type),
	key(key)
{}

//-definition of the event checker functions
bool KeyboardEvent::isPressed() const
{
	return this->type == EventType::Press;
}
bool KeyboardEvent::isReleased() const
{
	return this->type == EventType::Released;
}
bool KeyboardEvent::isValid() const
{
	return this->type != EventType::Invalid;
}
//-def of the get key func
unsigned char KeyboardEvent::GetKeyCode() const
{
	return this->key;
}
