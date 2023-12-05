#include "Tools.h"
#include "StarterBot.h"
#include <cstdlib>


BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto& u : units)
    {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }

    return closestUnit;
}

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units)
{
    if (!unit) { return nullptr; }
    return GetClosestUnitTo(unit->getPosition(), units);
}

int Tools::CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units)
{
    int sum = 0;
    for (auto& unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

BWAPI::Unit Tools::GetUnitOfType(BWAPI::UnitType type)
{
    std::vector<BWAPI::Unit> workers;
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted())
        {
            workers.push_back(unit);
        }
    }
    if (!workers.empty()) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(workers.begin(), workers.end(), g);
        return workers.back();
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

BWAPI::Unit Tools::GetDepot()
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();
    return GetUnitOfType(depot);
}

// Attempt tp construct a building of a given type 
bool Tools::BuildBuilding(BWAPI::UnitType type)
{
    // Get the type of unit that is required to build the desired building
    BWAPI::UnitType builderType = type.whatBuilds().first;

    // Get a unit that we own that is of the given type so it can build
    // If we can't find a valid builder unit, then we have to cancel the building
    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);
    if (!builder) { return false; }

    // Get a location that we want to build the building next to
    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;
    bool buildingOnCreep = type.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
    return builder->build(type, buildPos);
}

void Tools::DrawUnitCommands()
{
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        const BWAPI::UnitCommand & command = unit->getLastCommand();

        // If the previous command had a ground position target, draw it in red
        // Example: move to location on the map
        if (command.getTargetPosition() != BWAPI::Positions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        // If the previous command had a tile position target, draw it in red
        // Example: build at given tile position location
        if (command.getTargetTilePosition() != BWAPI::TilePositions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        // If the previous command had a unit target, draw it in red
        // Example: attack unit, mine mineral, etc
        if (command.getTarget() != nullptr)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }
    }
}

void Tools::DrawUnitBoundingBoxes()
{
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

void Tools::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target)
{
    // if there's no valid unit, ignore the command
    if (!unit || !target) { return; }

    // Don't issue a 2nd command to the unit on the same frame
    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount()) { return; }

    // If we are issuing the same type of command with the same arguments, we can ignore it
    // Issuing multiple identical commands on successive frames can lead to bugs
    if (unit->getLastCommand().getTarget() == target) { return; }
    
    // If there's nothing left to stop us, right click!
    unit->rightClick(target);
}

int Tools::GetTotalSupply(bool inProgress)
{
    // start the calculation by looking at our current completed supplyt
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    // if we don't want to calculate the supply in progress, just return that value
    if (!inProgress) { return totalSupply; }

    // if we do care about supply in progress, check all the currently constructing units if they will add supply
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // ignore units that are fully completed
        if (unit->isCompleted()) { continue; }

        // if they are not completed, then add their supply provided to the total supply
        totalSupply += unit->getType().supplyProvided();
    }

    // one last tricky case: if a unit is currently on its way to build a supply provider, add it
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build) { continue; }

        // add the supply amount of the unit that it's trying to build
        totalSupply += command.getUnitType().supplyProvided();
    }

    return totalSupply;
}

void Tools::DrawUnitHealthBars()
{
    // how far up from the unit to draw the health bar
    int verticalOffset = -10;

    // draw a health bar for each unit on the map
    for (auto& unit : BWAPI::Broodwar->getAllUnits())
    {
        // determine the position and dimensions of the unit
        const BWAPI::Position& pos = unit->getPosition();
        int left = pos.x - unit->getType().dimensionLeft();
        int right = pos.x + unit->getType().dimensionRight();
        int top = pos.y - unit->getType().dimensionUp();
        int bottom = pos.y + unit->getType().dimensionDown();

        // if it's a resource, draw the resources remaining
        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            DrawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        }
        // otherwise if it's a unit, draw the hp 
        else if (unit->getType().maxHitPoints() > 0)
        {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;
            DrawHealthBar(unit, hpRatio, hpColor, 0);
            
            // if it has shields, draw those too
            if (unit->getType().maxShields() > 0)
            {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                DrawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void Tools::DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset)
{
    int verticalOffset = -10;
    const BWAPI::Position& pos = unit->getPosition();

    int left = pos.x - unit->getType().dimensionLeft();
    int right = pos.x + unit->getType().dimensionRight();
    int top = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop = top + yOffset + verticalOffset;
    int hpBottom = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i(left); i < right - 1; i += ticWidth)
    {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}

//build a barracks 
void Tools::buildBarraks()
{
    const bool startedBuilding = BuildBuilding(BWAPI::UnitTypes::Terran_Barracks);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", BWAPI::UnitTypes::Terran_Barracks.getName().c_str());
    }
}

//trains Marines 
void Tools::trainAdditionalMarines()
{

    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        //find barracks
        //should implemnet to que in most empty barrack
        if (unit->getType() == BWAPI::UnitTypes::Terran_Barracks) {
            unit->train(BWAPI::UnitTypes::Terran_Marine);
        }
    }
}

