#include "StateMachine.h"

StateMachine::StateMachine()
{
	setState(IDLE);
}
bool StateMachine::setState(State s)
{
	if (isState(s))
		return false;

	if (resetStates())
		state[s] = true;

	return true;
}
bool StateMachine::isState(State s)
{
	return state[s];
}
bool StateMachine::resetStates()
{
	for (unsigned i = 0; i < NUM_STATES; ++i)
	{
		state[i] = false;
	}
	return true;
}