// Player.h
//
// Authors: 
// Shane Blair	sblair@wustl.edu
// Steven Lee	lee.steven@wustl.edu
//
// April 6, 2018
//
// Purpose- Definitions for a "Player" struct
#pragma once
#include "Hand.h"

#include <string>
using namespace std;

struct Player {
	string name;
	Hand hand;
	unsigned int wins;
	unsigned int losses;

	Player(char *);
};

ostream & operator<<(ostream &, const Player &);