//returns an unexplored map tile
BWAPI::TilePosition Tools::findUnexploredSector()
{
    int mapWidth = BWAPI::Broodwar->mapWidth();
    int mapHeight = BWAPI::Broodwar->mapHeight();
    int sectorSize = 100;  // Adjust the sector size as needed

    for (int x = 0; x < mapWidth; x += sectorSize)
    {
        for (int y = 0; y < mapHeight; y += sectorSize)
        {
            BWAPI::TilePosition sectorCenter(x + sectorSize / 2, y + sectorSize / 2);

            // Check if the central tile of the sector is visible (explored)
            if (!BWAPI::Broodwar->isExplored(sectorCenter))
            {
                return sectorCenter;
            }
        }
    }

    // If all sectors have been explored, return an invalid tile
    return BWAPI::TilePositions::Invalid;
}

//send marines scouting 
void Tools::sendScoutingMarines()
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    int marineScouts = 0;

    for (auto& unit : myUnits)
    {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Marine && (unit->isIdle()))
        {
            // Find an unexplored location for scouting
            BWAPI::TilePosition scoutLocation = findUnexploredSector();

            if (scoutLocation.isValid())
            {
                unit->attack(BWAPI::Position(scoutLocation));
                marineScouts++;
            }
        }
    }
}

// engage idle marines in combat
void Tools::engageCombat() {
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& marine : myUnits) {
        if (marine->getType() == BWAPI::UnitTypes::Terran_Marine && marine->canAttack()) {
            BWAPI::Unit enemy = NULL;
            enemy = marine->getClosestUnit(BWAPI::Filter::IsEnemy);
            if (enemy != NULL) {
                if (!marine->attack(enemy)) { std::cout << "\n\tAttack fail\n"; }
            }
        }
    }
}

//Allocates when to send resources to training units
void Tools::trainAdditionalUnits()
{
    //get numbers of different unit types
    int worker_count = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_SCV);
    int marine_count = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Marine);
    int Barraks_count = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_Barracks);
    //get current amount of minerals
    int Mineral_count = BWAPI::Broodwar->self()->minerals();

    //only create units if plentiful minerals 
    if (Mineral_count < 100) { return; }

    //check if a barraks has been built so that marines can be trained 
    if ((Barraks_count == 0) || (marine_count > 50 * Barraks_count)) { buildBarraks(); }

    if (Mineral_count < 100) { return; }
    //Determin what units to produce
    if (marine_count > 10 * worker_count) {
        trainAdditionalWorkers();
        Mineral_count -= 50;
    }
    while (Mineral_count >= 50) {
        trainAdditionalMarines();
        Mineral_count -= 50;
    }

}

// Send our idle workers to mine minerals so they don't just stand there
void Tools::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
void Tools::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void Tools::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

//helper function 
void vector_fill(std::vector<std::variant<int, BWAPI::Unit>>(&Items)[3], int indx, int value, BWAPI::Unit unit) {
    Items[0].push_back(indx);
    Items[1].push_back(value);
    Items[2].push_back(unit);
}

