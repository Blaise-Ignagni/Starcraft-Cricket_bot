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
    std::vector<BWAPI::Unit> Marines, Firebats, SCV, Barracks, Command, Factories, Starports, EnemyUnits, EnemyBuildings;
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
    int VultureCount = 0;
    int SeigeTankCount = 0;
    int WraithCount = 0;
    int BattleCruiserCount = 0;
    int FactoryCount = 0;
    int StarportCount = 0;
    int MachineShopCount = 0;
    int ControlTowerCount = 0;
    int ArmoryCount = 0;
    int PhysicsCount = 0;
    int ScienceCount = 0;
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
            case 27://Factory
                FactoryCount = std::get<int>(value);
                break;
            case 28://Armory
                ArmoryCount = std::get<int>(value);
                break;
            case 29://Starport
                StarportCount = std::get<int>(value);
                break;
            case 210://Science Facility
                ScienceCount = std::get<int>(value);
                break;
            case 211://Control tower
                ControlTowerCount = std::get<int>(value);
                break;
            case 212://Machine Shop
                MachineShopCount = std::get<int>(value);
                break;
            case 213://Physics lab
                PhysicsCount = std::get<int>(value);
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
            case 234://vulture count
                VultureCount = std::get<int>(value);
                break;
            case 235://seige tank count
                SeigeTankCount = std::get<int>(value);
                break;
            case 236://wraith count
                WraithCount = std::get<int>(value);
                break;
            case 238://battle cruiser count
                BattleCruiserCount = std::get<int>(value);
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
            case 424://factory
                Factories.push_back(std::get<BWAPI::Unit>(unit));
                break;
            case 425://starport
                Starports.push_back(std::get<BWAPI::Unit>(unit));
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
    int ArmySize = MarineCount + FirebatCount + VultureCount + SeigeTankCount + WraithCount + BattleCruiserCount;
    while (MineralCount > 100) {
        //check what buildings need to be built
        //check supply
        if (Tools::GetTotalSupply() - 6 <= BWAPI::Broodwar->self()->supplyUsed()) {
            //add instruction to build supply depot 
            Outputs[0].push_back(2);
            Outputs[1].push_back(13);
            Outputs[2].push_back(NULL);
            break;
        }
        //check command center
        if ((ArmySize >= 100 && CommandCenterCount < 2) || (ArmySize >= 250 && CommandCenterCount < 3)) {
            //develop command center at new node
            if (MineralCount > 400){/*build command center*/ 
                Outputs[0].push_back(2);
                Outputs[1].push_back(16);
                Outputs[2].push_back(NULL);
                MineralCount -= 400;
            }
        }

        //check refinery
        if (RefineryCount < CommandCenterCount && MineralCount > 100) {
            Outputs[0].push_back(2);
            Outputs[1].push_back(15);
            Outputs[2].push_back(NULL);
            MineralCount -= 100;
        }

        //check barracks
        if ((BarracksCount < 5) || (ArmySize > 100 && BarracksCount < 10) || (ArmySize > 250 && BarracksCount < 15)) {
            if (MineralCount > 150) {/*build barracks*/ 
                Outputs[0].push_back(2);
                Outputs[1].push_back(16);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
            }
        }

        //check accadamy
        if (AcadamyCount < 1) {
            if (MineralCount > 150) {/*build Acadamy*/ 
                Outputs[0].push_back(2);
                Outputs[1].push_back(22);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
            }
        }

        //check factory
        if ((ArmySize > 100 && FactoryCount < 5) || (ArmySize > 250 && FactoryCount < 10)) {
            if (MineralCount > 200 && GasCount > 100) {/*build factory*/
                Outputs[0].push_back(2);
                Outputs[1].push_back(27);
                Outputs[2].push_back(NULL);
                MineralCount -= 200;
                GasCount -= 100;
            }
        }
        //check armory
        if (ArmySize > 250 && ArmoryCount < 1) {
            if (MineralCount > 100 && GasCount > 50) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(28);
                Outputs[2].push_back(NULL);
                MineralCount -= 100;
                GasCount -= 50;
            }
        }
        //check starport
        if (ArmySize > 250 && StarportCount < 10) {
            if (MineralCount > 150 && GasCount > 100) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(29);
                Outputs[2].push_back(NULL);
                MineralCount -= 150;
                GasCount -= 100;
            }
        }
        //check science facility
        if (ArmySize > 250 && ScienceCount < 1) {
            if (MineralCount > 100 && GasCount > 150) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(210);
                Outputs[2].push_back(NULL);
                MineralCount -= 100;
                GasCount -= 150;
            }
        }
        //check research
          //check machine shop
        if (FactoryCount > 0 && MachineShopCount < FactoryCount) {
            if (MineralCount > 50 && GasCount > 50) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(212);
                Outputs[2].push_back(NULL);
                MineralCount -= 50;
                GasCount -= 50;
            }
        }
          //check control tower
        if (StarportCount > 0 && ControlTowerCount < StarportCount) {
            if (MineralCount > 50 && GasCount > 50) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(211);
                Outputs[2].push_back(NULL);
                MineralCount -= 50;
                GasCount -= 50;
            }
        }
          //check physics lab
        if (ScienceCount == 1 && PhysicsCount < ScienceCount) {
            if (MineralCount > 50 && GasCount > 50) {
                Outputs[0].push_back(2);
                Outputs[1].push_back(29);
                Outputs[2].push_back(NULL);
                MineralCount -= 50;
                GasCount -= 50;
            }
        }

        //Train needed units
        //train SCV
        if ((CommandCenterCount == 1 && SCVCount < 20) || (CommandCenterCount == 2 && SCVCount < 50) || (CommandCenterCount >= 3 && SCVCount < 100)) {
            if(MineralCount > 50){/*train SCV*/
                Outputs[0].push_back(1);
                Outputs[1].push_back(12);
                Outputs[2].push_back(Command.back());
                MineralCount -= 50;
            }
        }
        
        //train marine
        if ((CommandCenterCount == 1 && MarineCount < 30) || (CommandCenterCount == 2 && MarineCount < 30) || (CommandCenterCount >= 3 && MarineCount < 50)) {
            if(BarracksCount > 0 && MineralCount > 50){/*train marine*/
                Outputs[0].push_back(1);
                Outputs[1].push_back(231);
                Outputs[2].push_back(Barracks.back());
                MineralCount -= 50;
            }
        }

        //train FireBat 
        if ((CommandCenterCount == 1 && FirebatCount < 70) || (CommandCenterCount == 2 && FirebatCount < 150) || (CommandCenterCount >= 3 && FirebatCount < 250)) {
            if (BarracksCount > 0 && AcadamyCount > 0 && MineralCount > 50 && GasCount > 25) {/*train Firebat*/ 
                Outputs[0].push_back(1);
                Outputs[1].push_back(232);
                Outputs[2].push_back(Barracks.back());
                MineralCount -= 50;
                GasCount -= 25;
            }
        }

        //train vulture 
        if ((CommandCenterCount == 2 && VultureCount < 50) || (CommandCenterCount >= 3 && VultureCount < 100)) {
            if (FactoryCount > 0 && MineralCount > 75) {/*train Vulture*/ 
                Outputs[0].push_back(1);
                Outputs[1].push_back(234);
                Outputs[2].push_back(Factories.back());
                MineralCount -= 75;
            }
        }

        //train Seige tank
        if ((CommandCenterCount == 2 && SeigeTankCount < 20) || (CommandCenterCount >= 3 && SeigeTankCount < 50)) {
            if (MachineShopCount > 0 && MineralCount > 150 && GasCount > 100) {/*train Siege tank*/ 
                Outputs[0].push_back(1);
                Outputs[1].push_back(235);
                Outputs[2].push_back(Factories.back());
                MineralCount -= 150;
                GasCount -= 100;
            }
        }

        //train Wraith
        if (CommandCenterCount >= 3 && WraithCount < 40) {
            if (StarportCount > 0 && MineralCount > 150 && GasCount > 100) {/*train wraith*/ 
                Outputs[0].push_back(1);
                Outputs[1].push_back(236);
                Outputs[2].push_back(Starports.back());
                MineralCount -= 150;
                GasCount -= 100;
            }
        }

        //train Battle Cruiser
        if (CommandCenterCount >= 3 && BattleCruiserCount < 10) {
            if (StarportCount > 0 && ControlTowerCount > 0 && ArmoryCount > 0 && MineralCount > 400 && GasCount > 300) {/*train Battle cruier*/
                Outputs[0].push_back(1);
                Outputs[1].push_back(238);
                Outputs[2].push_back(Starports.back());
                MineralCount -= 400;
                GasCount -= 300;
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
                else if (value == 27) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Factory); }
                else if (value == 28) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Armory); }
                else if (value == 29) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Starport); }
                else if (value == 210) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Science_Facility); }
                else if (value == 211) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Control_Tower); }
                else if (value == 212) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Machine_Shop); }
                else if (value == 213) { Tools::BuildBuilding(BWAPI::UnitTypes::Terran_Physics_Lab); }
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
