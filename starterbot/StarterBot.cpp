#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include "GameItem.h"

StarterBot::StarterBot()
{
    Item_Array = std::vector<GameItem>();
}

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    //Use tool functions to observe game state and build input array
    Observe_game_state();

    //use decision matrix to weight inputs and build array of tasks to commplete
    Decission_matrix();
    
    //take tasks from decision matrix and complete using tool functions
    Event_Handler();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
 
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}


void StarterBot::Observe_game_state() {

    Tools::get_available_resources(StarterBot::Inputs);
    Tools::get_units(StarterBot::Inputs);
    Tools::get_enemy_units(StarterBot::Inputs);
}

void StarterBot::Decission_matrix() {
    std::vector<BWAPI::Unit> Marines, Firebats, SCV, Barracks, Command, EnemyUnits, EnemyBuildings;
    int MineralCount = 0;
    int GasCount = 0;
    int CommandCenterCount = 0;
    int RefineryCount = 0;
    int DepotCount = 0;
    int BarracksCount = 0;
    int AcadamyCount = 0;
    int SCVCount = 0;
    int MarineCount = 0;
    int FirebatCount = 0;
    int SupplyUsed = 0;
    // pass in inputs
    while (!Inputs[0].empty()) {
        auto& index = Inputs[0].back();
        auto& value = Inputs[1].back();
        auto& unit = Inputs[2].back();
        
            switch (std::get<int>(index)) {
            case 11://Mineral
                MineralCount = std::get<int>(value);
                break;
            case 14://Gas
                GasCount = std::get<int>(value);
                break;
            case 16://command center
                CommandCenterCount = std::get<int>(value);
                //Supply += 10;
                break;
            case 15://Refinery
                RefineryCount = std::get<int>(value);
                break;
            case 13://Depot
                DepotCount = std::get<int>(value);
                //Supply += 8;
                break;
            case 21://Barracks
                BarracksCount = std::get<int>(value);
                break;
            case 22://Acadamy
                AcadamyCount = std::get<int>(value);
                break;
            case 12://SCV count
                SCVCount = std::get<int>(value);
                break;
            case 231://Marine count
                MarineCount = std::get<int>(value);
                break;
            case 232://Firebat count
                FirebatCount = std::get<int>(value);
                break;
            case  410: //scv units
                SCV.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 411://Marine units
                Marines.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 412://Firebat units
                Firebats.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 421://Command center units
                Command.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 422://Barracks units
                Barracks.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 511://enemy units
                EnemyUnits.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 521://enemy buildings
                EnemyBuildings.push_back(std::get<BWAPI::Unit>(unit));
                break;
            }
        Inputs[0].pop_back();
        Inputs[1].pop_back();
        Inputs[2].pop_back();
    }

    //generate outputs
    SupplyUsed =  SCVCount + MarineCount + FirebatCount;
    while (MineralCount > 100) {
        //build supply depot
        if (Tools::GetTotalSupply(true) - 2 <= BWAPI::Broodwar->self()->supplyUsed()) {
            Outputs[0].push_back(2);
            Outputs[1].push_back(13);
            Outputs[2].push_back(NULL);
            MineralCount -= 100;
        }
        else {
            //build barracks
            if (BarracksCount == 0 && MineralCount > 150) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(21);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
            }
            //build acadamy
            if (AcadamyCount == 0 && MineralCount > 150) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(22);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
            }
            //build refinery 
            if (RefineryCount == 0 && MineralCount > 100) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(15);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
            }
            //trail marine 
            if (MarineCount < Tools::GetTotalSupply(true) / 2 && MineralCount > 50 && BarracksCount >= 1) {
                Outputs[0].push_back(1);
                Outputs[1].push_back(231);
                Outputs[2].push_back(Barracks.back());
                MineralCount -= 50;
            }
            //train scv
            if (SCVCount < Tools::GetTotalSupply(true) / 2 && MineralCount > 50 && CommandCenterCount >= 1) {
                Outputs[0].push_back(1);
                Outputs[1].push_back(12);
                Outputs[2].push_back(Command.back());
                MineralCount -= 50;
            }
        }
    }

    for (auto& s : SCV) {
        if (s->isIdle()) {
            Outputs[0].push_back(3);
            Outputs[1].push_back(1);
            Outputs[2].push_back(s);
        }
    }

    for (auto& m : Marines) {
        if (m->isIdle()) {
            Outputs[0].push_back(4);
            Outputs[1].push_back(1000);
            Outputs[2].push_back(m);
        }
    }

    for (auto& e : EnemyUnits) {
        Outputs[0].push_back(5);
        Outputs[1].push_back(0);
        Outputs[2].push_back(e);
    }

    for (auto& B : EnemyBuildings) {
        Outputs[0].push_back(5);
        Outputs[1].push_back(1000);
        Outputs[2].push_back(B);
    }
}

void StarterBot::Event_Handler() {
    
    while (!Outputs[0].empty()) {
        int index = std::get<int>(Outputs[0].back());
        int value = std::get<int>(Outputs[1].back());
        BWAPI::Unit unit;
        if (std::holds_alternative<BWAPI::Unit>(Outputs[2].back())) {
            unit = std::get<BWAPI::Unit>(Outputs[2].back());
        }
        switch (index) {
            case 0: //empty event
                break;
            case 1: // build unit
                Tools::BuildUnit(unit, value);
                break;
            case 2: // build building
                if (value == 13) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Supply_Depot); }
                else if (value == 21) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Barracks); }
                else if (value == 15) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Refinery); }
                else if (value == 22) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Academy); }
                else if (value == 16) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Command_Center); }
                break;
            case 3: // Scb mine
                Tools::sendScbMining(unit, value);
                break;
            case 4: // move unit
                Tools::moveUnit(unit, value);
                break;
            case 5: // attack unit
                Tools::AttackUnit(unit, value);
                break;

        }
        Outputs[0].pop_back();
        Outputs[1].pop_back();
        Outputs[2].pop_back();
    }
}
