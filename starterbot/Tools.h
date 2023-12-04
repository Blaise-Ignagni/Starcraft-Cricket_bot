#pragma once

#include <BWAPI.h>
#include "GameItem.h"
#include <variant>
#include <random>
#include <algorithm>

namespace Tools
{
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unit GetDepot();

    //drawing tools
    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();
    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    // Observation functions // 
    int GetTotalSupply(bool inProgress = false);
    void get_available_resources(std::vector<std::variant<int, BWAPI::Unit>> (&Items)[3]);
    void get_units(std::vector<std::variant<int, BWAPI::Unit>> (&Items)[3]);
    //implement to call for multiple enimies if playing more than 1 player
    void get_enemy_units(std::vector<std::variant<int, BWAPI::Unit>> (&Items)[3]);

        // building 
    void buildBarraks();
    void buildAdditionalSupply();
        // training 
    void trainAdditionalUnits();
    void trainAdditionalMarines();
    void trainAdditionalWorkers();
        // unit function 
    void sendScoutingMarines();
    void sendIdleWorkersToMinerals();
    //void patrolMarine();
    void engageCombat();

    // Lower level functions
    BWAPI::TilePosition findUnexploredSector();
    void BuildUnit(BWAPI::Unit unit, int value);
    bool BuildBuilding(BWAPI::UnitType type);
    void sendScbMining(BWAPI::Unit unit, int value);
    void moveUnit(BWAPI::Unit unit, int value);
    void AttackUnit(BWAPI::Unit unit, int value);

}