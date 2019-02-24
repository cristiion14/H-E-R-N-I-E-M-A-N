#pragma once
//keyboard event class
class KeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Released,
		Invalid
	};
	//-constructor to generate invalid event
	KeyboardEvent();

	//-default constructor
	KeyboardEvent(EventType type, unsigned char key);

	bool isPressed() const;
	bool isReleased() const;
	bool isValid() const;
	unsigned char GetKeyCode() const;

	//-store the event type and key
private:
	EventType type;
	unsigned char key;

};