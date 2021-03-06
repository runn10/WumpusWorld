// Agent.hpp

#ifndef AGENT_LOCK
#define AGENT_LOCK

#include <iostream>
#include <set>

namespace wumpus_project
{
	typedef std::pair<int,int> Coordinate;

	// An abstract class of agent
	// Need to overwrite two functions, getAction() and perceiveResult()
	class Agent
	{
	public:
		bool 		notShot;		// True if can shoot (Only for agent records)
		bool 		gold;			// Ture if has gold (Only for agent records)
		Coordinate 	facing;			// Agent's direction (Only for agent records)
		Coordinate 	position;		// Agent's position (Only for agent records)
		
		enum Action		{TURN_LEFT, TURN_RIGHT, FORWARD, SHOOT, GRAB, CLIMB};
		enum Percept	{STENCH, BREEZE, BUMP, GLITTER, SCREAM};
		
		Action lastAction;
		
		Agent( void ) {};
		
		virtual Action 	getAction ( void ) = 0;
		virtual void 	perceiveResult( std::set<Percept> percepts ) = 0;
		
		std::string getID ( void ) { return idNumber; }
	
	protected:
		std::string author;
		std::string idNumber;
	};
}

#endif /* AGENT_LOCK */
