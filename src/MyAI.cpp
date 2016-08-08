 // MyAI.cpp
#include "MyAI.hpp"
#include <math.h>
  
namespace wumpus_project
{
  MyAI::MyAI() : Agent()
  {
    // Class Attributes
    notShot = true;
    gold = false;
    lastAction = CLIMB;   
    facing = std::pair<int, int>(0,1);
    position = std::pair<int, int>(0,0);    
    worldDimension = 10;    
    bMoving = false;
    targetAction = FORWARD;
    targetDirection = 4;
    numVisitedSpaces = 0;
    numStay = 0;
	setOfPercepts = {};	
    initializeVisitedSpaces();
    for (int x = 0; x < INITSIZE; x++)
      for (int y = 0; y < INITSIZE; y++)
        AgentWorld[x][y] = 0;
    // ==================== IMPORTANT ==================== //
    // Fill these in with your name and student ID number. //
    idNumber = "51035780";		// 8 digit ID as a string
    author = "Grossman, Steven";		// Your name in that format
  } 

  void MyAI::initializeVisitedSpaces()
  { 
    // unvisited space notation = (-1,-1)
    for (int M = 1; M < INITSIZE * INITSIZE; ++M)
      visitedSpaces[M] = { -1, -1};
    // (0,0) always visited
    visitedSpaces[0] = { 0, 0 };
    numVisitedSpaces = 1;
  }  

  void MyAI::updateVisitedSpaces(int xCoord, int yCoord)
  {
	// if we haven't been to this space, store it in array
    if ((visitedSpaces[numVisitedSpaces - 1].first != xCoord) || (visitedSpaces[numVisitedSpaces - 1].second != yCoord))
    {              
       visitedSpaces[numVisitedSpaces] = {xCoord, yCoord}; 
       ++numVisitedSpaces;
    }
	
	// else we are staying in the same place
    else {
      numStay++;
    }
  }

  Agent::Action MyAI::getBackHome()
  {
    // when called from any square in AgentWorld
    // traverses coordinates in visitedSpaces in reverse
    // to get back to the entrance of the cave 
    Coordinate backTargetPosition = {visitedSpaces[numVisitedSpaces - 1].first + facing.first, visitedSpaces[numVisitedSpaces - 1].second + facing.second};        
   
    // if we are at the entrance, then climb out
    if ((position.first == 0) && (position.second == 0)) return CLIMB;
    // if we are facing the correct direction according to the next coordinate in our visitedSpaces array
    if ((backTargetPosition.first == visitedSpaces[numVisitedSpaces - 2].first) && (backTargetPosition.second == visitedSpaces[numVisitedSpaces - 2].second)) {
      numVisitedSpaces -= 1; 
      return FORWARD;
    }
    else {
      return TURN_RIGHT;
    }
  }
  
  Agent::Action MyAI::getAction()
  {    
    Agent::Action action;
    if(gold) { 
      return getBackHome();
    }
	// If the agent perceives a stench or breeze in the entrance of the cave, we climb out to avoid death.
    if ((checkPercept(Agent::STENCH) || checkPercept(Agent::BREEZE))
        && (position.first == 0 && position.second == 0)) {
      return getBackHome();
    }
    // This is a measure of how many times the same coordinate is encountered. 
	// Used in making a decision to return home if search is fruitless.
    if (numStay > 46) {
      return getBackHome();
    }
    if (checkPercept(Agent::GLITTER)) {       
      return GRAB;
    }
    else {
      if (takeAction(AgentWorld, position, action)) {
        return action;
      } 
      else {
        return getBackHome();
      }
    }    
  }
 

  bool MyAI::checkPercept(Agent::Percept percept)
  {
   // Checks to see if a particular percept is in the set of percepts for the agent.   
    bool found = false;
    std::set<Agent::Percept>::iterator iter;
    for ( iter = setOfPercepts.begin(); iter != setOfPercepts.end(); ++iter) {                        
      if (*iter == percept) {
        found = true;
       }
    }
    return found;
  }