void Tools::get_available_resources(std::vector<std::variant<int, BWAPI::Unit>>(&Items)[3]) {
    //return the number of minerals [11]
    int k = BWAPI::Broodwar->self()->minerals();
    vector_fill(Items, 11, k, NULL);
    //return gas count [14]
    k = BWAPI::Broodwar->self()->gas();
    vector_fill(Items, 14, k, NULL);

    int d = 0, r = 0, c = 0, b = 0, a = 0, s = 0, f = 0, sf = 0, ct = 0, ms = 0, pl = 0;
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits) {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Supply_Depot) { d++; }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Refinery) { r++; }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center) { 
            c++; 
            vector_fill(Items, 421, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Barracks) { 
            b++;
            vector_fill(Items, 422, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Academy) { 
            a++;
            vector_fill(Items, 423, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Factory) {
            f++;
            vector_fill(Items, 424, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Starport) {
            s++;
            vector_fill(Items, 425, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Science_Facility) {sf++;}
        if (unit->getType() == BWAPI::UnitTypes::Terran_Control_Tower) { ct++; }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Machine_Shop) { ms++; }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Physics_Lab) { pl++; }
    }

    //return depot count [13]
    vector_fill(Items, 13, d, NULL);
    //return refinery count [15]
    vector_fill(Items, 15, r, NULL);
    //return command center count [16]
    vector_fill(Items, 16, c, NULL);
    //return barraks count [21]
    vector_fill(Items, 21, b, NULL);
    //return accadamy count [22]
    vector_fill(Items, 22, a, NULL);
    //return factory count 
    vector_fill(Items, 27, f, NULL);
    //return starport count 
    vector_fill(Items, 29, s, NULL);
    //return sci facility count 
    vector_fill(Items, 210, sf, NULL);
    //returnn control tower count 
    vector_fill(Items, 211, ct, NULL);
    //return machine shop count 
    vector_fill(Items, 212, ms, NULL);
    //return Physics lab count 
    vector_fill(Items, 213, pl, NULL);
}

void Tools::get_units(std::vector<std::variant<int, BWAPI::Unit>> (&Items)[3]) {
    //return total number of units [230]

    int m = 0, f = 0, s = 0, v =0, st = 0, w = 0, bc = 0;
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits) {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Marine) {
            m++;
            vector_fill(Items, 411, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Firebat) { 
            f++;
            vector_fill(Items, 412, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_SCV) { 
            s++;
            vector_fill(Items, 410, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Vulture) {
            v++;
            vector_fill(Items, 414, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) {
            st++;
            vector_fill(Items, 415, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
            st++;
            vector_fill(Items, 415, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Wraith) {
            w++;
            vector_fill(Items, 416, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Terran_Battlecruiser) {
            bc++;
            vector_fill(Items, 418, unit->getHitPoints(), unit);
        }
    }

    //return marine count [231]
    vector_fill(Items, 231, m, NULL);
    //return firebat count [232]
    vector_fill(Items, 232, f, NULL);
    //return SCB count [12]
    vector_fill(Items, 12, s, NULL);
    //return vulture count 
    vector_fill(Items, 234, v, NULL);
    //return seige tank count 
    vector_fill(Items, 235, st, NULL);
    //return wraith count 
    vector_fill(Items, 236, w, NULL);
    //return battlecruiser count 
    vector_fill(Items, 238, bc, NULL);
}

void Tools::get_enemy_units(std::vector<std::variant<int, BWAPI::Unit>> (&Items)[3]) {

    int g = 0, a = 0, b = 0;

    const BWAPI::Unitset& enemyUnits = BWAPI::Broodwar->enemy()->getUnits();
    for (auto& unit : enemyUnits) {
        if (unit->getType() == BWAPI::UnitTypes::Men && unit->getType().isFlyer()) {
            g++;
            vector_fill(Items, 511, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Men && !unit->getType().isFlyer()) {
            a++;
            vector_fill(Items, 512, unit->getHitPoints(), unit);
        }
        if (unit->getType() == BWAPI::UnitTypes::Buildings) {
            b++;
            vector_fill(Items, 513, unit->getHitPoints(), unit);
        }
    }

    //get visible enemy units [31]
    vector_fill(Items, 31, g+a, NULL);
    //get visible enemy buildings [32]
    vector_fill(Items, 32, b, NULL);
}

void Tools::BuildUnit(BWAPI::Unit unit, int value) {
    //unit Building 
    //value encoding for what unit to build
    //SCV [12]   Marine [231]  Firebat [232]
    //Vulture [234] Seige tank [235] 
    //wraith [236] Battle Cruiser [238]
    switch (value) {
        case 12:
            unit->train(BWAPI::UnitTypes::Terran_SCV);
            break;
        case 231:
            unit->train(BWAPI::UnitTypes::Terran_Marine);
            break;
        case 232:
            unit->train(BWAPI::UnitTypes::Terran_Firebat);
            break;
        case 234:
            unit->train(BWAPI::UnitTypes::Terran_Vulture);
            break;
        case 235:
            unit->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
            break;
        case 236:
            unit->train(BWAPI::UnitTypes::Terran_Wraith);
            break;
        case 238:
            unit->train(BWAPI::UnitTypes::Terran_Battlecruiser);
            break;
    }
}

void Tools::sendScbMining(BWAPI::Unit unit, int value) {
    if (value == 1) {
        // Get the closest mineral to this worker unit
        BWAPI::Unit closestMineral = GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral) { unit->rightClick(closestMineral); }
    }
    else if (value == 2) {
        //get gas 
        BWAPI::Unit refinery = NULL;
        for (auto& u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Terran_Refinery) { 
                refinery = u;
                break;
            }
        }
        unit->rightClick(refinery);
    }
}

void Tools::moveUnit(BWAPI::Unit unit, int value) {
    BWAPI::Position unitPosition = unit->getPosition();

    int newx = unitPosition.x + (std::rand() % (2 * value)) - value;
    int newy = unitPosition.y + (std::rand() % (2 * value)) - value;

    BWAPI::Position random(newx, newy);
    random.makeValid();

    unit->move(random);
}

void Tools::AttackUnit(BWAPI::Unit unit, int value) {
    
    for (auto& u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->isIdle() && (u->getType() == BWAPI::UnitTypes::Terran_Marine || u->getType() == BWAPI::UnitTypes::Terran_Firebat)) {
            if (u->getDistance(unit) < value) {u->rightClick(unit); } 
        }
    }
}
