// FiveCardDraw.cpp
//
// Authors: 
// Shane Blair	sblair@wustl.edu
// Steven Lee	lee.steven@wustl.edu
//
// April 6, 2018
//
// Purpose- Declarations for the FiveCardDraw class derived from "Game" publically

#include "stdafx.h"
#include "FiveCardDraw.h"
#include "Card.h"
#include "Parse.h"
#include "Hand.h"

#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

const int max_discard = 5;
const int min_discard = 0;
const string invalid_string = "*STRINGINVALID*";

FiveCardDraw::FiveCardDraw()
{
	dealer_index = 0;
	draw_deck = Deck();
	discard_deck = Deck();
	for (int card_rank = TWO; card_rank <= ACE; ++card_rank) {
		for (int card_suit = CLUBS; card_suit <= SPADES; ++card_suit) {
			Card card;
			card.rank = (Rank)card_rank;
			card.suit = (Suit)card_suit;
			draw_deck.add_card(card);
		}
	}
}

void FiveCardDraw::remove_cards(vector<size_t> &to_remove, Player &player)
{
	sort(to_remove.begin(), to_remove.end());
	cout << "Discarding ";
	for (size_t i = to_remove.size() - 1; i >= 0 && i < to_remove.size(); --i) {
		Card card = player.hand.remove_card(to_remove[i]);
		cout << card << " ";
		discard_deck.add_card(card);
	}
	cout << endl;
}

void FiveCardDraw::bet()
{
	size_t bet_index = (dealer_index + 1) % ptr_vector.size();
	unsigned int bet_round = 0;
	unsigned int current_bet = 0;

	while (bet_round < ptr_vector.size()) {
		string input;
		string substring = "invalid";
		unsigned int bet_amount = 0;
		unsigned int correct_length;

		cout << ptr_vector[bet_index]->name << "\t" << ptr_vector[bet_index]->hand << endl;
		cout << "Chips: " << ptr_vector[bet_index]->chips << "\t";

		if (ptr_vector[bet_index]->fold == false && ptr_vector[bet_index]->chips > 0) {
			while (substring != "check" && substring != "bet" && substring != "fold" && substring != "raise" && substring != "call") {
				if (current_bet == 0) {
					cout << endl;
					cout << endl;
					cout << "Would you like to 'check' or 'bet'? (ex: 'bet 2', the max bet is 2 chips)" << endl;
					getline(cin, input);
					if (input == "check") {
						substring = input;
					}
					if (input.substr(0, 3) == "bet") {
						correct_length = 5;
						substring = "bet";
						if (input.length() == 5) {
							istringstream bet_stream(input.substr(4, 5));
							bet_stream >> bet_amount;
							if (bet_amount <= 2 && bet_amount > 0 && bet_amount <= ptr_vector[bet_index]->chips) {
								current_bet = bet_amount;
								ptr_vector[bet_index]->bet = current_bet;
								bet_round = 0;
							}
							else if (bet_amount > ptr_vector[bet_index]->chips) {
								cout << "Invalid bet: You only have " << ptr_vector[bet_index]->chips << " chip(s) to bet" << endl;
								substring = "invalid";
							}
							else {
								cout << "Invalid bet: The maximum bet is 2 chips" << endl;
								substring = "invalid";
							}
						}
						else {
							cout << "Invalid bet: Bet must be either 1 or 2 chips" << endl;
							substring = "invalid";
						}
					}
					if (input == "raise") {
						substring = "invalid";
					}
				}
				else {
					cout << "Current Bet: " << current_bet << endl;
					cout << endl;
					cout << "Would you like to 'fold', 'raise', or 'call'? (ex: 'raise 2', the max raise is 2 chips)" << endl;
					getline(cin, input);
					if (input == "fold") {
						ptr_vector[bet_index]->fold = true;
						substring = input;
					}
					if (input == "call") {
						substring = input;
					}
					if (input.substr(0, 5) == "raise") {
						correct_length = 7;
						substring = "raise";
						if (input.length() == 7) {
							istringstream bet_stream(input.substr(6, 7));
							bet_stream >> bet_amount;
							if (bet_amount <= 2 && bet_amount > 0 && ((bet_amount + current_bet) <= ptr_vector[bet_index]->chips)) {
								current_bet += bet_amount;
								ptr_vector[bet_index]->bet = current_bet;
								bet_round = 0;
							}
							else if ((bet_amount + current_bet) > ptr_vector[bet_index]->chips) {
								cout << "Invalid raise: You only have " << ptr_vector[bet_index]->chips << " chip(s) to bet" << endl;
								substring = "invalid";
							}
							else {
								cout << "Invalid raise: The maximum raise is 2 chips" << endl;
								substring = "invalid";
							}
						}
						else {
							cout << "Invalid raise: Raise must be 1 or 2 chips" << endl;
							substring = "invalid";
						}
					}
				}
			}
		}
		else if (ptr_vector[bet_index]->chips == 0) {
			cout << endl;
			cout << "Already all in" << endl;
		}
		else {
			cout << endl;
			cout << "Folded" << endl;
		}
		bet_index = (bet_index + 1) % ptr_vector.size();
		bet_round++;
		cout << endl;
	}

	//Take bets from players not folded
	for (int i = 0; i < ptr_vector.size(); ++i) {
		if (ptr_vector[i]->fold == false) {
			if (current_bet <= ptr_vector[i]->chips) {
				ptr_vector[i]->chips -= current_bet;
				pot += current_bet;
			}
			else {
				pot += ptr_vector[i]->chips;
				ptr_vector[i]->chips = 0;
			}
		}
		else {
			pot += ptr_vector[i]->bet;
			ptr_vector[i]->chips -= ptr_vector[i]->bet;
		}
		ptr_vector[i]->bet = 0;
	}
	cout << "Pot: " << pot << endl;
}