  void MyAI::perceiveResult(std::set<Agent::Percept> percepts)
  {
    setOfPercepts = percepts;
    updateVisitedSpaces(position.first, position.second);   

    std::set<Agent::Percept>::iterator iter;

    if (testBit(AgentWorld, position, MAP_VISITED) == 0) {        

      //update the current position    
      for ( iter = percepts.begin(); iter != percepts.end(); ++iter)
      {      
        if (*iter == STENCH)  AgentWorld[position.first][position.second] |= MAP_STENCH;
        if (*iter == BREEZE)  AgentWorld[position.first][position.second] |= MAP_BREEZE;
        // If bump occurs to set worldDimension to to right or bottom coordinate 
        if (*iter == BUMP) { 
          if (facing.first == 0 && facing.second == 1)  { worldDimension = position.second + 1; } 
          else if (facing.first == 1 && facing.second == 0) { worldDimension = position.first + 1;}
        }
      }      

      AgentWorld[position.first][position.second] |= MAP_CURRENT;
      checkNeighbors(AgentWorld, position);     
    } 
 
  }

  Agent::Action MyAI::processBump(void)
  { 
    if (position.first == 0 && position.second == 0) {
      return CLIMB;
    }// facing right           
    else if ((facing.first == 0 && facing.second == 1) && (position.first == 0 && position.second == worldDimension - 1)) {
      return TURN_RIGHT;
    } // facing down
    else if ((facing.first == 1 && facing.second == 0) && (position.first == worldDimension - 1 && position.second == worldDimension - 1)) {      
      return TURN_RIGHT;
    } // facing up
    else if ((facing.first == -1 && facing.second == 0) && (position.first == 0)) { 
      return TURN_LEFT;
    }  // facing left
    else if ((facing.first == 0 && facing.second == -1) && (position.second == 0)) { 
      return TURN_RIGHT;
    }
  }

  void MyAI::calcNeighborCoordinates(Coordinate current, Coordinate neighborSpaces[4])
  {
    // One up
    neighborSpaces[0].first = current.first - 1;
    neighborSpaces[0].second = current.second;

    // One right
    neighborSpaces[1].first = current.first ;
    neighborSpaces[1].second = current.second + 1;

    // One down
    neighborSpaces[2].first = current.first + 1;
    neighborSpaces[2].second = current.second;

    // One left
    neighborSpaces[3].first = current.first;
    neighborSpaces[3].second = current.second - 1;
  }


  Agent::Action MyAI::moveOneCoord(int agentWorld[INITSIZE][INITSIZE], int direction, Coordinate srcCoord)
  {
    Coordinate temp;
    Agent::Action act = TURN_LEFT; 
    bMoving = true;  
    switch(direction) {      
      case 0:    // up         
        if (facing.first == 1 && facing.second == 0) {   // facing down
          act = TURN_RIGHT; 
        }
        else if (facing.first == 0 && facing.second == 1) {   // facing right
          act = TURN_LEFT;
        }
        else if (facing.first == -1 && facing.second == 0) {   // facing up
          act = FORWARD; bMoving = false; agentWorld[srcCoord.first][srcCoord.second] = agentWorld[srcCoord.first][srcCoord.second] & ~MAP_CURRENT;  
        }
        else if (facing.first == 0 && facing.second == -1) {// facing LEFT 
          act = TURN_RIGHT;                                   
        }
	break;      
      case 1:  // right	   
        if (facing.first == 1 && facing.second == 0) {   // facing down
          act = TURN_LEFT;
        }
        else if (facing.first == 0 && facing.second == 1) {   // facing right
          act = FORWARD; bMoving = false; agentWorld[srcCoord.first][srcCoord.second] = agentWorld[srcCoord.first][srcCoord.second] & ~MAP_CURRENT;  
        }
        else if (facing.first == -1 && facing.second == 0) {   // facing up
          act = TURN_RIGHT;
        }
        else if (facing.first == 0 && facing.second == -1) {// facing LEFT 
          act = TURN_RIGHT;                                   
        }
	break;
      // Down
      case 2:          
        if (facing.first == 1 && facing.second == 0) {   // facing down
          act = FORWARD; bMoving = false; agentWorld[srcCoord.first][srcCoord.second] = agentWorld[srcCoord.first][srcCoord.second] & ~MAP_CURRENT; 
        }
        else if (facing.first == 0 && facing.second == 1) {   // facing right
          act = TURN_RIGHT;
        }
        else if (facing.first == -1 && facing.second == 0) {   // facing up
          act = TURN_RIGHT;
        }
        else if (facing.first == 0 && facing.second == -1) {// facing LEFT 
          act = TURN_LEFT;                                   
        }
	break;
      // Left
      case 3:                
        if (facing.first == 1 && facing.second == 0) {   // facing down
          act = TURN_RIGHT;
        }
        else if (facing.first == 0 && facing.second == 1) {   // facing right
          act = TURN_RIGHT;
        }
        else if (facing.first == -1 && facing.second == 0) {   // facing up
          act = TURN_LEFT;
        }
        else if (facing.first == 0 && facing.second == -1) {// facing LEFT 
          act = FORWARD; bMoving = false; agentWorld[srcCoord.first][srcCoord.second] = agentWorld[srcCoord.first][srcCoord.second] & ~MAP_CURRENT;                            
        }
        break;
    }
    
    return act;

   }
  
