#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsKeyboard : public IKeyboard
{
public:
	WindowsKeyboard();
	virtual ~WindowsKeyboard();
	virtual void Init();
	virtual bool GetIsActive();
	virtual KeyState GetKeyState(char key);
	//virtual double GetKeyDownTimestamp(char key);
	virtual int GetNumberOfEvents();
	virtual KeyEvent* GetEvent(int index);
	virtual void ClearEvents();
	void RaiseKeyDownEvent(char key);
	void RaiseKeyUpEvent(char key);


private:
	//double keyDownTimestamps[128];
	KeyState keyStates[128];
	DynamicLengthArray<KeyEvent> keyEvents;
};

#endif