void FiveCardDraw::print_rankings()
{
	for (int i = 0; i < ptr_vector.size(); ++i) {
		rankHand(ptr_vector[i]->hand);
	}
	vector<shared_ptr<Player>> ptr_temp = ptr_vector;
	sort(ptr_temp.begin(), ptr_temp.end(), poker_rank_ptr);
	ptr_temp[0]->wins++;
	ptr_temp[0]->chips += pot;
	if (ptr_temp[0]->computer) {
		cout << ptr_temp[0]->name << "\t(Computer)";
		ptr_temp[0]->position = 2;
	}
	else {
		cout << ptr_temp[0]->name;
	}
	cout << "\t won " << pot << " chips!" << endl;
	cout << "Wins:\t" << ptr_temp[0]->wins << "\tLosses: " << ptr_temp[0]->losses << "\tChips: " << ptr_temp[0]->chips << endl;
	cout << ptr_temp[0]->hand << "\t" << poker_text[ptr_temp[0]->hand.get_poker()] << endl;

	for (int i = 1; i < ptr_temp.size(); ++i) {
		if (i == ptr_temp.size() - 1) {
			ptr_temp[i]->position = 0;
		}
		cout << endl;
		ptr_temp[i]->losses++;
		if (ptr_temp[i]->computer) {
			cout << ptr_temp[i]->name << "\t(Computer)" << endl;
		}
		else {
			cout << ptr_temp[i]->name << endl;
		}
		cout << "Wins:\t" << ptr_temp[i]->wins << "\tLosses: " << ptr_temp[i]->losses << "\tChips: " << ptr_temp[i]->chips << endl;
		if (ptr_temp[i]->fold == false) {
			cout << ptr_temp[i]->hand << "\t" << poker_text[ptr_temp[i]->hand.get_poker()] << endl;
		}
		else {
			cout << "FOLD" << endl;
		}
	}
	//Reset fold bools to false
	for (int i = 0; i < ptr_vector.size(); ++i) {
		ptr_vector[i]->fold = false;
	}
}