  double MyAI::calcDistance(int agentWorld[INITSIZE][INITSIZE], Coordinate srcCoord, Coordinate destCoord)
  {
    double distance;
    distance = sqrt((double)pow((double)destCoord.first-srcCoord.first,2)+(double)pow((double)destCoord.second-srcCoord.second,2));
    return distance;
  }

  int MyAI::randVal(int min, int max, int *excluded, int noExcluded)
  {
    int result, i;
    result = rand() % (max - min + 1) + min;
    if (excluded != NULL) {
      for (i = 0; i < noExcluded; i++) {
        if (result == excluded[i]) {
	  result = randVal(min,max,excluded,noExcluded);
	  break;
	}
      }
    }
    return result;
  }

void MyAI::clrBit(int world[INITSIZE][INITSIZE], Coordinate current, int action)
  {
    // check to see if current is a valid coord	  
    if (current.first < 0 || current.second < 0)  {  
      return; 
    }

	// clear "action" bit at current pos
    world[current.first][current.second] = world[current.first][current.second] & ~action;
  }
  
  void MyAI::copyBits(int fromWorld[INITSIZE][INITSIZE], int toWorld[INITSIZE][INITSIZE], Coordinate current)
  {
	// check to see if current is a valid coord
    if (current.first < 0 || current.second < 0)  {
      return;
    }

	// copy "action" bit fromWorld->toWorld 
    toWorld[current.first][current.second] |= fromWorld[current.first][current.second];
  }
  
  int MyAI::testBit(int world[INITSIZE][INITSIZE], Coordinate current, int action)
  {
	// test if a current bit "action" is set at the current position
    if (((world[current.first][current.second] & action) == action)) {
      return 1;
    } 
    else {
      return 0;
    }
  }
  
  void MyAI::setBit(int world[INITSIZE][INITSIZE], Coordinate current, int action)
  {
	// valid coord check
    if (current.first < 0 || current.second < 0){  
      return;
    }
   
    // if we have visited a space, set appropriate marker bits
    if (action == MAP_VISITED) {
      clrBit(world, current, MAP_SAFE);
      clrBit(world, current, MAP_PITPOS);
      clrBit(world, current, MAP_WUMPUSPOS);
    }

	// set action bit at the current index of our world array
    world[current.first][current.second] = world[current.first][current.second] | action;
  }

