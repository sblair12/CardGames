// Player.cpp
//
// Authors: 
// Shane Blair	sblair@wustl.edu
// Steven Lee	lee.steven@wustl.edu
//
// April 6, 2018
//
// Purpose- Declarations for a "Player" struct

#include "stdafx.h"
#include "Player.h"

#include <fstream>
#include <sstream>
using namespace std;

Player::Player(char * file_name)
{
	name = file_name;
	wins = 0;
	losses = 0;
	ifstream ifs(name);

	if (ifs.is_open()) {
		string line_1, player_string;
		int player_score;
		getline(ifs, line_1);
		if (!line_1.empty()) {
			istringstream iss(line_1);
			iss >> player_string;
			if (player_string == name) {
				iss >> player_score;
				wins = player_score;
				iss >> player_score;
				losses = player_score;
			}
		}
	}
}

ostream & operator<<(ostream &o, const Player &p)
{
	o << p.name << " " << p.wins << " " << p.losses;
	return o;
}