int FiveCardDraw::before_turn(Player &player)
{
	cout << endl;
	cout << endl;
	cout << player.name << "\t" << player.hand << endl;
	cout << endl;
	cout << endl;
	cout << "How many cards to discard?" << endl;
	string input;
	int number_discarded;

	if (player.computer) {
		number_discarded = computer_number_discarded(player);
	}
	else {
		//Get number of cards to discard
		while (input.length() != 1 || number_discarded > max_discard || number_discarded < min_discard) {
			cout << "Please give a VALID number of cards to discard, from 0 to 5" << endl;
			getline(cin, input);
			if (input.length() == 1) {
				istringstream iss(input);
				iss >> number_discarded;
			}
		}
	}

	cout << endl;

	if (player.computer) {
		if (number_discarded == max_discard) {
			cout << "Discarding all cards" << endl;
			vector<size_t> computer_remove = { 0,1,2,3,4 };
			remove_cards(computer_remove, player);
		}
		else if (number_discarded == min_discard) {
			cout << "No cards will be discarded" << endl;
		}
		else {
			vector<size_t> computer_remove = computer_discard(player);
			remove_cards(computer_remove, player);
		}
	}
	else {
		const int valid_string_length = 2 * number_discarded - 1;
		size_t number;
		input = invalid_string;


		//Discard those cards from the Player hand
		if (number_discarded < max_discard && number_discarded > 0) {
			if (number_discarded == 1) {
				cout << "What card to discard? (indices 1-5)" << endl;
			}
			else {
				cout << "Which cards to discard? (indices 1-5, separate choices by spaces, ex: 1 2 4)" << endl;
			}
			while (input.length() != valid_string_length) {
				number = 1000;
				vector<size_t> to_remove;
				cout << "Please give valid card indices, from 1 to 5" << endl;
				getline(cin, input);
				if (input.length() == valid_string_length) {
					istringstream iss_2(input);
					int i;
					for (i = 0; iss_2 >> number; ++i) {
						if (number > max_discard || number < min_discard + 1) {
							cout << "Invalid index: " << number << endl;
							input = invalid_string;
							break;
						}
						else {
							for (int j = 0; j < to_remove.size(); ++j) {
								if (number - 1 == to_remove[j]) {
									cout << "Invalid index: " << number << endl;
									input = invalid_string;
									break;
								}
							}
							to_remove.push_back(number - 1);
						}
					}
					if (number == 1000 || i != number_discarded) {
						cout << "Invalid index value" << endl;
						input = invalid_string;
					}
					if (input.length() == valid_string_length && to_remove.size() > 0) {
						remove_cards(to_remove, player);
					}
				}
			}
		}

		//Discard ALL cards
		else if (number_discarded == max_discard) {
			cout << "Discarding all cards" << endl;
			vector<size_t> to_remove = { 0,1,2,3,4 };
			remove_cards(to_remove, player);
		}

		//Discard NO cards
		else {
			cout << "No cards will be discarded" << endl;
		}
	}

	return 0;
}

int FiveCardDraw::turn(Player &player)
{
	for (int i = player.hand.size(); i < max_discard; ++i) {
		if (draw_deck.size() == 0) {
			if (discard_deck.size() == 0) {
				cout << "No more cards in the deck" << endl;
				int i = deck_out_of_cards;
				throw i;
			}
			discard_deck.shuffle();
			swap(draw_deck, discard_deck);
		}
		player.hand << draw_deck;
	}
	return 0;
}

int FiveCardDraw::after_turn(Player &player)
{
	//deprecated by betting rounds
	return 0;
}

int FiveCardDraw::before_round()
{
	cout << endl;
	ante();

	draw_deck.shuffle();
	int card_count = 0;
	size_t index = (dealer_index + 1) % ptr_vector.size();
	while (card_count < max_discard) {
		if (draw_deck.size() == 0) {
			cout << "No more cards in the deck" << endl;
			int i = deck_out_of_cards;
			throw i;
		}
		ptr_vector[index]->hand << draw_deck;
		index = (index + 1) % ptr_vector.size();
		if (index == (dealer_index + 1) % ptr_vector.size()) {
			++card_count;
		}
	}

	cout << endl;
	bet();
	int play_count = 0;
	for (size_t i = 0; i < ptr_vector.size(); i++) {
		if (!(ptr_vector[i]->fold)) {
			play_count++;
		}
	}
	if (play_count == 1) {
		all_fold = true;
	}

	if (!all_fold) {
		for (size_t i = 0; i < ptr_vector.size(); ++i) {
			try {
				if (ptr_vector[index]->fold == false) {
					before_turn(*ptr_vector[index].get());
				}
				else {
					cout << endl;
					cout << endl;
					cout << ptr_vector[index]->name << "\t" << "FOLD" << endl;
					cout << endl;
					cout << endl;
				}
				index = (index + 1) % ptr_vector.size();
			}
			catch (int i) {
				throw i;
			}
		}
	}
	return 0;
}

int FiveCardDraw::round()
{
	if (!all_fold) {
		size_t index = (dealer_index + 1) % ptr_vector.size();
		for (size_t i = 0; i < ptr_vector.size(); ++i) {
			try {
				turn(*ptr_vector[index].get());
				index = (index + 1) % ptr_vector.size();
			}
			catch (int i) {
				throw i;
			}
		}
		cout << endl;
		cout << endl;
		bet();
	}
	return 0;
}

