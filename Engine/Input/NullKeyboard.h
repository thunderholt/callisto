#pragma once
#ifdef ANDROID

#include "Include/Callisto.h"

class NullKeyboard : public IKeyboard
{
public:
	NullKeyboard();
	virtual ~NullKeyboard();
	virtual void Init();
	virtual bool GetIsActive();
	virtual KeyState GetKeyState(char key);
	//virtual double GetKeyDownTimestamp(char key);
	virtual int GetNumberOfEvents();
	virtual KeyEvent* GetEvent(int index);
	virtual void ClearEvents();
};

#endif