  char* MyAI::FillArray(long int x,char *so, char fillChar)
  {
    char s[ARRAYSIZE+1];
    int i = ARRAYSIZE;
    s[i--]=0x00;   
    do
    { 
      s[i--] = (x & 1) ? '1':'0';
      x >>= 1;  
    } while( x > 0);
   
    while(i >= 0) s[i--] = fillChar;    
    sprintf(so,"%s",s);
    return so;
  }
 
 
  void MyAI::checkNeighbors(int agentWorld[INITSIZE][INITSIZE], Coordinate current)
  {
    if (testBit(agentWorld, current, MAP_VISITED) == 0) { // current position is a new position
      setBit(AgentWorld, position, MAP_VISITED);
      // get adjacent positions
      Coordinate neighbors[4];
      calcNeighborCoordinates(current, neighbors);
  

      // Check the four neighbors unless already SAFE and VISITED and set the cooresponding bits in the agentWorld array
      for (int index= 0; index< 4; index++) {
        if ((((neighbors[index].first >= 0) && (index== 0))
  	  ||((neighbors[index].second < worldDimension) && (index== 1))
  	  ||((neighbors[index].first  < worldDimension) && (index== 2))
  	  ||((neighbors[index].second >= 0) && (index== 3)))
	  &&(testBit(agentWorld, neighbors[index], MAP_VISITED) == 0) 
	  &&(testBit(agentWorld, neighbors[index], MAP_SAFE) == 0)) {
  
		  // if we perceive a stench from our current space
          if (testBit(agentWorld, current, MAP_STENCH)) {       
            if (testBit(agentWorld, neighbors[index], MAP_WUMPUSPOS)){             
              setBit(agentWorld, neighbors[index], MAP_WUMPUS);
              clrBit(agentWorld, neighbors[index], MAP_WUMPUSPOS);
			  // mark neighboring coordinates as possible Wumpus
            } 
            else {            
              setBit(agentWorld, neighbors[index], MAP_WUMPUSPOS);
            }
          }
			
		  // if we perceive a breeze from our current pos
          if (testBit(agentWorld, current, MAP_BREEZE)) {         
            if (testBit(agentWorld, neighbors[index], MAP_PITPOS)) {          
	      setBit(agentWorld, neighbors[index], MAP_PIT);
		  // mark neighboring coordinates as possible Pit
	    }
            else {             
      	      setBit(agentWorld, neighbors[index], MAP_PITPOS);
	    }
          }
		  // if there is no stench and no breeze in our current square
          if ((testBit(agentWorld, current, MAP_STENCH) == 0) &&
             (testBit(agentWorld, current, MAP_BREEZE) == 0)) 
          {
			// the neighbors are safe
            setBit(agentWorld, neighbors[index], MAP_SAFE);
	    clrBit(agentWorld, neighbors[index], MAP_WUMPUS);
	    clrBit(agentWorld, neighbors[index], MAP_PIT);
	    clrBit(agentWorld, neighbors[index], MAP_WUMPUSPOS);
	    clrBit(agentWorld, neighbors[index], MAP_PITPOS);
          }
        }
      }
    }
  }
  

  int MyAI::takeAction(int agentWorld[INITSIZE][INITSIZE], Coordinate current, Agent::Action& result)
  {
    int random;
    Coordinate neighbors[4];		
   
    if (bMoving) {
	  // We are already facing the correct space
      result = moveOneCoord(agentWorld, targetDirection, current);    
      return 1;
    }
   
    // acquire neighboring spaces' coordinates
    calcNeighborCoordinates(current, neighbors);
  
    // these arrays refer to the spaces that are not safe and should not be moved to
    int excluded_i;			
    int excluded_size;			
    int *excl_safe_coord = NULL;		
    int *excl_safe_coord_temp = NULL;	
  
    excluded_i = 0;
    excluded_size = 0;
  
    
    for (int index = 0; index< 4; index++) {
      if ((((neighbors[index].first < 0) && (index == 0))
  	||((neighbors[index].second > worldDimension - 1) && (index == 1))
  	||((neighbors[index].first > worldDimension - 1) && (index == 2))
  	||((neighbors[index].second < 0) && (index == 3)))
  	||(testBit(agentWorld, neighbors[index], MAP_VISITED))
  	||(testBit(agentWorld, neighbors[index], MAP_SAFE) == 0)) {
        if (excluded_i == 0) {
  	  excluded_size++;
  	  if ((excl_safe_coord = (int*)calloc(excluded_size, sizeof(int))) == NULL) {
		    // error allocating memory
            return 0;
          }
	}
        else if (excluded_i == excluded_size) {
  	  excluded_size++;
  	  if ((excl_safe_coord_temp = (int*)realloc(excl_safe_coord, excluded_size * sizeof(int))) == NULL) {
  	    free(excl_safe_coord);
		    // error allocating memory
            return 0;
  	  }
  	  excl_safe_coord = excl_safe_coord_temp;
  	}
  	 
        excl_safe_coord[excluded_i] = index;
	  excluded_i++;
      }
    }     
  
    // if there is at least one safe space adjacent to us
    if (excluded_i < 4) {            
      targetDirection = randVal(0,3,excl_safe_coord,excluded_i);   
      result = moveOneCoord(agentWorld, targetDirection, current);
      free(excl_safe_coord);
      return 1;
    }
    else {   
	  // else we have to find the nearestSafeCoordinate
      free(excl_safe_coord);
      int x,y,direction;
      Coordinate nearestSafeCoordCoord = { -1, -1 };

      for (x = 0; x < worldDimension; x++) {
        for (y = 0; y < worldDimension; y++) {
  	  Coordinate verifySafeCoord = { x, y };
  	  if (testBit(agentWorld, verifySafeCoord, MAP_SAFE)) {
  	    if ((nearestSafeCoordCoord.first == -1) && (nearestSafeCoordCoord.second == -1)) {
  	      nearestSafeCoordCoord = verifySafeCoord;
  	    }
            else {
  	      if (calcDistance(agentWorld, current, verifySafeCoord) < calcDistance(agentWorld, current, nearestSafeCoordCoord)) {
	        nearestSafeCoordCoord = verifySafeCoord;
	      }
	    }
	  }
        }
      }

     
      if ((nearestSafeCoordCoord.first == -1) && (nearestSafeCoordCoord.second == -1)) {
        // if we are not at a valid position on the game board, return error		  
        return 0;
      } 
      else {
		// direction we are facing 
        targetDirection = calcDirection(agentWorld, current, nearestSafeCoordCoord);
	result = moveOneCoord(agentWorld, targetDirection, current);
	return 1;
      }
    }
  }


