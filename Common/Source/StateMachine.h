#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

class StateMachine
{
public:
	enum State
	{
		IDLE,
		ATTACK,
		WALK,
		JUMP,
		FALLING,
		FALL,
		LADDER,
		NUM_STATES,
	};

	StateMachine();
	~StateMachine(){}

	bool state[NUM_STATES];
	
	bool resetStates();
	bool setState(State s);
	bool isState(State s);
};

#endif