int FiveCardDraw::after_round()
{
	cout << endl;
	print_rankings();

	//Take cards from hands and throw them into the draw deck
	for (int i = 0; i < ptr_vector.size(); ++i) {
		for (int index = ptr_vector[i]->hand.size() - 1; index >= 0 && index < ptr_vector[i]->hand.size(); --index) {
			Card card = ptr_vector[i]->hand.remove_card(index);
			draw_deck.add_card(card);
		}
	}

	//Take cards from discard deck and throw them into the draw deck
	draw_deck.deck.insert(draw_deck.deck.end(), discard_deck.deck.begin(), discard_deck.deck.end());
	discard_deck = Deck();

	cout << endl;
	cout << endl;

	chips_empty();

	try {
		players_leave();
		players_join();
	}
	catch (int i) {
		throw i;
	}

	if (ptr_vector.size() != 0) {
		dealer_index = (dealer_index + 1) % ptr_vector.size();
	}
	return 0;
}

int FiveCardDraw::computer_number_discarded(Player p)
{
	int number_discarded;
	rankHand(p.hand);
	if (p.hand.get_poker() == straight_flush || p.hand.get_poker() == full_house || p.hand.get_poker() == straight || p.hand.get_poker() == flush_rank) {
		number_discarded = 0;
	}
	else if (p.hand.get_poker() == four_kind || p.hand.get_poker() == two_pair) {
		number_discarded = 1;
	}
	else if (p.hand.get_poker() == three_kind) {
		number_discarded = 2;
	}
	else if (p.hand.get_poker() == one_pair) {
		number_discarded = 3;
	}
	else if (p.hand.get_poker() == high_card) {
		if (p.hand[4].rank == ACE) {
			number_discarded = 4;
		}
		else {
			number_discarded = 3;
		}
	}
	cout << number_discarded << "\t*Computer*" << endl;
	return number_discarded;
}

vector<size_t> FiveCardDraw::computer_discard(Player p)
{
	vector<size_t> to_remove;
	rankHand(p.hand);
	if (p.hand.get_poker() == straight_flush || p.hand.get_poker() == full_house || p.hand.get_poker() == straight || p.hand.get_poker() == flush_rank) {
		to_remove = {};
	}
	else if (p.hand.get_poker() == four_kind || p.hand.get_poker() == two_pair) {
		if (p.hand.get_poker() == four_kind) {
			if (p.hand[0].rank == p.hand[1].rank) {
				to_remove = { 4 };
			}
			else {
				to_remove = { 0 };
			}
		}
		else {
			//Unmatched card on the right
			if (p.hand[0].rank == p.hand[1].rank && p.hand[2].rank == p.hand[3].rank) {
				to_remove = { 4 };
			}
			//Unmatched card on the left
			else if (p.hand[1].rank == p.hand[2].rank && p.hand[3].rank == p.hand[4].rank) {
				to_remove = { 0 };
			}
			//Unmatched card in the center
			else if (p.hand[0].rank == p.hand[1].rank && p.hand[3].rank == p.hand[4].rank) {
				to_remove = { 2 };
			}
		}
	}
	else if (p.hand.get_poker() == three_kind) {
		//Three of a kind on the left
		if (p.hand[0].rank == p.hand[1].rank && p.hand[1].rank == p.hand[2].rank) {
			to_remove = { 3, 4 };
		}
		//Three of a kind in the middle
		else if (p.hand[1].rank == p.hand[2].rank && p.hand[2].rank == p.hand[3].rank) {
			to_remove = { 0, 4 };
		}
		//Three of a kind on the right
		else if (p.hand[2].rank == p.hand[3].rank && p.hand[3].rank == p.hand[4].rank) {
			to_remove = { 0, 1 };
		}
	}
	else if (p.hand.get_poker() == one_pair) {
		if (p.hand[0].rank == p.hand[1].rank) {
			to_remove = { 2, 3, 4 };
		}
		else if (p.hand[1].rank == p.hand[2].rank) {
			to_remove = { 0, 3, 4 };
		}
		else if (p.hand[2].rank == p.hand[3].rank) {
			to_remove = { 0, 1, 4 };
		}
		else if (p.hand[3].rank == p.hand[4].rank) {
			to_remove = { 0, 1, 2 };
		}
	}
	else if (p.hand.get_poker() == high_card) {
		if (p.hand[4].rank == ACE) {
			to_remove = { 0, 1, 2, 3 };
		}
		else {
			to_remove = { 0, 1, 2 };
		}

	}
	return to_remove;
}