  int MyAI::calcDirection(int agentWorld[INITSIZE][INITSIZE], Coordinate srcCoord, Coordinate destCoord)
  {
	// Calculates the direction our agent needs to be facing in order to 
	// move from srcCoord to destCoord
    int index, i, shortestSafeNeighbor;
    double shortestDist;
    Coordinate neighbors[4];
    int safeNeighbors_i;				
    int safeNeighbors_size;		
    int *safeNeighbors = NULL;			
    int *safeNeighbors_temp = NULL;	
    double *distances = NULL;			
    double *distances_temp = NULL;	
    calcNeighborCoordinates(srcCoord, neighbors);
    safeNeighbors_i = 0;
    safeNeighbors_size = 0;
	// Iterate through the neighbors of srcCoord to determine the direction we need to move in
    for (index = 0; index < 4; index++) {
      if((((neighbors[index].first < worldDimension) && (index == 0))
	||((neighbors[index].second < worldDimension) && (index == 1))
	||((neighbors[index].first >= 0) && (index == 2))
	||((neighbors[index].second >= 0) && (index == 3)))
	&&(testBit(agentWorld, neighbors[index], MAP_VISITED))) {
        if (safeNeighbors_i == 0) {
	  safeNeighbors_size++;
	  if ((safeNeighbors = (int*)calloc(safeNeighbors_size, sizeof(int))) == NULL) {
		// error allocating memory
	    return 0;
	  }
	  if ((distances = (double*)calloc(safeNeighbors_size, sizeof(double))) == NULL) {
		// error allocating memory
	    return 0;
	  }
	} 
        else if (safeNeighbors_i == safeNeighbors_size) {
          safeNeighbors_size++;
	  if ((safeNeighbors_temp = (int*)realloc(safeNeighbors, safeNeighbors_size * sizeof(int))) == NULL) {
	    free(safeNeighbors);
		// error allocating memory
	    return 0;
	  }
          
          if ((distances_temp = (double*)realloc(distances, safeNeighbors_size * sizeof(double))) == NULL) {
	    free(distances);
		// error allocating memory
	    return 0;
	  }
	  safeNeighbors = safeNeighbors_temp;
          distances = distances_temp;
	}
	    // update the safeNeighbors array to the current index
        safeNeighbors[safeNeighbors_i] = index;
		// distance from neighbor to destCoord stored in distances array
        distances[safeNeighbors_i] = calcDistance(agentWorld,neighbors[index], destCoord);
	safeNeighbors_i++;
      }
    }

    if (safeNeighbors != 0) {
	  // If we have at least 1 safeNeighbor 
      shortestSafeNeighbor = safeNeighbors[0];
      shortestDist = distances[0];
	  // Iterate to find the most optimal move (least amount of distance)
      for (i = 1; i < safeNeighbors_i; i++) {
        if (distances[i] < shortestDist) {
	  shortestSafeNeighbor = safeNeighbors[i];
	  shortestDist = distances[i];
	}
      }
    }
    
    free(safeNeighbors);
    free(distances);
    return shortestSafeNeighbor;
  }


  }
