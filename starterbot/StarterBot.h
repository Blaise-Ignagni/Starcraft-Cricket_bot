#pragma once

#include "MapTools.h"
#include "GameItem.h"
#include <vector>
#include <BWAPI.h>
#include <variant>

class StarterBot
{
    MapTools m_mapTools;

public:

    StarterBot();

    void drawDebugInformation();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);

	// functions used to interact with game 
	void Observe_game_state();
	void Decission_matrix();
	void Event_Handler();

	std::vector<GameItem> Item_Array;
	std::vector<GameItem> Event_Array;

	std::vector<std::variant<int, BWAPI::Unit>> Inputs[3];
	std::vector<std::variant<int, BWAPI::Unit>> Outputs[3];
};