 // MyAI.hpp

#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"

#define ARRAYSIZE       10
#define INITSIZE        10

#define MAP_WUMPUS      512
#define MAP_PIT         256
#define MAP_GLITTER     128
#define MAP_WUMPUSPOS   64
#define MAP_PITPOS      32
#define MAP_BREEZE      16
#define MAP_STENCH      8
#define MAP_VISITED     4
#define MAP_SAFE        2
#define MAP_CURRENT     1

namespace wumpus_project
{
  class MyAI : public Agent 
  {
  public:
    MyAI();
    virtual Agent::Action getAction(void);
    virtual void perceiveResult(std::set<Agent::Percept> percepts); 
  private:
    Agent::Action processBump();    
    void checkNeighbors(int agentWorld[INITSIZE][INITSIZE], Coordinate current);
    void calcNeighborCoordinates(Coordinate current, Coordinate results[4]);
    void copyBits(int fromWorld[INITSIZE][INITSIZE], int toWorld[INITSIZE][INITSIZE], Coordinate current);
    void clrBit(int world[INITSIZE][INITSIZE], Coordinate current, int action);
    int testBit(int world[INITSIZE][INITSIZE], Coordinate current, int action);
    void setBit(int world[INITSIZE][INITSIZE], Coordinate current, int action);
    bool checkPercept(Agent::Percept percept); 
    char* FillArray(long int x,char *so, char fillChar);
    int takeAction(int agentWorld[INITSIZE][INITSIZE], Coordinate current, Agent::Action& result);
    int randVal(int min, int max, int *excluded, int noExcluded);
    int calcDirection(int agentWorld[INITSIZE][INITSIZE], Coordinate srcCoord, Coordinate destCoord);
    double calcDistance(int agentWorld[INITSIZE][INITSIZE], Coordinate srcCoord, Coordinate destCoord);
    Agent::Action moveOneCoord(int agentWorld[INITSIZE][INITSIZE], int direction, Coordinate srcCoord);
    void initializeVisitedSpaces();
    void updateVisitedSpaces(int xCoord, int yCoord);
    Agent::Action getBackHome();
    std::set<Agent::Percept> setOfPercepts;   
    int worldDimension;
    int numVisitedSpaces;
    int AgentWorld[INITSIZE][INITSIZE];    
    Coordinate lastPosition;
    int numStay;
    Agent::Action targetAction; 
    Coordinate visitedSpaces[INITSIZE*INITSIZE];
    int targetDirection; 
    bool bMoving;
  };
}

#endif /* MYAI_LOCK */
