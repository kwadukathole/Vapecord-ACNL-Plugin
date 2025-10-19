#include "cheats.hpp"
#include "RegionCodes.hpp"
#include "Helpers/Game.hpp"
#include "Helpers/Player.hpp"
#include "Helpers/Dropper.hpp"
#include "Helpers/PlayerClass.hpp"
#include "Helpers/Inventory.hpp"
#include "Helpers/Wrapper.hpp"
#include "Helpers/IDList.hpp"
#include "Color.h"

namespace CTRPluginFramework {
//Shops Always Open
	void ShopsAlwaysOpen(MenuEntry *entry) {
		static const Address shopretail(0x309348, 0x309430, 0x309344, 0x309344, 0x3093BC, 0x3093BC, 0x30935C, 0x30935C);
		static const Address shopnookling(0x711B14, 0x710FC4, 0x710B1C, 0x710AF4, 0x7102C8, 0x7102A0, 0x70FE70, 0x70FE70);
		static const Address shopgarden(0x711BCC, 0x71107C, 0x710BD4, 0x710BAC, 0x710380, 0x710358, 0x70FF28, 0x70FF28);
		static const Address shopables(0x713EB0, 0x713360, 0x712EB8, 0x712E90, 0x712664, 0x71263C, 0x71220C, 0x71220C);
		static const Address shopshampoodle(0x71D42C, 0x71C774, 0x71C434, 0x71C40C, 0x71BBE0, 0x71BBB8, 0x71B788, 0x71B788);
		static const Address shopkicks(0x71184C, 0x710CFC, 0x710854, 0x71082C, 0x710000, 0x70FFD8, 0x70FBA8, 0x70FBA8);   
		static const Address shopnooks(0x71F654, 0x71E99C, 0x71E65C, 0x71E634, 0x71DE08, 0x71DDE0, 0x71D9B0, 0x71D9B0);
		static const Address shopkatrina(0x718098, 0x717548, 0x7170A0, 0x717078, 0x71684C, 0x716824, 0x7163F4 ,0x7163F4);
		static const Address shopredd(0x718444, 0x7178F4, 0x71744C, 0x717424, 0x716BF8, 0x716BD0, 0x7167A0, 0x7167A0);

		const u32 ShopOpen[9] = { shopretail.addr, shopnookling.addr, shopgarden.addr, shopables.addr, shopshampoodle.addr, shopkicks.addr, shopnooks.addr, shopkatrina.addr, shopredd.addr };

		if(entry->WasJustActivated()) {
			for(int i = 0; i < 9; ++i)
				Process::Patch(ShopOpen[i], 0xE3A00001);
		}
		else if(!entry->IsActivated()) {
			for(int i = 0; i < 9; ++i)
				Process::Patch(ShopOpen[i], 0xE3A00000);
		}
    }

//Disable Save Menus
	void nonesave(MenuEntry *entry) {
		if(entry->WasJustActivated()) {
			Process::Patch(Code::nosave.addr, 0xE1A00000);
			save = true;
		}
		else if(!entry->IsActivated()) {
			Process::Patch(Code::nosave.addr, 0xE8900006);
			save = false;
		}
	}

//Disable Item Locks /*Credits to Nico*/
	void bypass(MenuEntry *entry) {
		if(entry->WasJustActivated()) {
			Dropper::DropItemLock(true);
			bypassing = true;
		}
		else if(!entry->IsActivated()) {
			Dropper::DropItemLock(false);
			bypassing = false;
		}
	}
//Can't Fall In Holes Or Pitfalls /*Credits to Nico*/
	void noTrap(MenuEntry *entry) {
		static const Address notraps1(0x65A668, 0x659B90, 0x6596A0, 0x6596A0, 0x659160, 0x659160, 0x658D08, 0x658D08);
		static const Address notraps2(0x6789E4, 0x677F0C, 0x677A1C, 0x677A1C, 0x6774DC, 0x6774DC, 0x677084, 0x677084);
		
		if(entry->WasJustActivated()) {
			Process::Patch(notraps1.addr, 0xEA000014);
			Process::Patch(notraps2.addr, 0xEA00002D);
		}
		else if(!entry->IsActivated()) {
			Process::Patch(notraps1.addr, 0x1A000014);
			Process::Patch(notraps2.addr, 0x1A00002D);
		}
	}

	void SetSpotState(MenuEntry *entry) {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			MessageBox(Language::getInstance()->get("SAVE_PLAYER_NO")).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> spotVEC = {
			Language::getInstance()->get("VECTOR_QUICK_LOCK_SPOT"), 
			Language::getInstance()->get("VECTOR_QUICK_UNLOCK_SPOT"),
			Language::getInstance()->get("VECTOR_QUICK_LOCK_MAP"),
			Language::getInstance()->get("VECTOR_QUICK_UNLOCK_MAP")
		};

		u32 x = 0, y = 0;
		PlayerClass::GetInstance()->GetWorldCoords(&x, &y);

		if(bypassing) 
			Dropper::DropItemLock(false);

		Keyboard KB(Language::getInstance()->get("KEY_CHOOSE_OPTION"), spotVEC);
		switch(KB.Open()) {
			default: break;
			case 0: {
				if(GameHelper::CreateLockedSpot(0x12, x, y, GameHelper::RoomCheck(), true) == 0xFFFFFFFF) 
					OSD::Notify("Error: Too many locked spots are already existing!");			
				else 
					OSD::Notify("Locked Spot");
			} break;

			case 1: {
				GameHelper::ClearLockedSpot(x, y, GameHelper::RoomCheck(), 4);
				OSD::Notify("Unlocked Spot");
			} break;

			case 2: {
				x = 0, y = 0;
				while(GameHelper::CreateLockedSpot(0x12, 0x10 + x, 0x10 + y, GameHelper::RoomCheck(), true) != 0xFFFFFFFF) {
					x++;
					if(x % 6 == 2) { 
						y++; 
						x = 0; 
					}
					
					Sleep(Milliseconds(40));
				}
				OSD::Notify("Locked Map");
			} break;

			case 3: {
				x = 0x10, y = 0x10;
				bool res = true;

				while(res) {
					while(res) {
						if(GameHelper::GetItemAtWorldCoords(x, y)) {
							if(GameHelper::GetLockedSpotIndex(x, y, GameHelper::RoomCheck()) != 0xFFFFFFFF) {
								GameHelper::ClearLockedSpot(x, y, GameHelper::RoomCheck(), 4);
								Sleep(Milliseconds(40));
							}
						}
						else 
							res = false;

						y++;
					}
					res = true;
					y = 0x10;
					x++;
					if(!GameHelper::GetItemAtWorldCoords(x, y)) 
						res = false;
				}
				OSD::Notify("Unlocked Map");
			} break;
		}

		Sleep(Milliseconds(5));
		if(bypassing) 
			Dropper::DropItemLock(true);
	}

//search and replace
	void SearchReplace(MenuEntry *entry) {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			MessageBox(Language::getInstance()->get("SAVE_PLAYER_NO")).SetClear(ClearScreen::Top)();
			return;
		}

		u32 x = 0, y = 0;
		u32 count = 0;
		Item ItemToSearch = {0x7FFE, 0};
		Item ItemToReplace = {0x7FFE, 0};
		
		if(!Wrap::KB<u32>(Language::getInstance()->get("QUICK_MENU_SEARCH_REPLACE_SEARCH"), true, 8, *(u32 *)&ItemToSearch, 0x7FFE)) 
			return;
		
		if(!Wrap::KB<u32>(Language::getInstance()->get("QUICK_MENU_SEARCH_REPLACE_REPLACE"), true, 8, *(u32 *)&ItemToReplace, *(u32 *)&ItemToReplace)) 
			return;
		
		if(!IDList::ItemValid(ItemToReplace)) {
			OSD::Notify("Item Is Invalid!", Color::Red);
			return;
		}

		int res = Dropper::Search_Replace(300, { ItemToSearch }, ItemToReplace, 0x3D, true, "items replaced!", true);
		if(res == -1) {
			OSD::Notify("Your player needs to be loaded!", Color::Red);
			return;
		}
		else if(res == -2) {
			OSD::Notify("Only works outside!", Color::Red);
			return;
		}
	}

//remove all town items
	void RemoveItemsCheat(MenuEntry *entry) {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			MessageBox(Language::getInstance()->get("SAVE_PLAYER_NO")).SetClear(ClearScreen::Top)();
			return;
		}

		if((MessageBox(Language::getInstance()->get("REMOVE_ITEM_WARNING"), DialogType::DialogYesNo)).SetClear(ClearScreen::Top)()) {
			GameHelper::RemoveItems(true, 0, 0, 0xFF, 0xFF, true, true);
		}
	}

//Water All Flowers	
	void WaterAllFlowers(MenuEntry *entry) {	
		if(entry->Hotkeys[0].IsPressed()) {
			u32 x = 0x10, y = 0x10;
			bool res = true;
			
			while(res) {
				while(res) {
					if(GameHelper::GetItemAtWorldCoords(x, y)) 
						GameHelper::WaterFlower(x, y);
					else 
						res = false;

					y++;
				}
				
				res = true;
				y = 0x10;
				x++;
				if(!GameHelper::GetItemAtWorldCoords(x, y)) 
					res = false;
			}
			OSD::Notify("Success!");
		}
    }
//Weed Remover
	void weedremover(MenuEntry *entry) {	
		const std::vector<std::string> weedopt = {
			Language::getInstance()->get("WEED_REMOVER_OFF"), 
			Language::getInstance()->get("WEED_REMOVER_ON")
		};
		
		static int size = 400;
		if(entry->Hotkeys[0].IsPressed()) {
			Keyboard KB(Language::getInstance()->get("WEED_REMOVER_KEY"), weedopt);

			switch(KB.Open()) {
				case 0: size = 5000; break;
				case 1: 
				default: size = 300; break;
			}
		}
		
		else if(entry->Hotkeys[1].IsPressed()) {
			int res = Dropper::Search_Replace(size, { {0x7C, 0}, {0x7D, 0}, {0x7E, 0}, {0x7F, 0}, {0xCC, 0}, {0xF8, 0} }, {0x7FFE, 0}, 0x3D, false, "Weed Removed!", true);
			if(res == -1)
				OSD::Notify("Your player needs to be loaded!", Color::Red);
			else if(res == -2) 
				OSD::Notify("Only works outdoors!", Color::Red);
		}
	}
//Edit Every Pattern
	void editpattern(MenuEntry *entry) {
		for(int i = 0; i < 10; ++i) 
			Player::StealDesign(i);

		entry->Disable();
	}
	
	static const int _GrassTile[64] = {
		0,	1,	4,	5,	16,	17,	20,	21,
		2,	3,	6,	7,	18,	19,	22,	23,
		8,	9,	12,	13,	24,	25,	28,	29,
		10,	11,	14,	15,	26,	27,	30,	31,
		32,	33,	36,	37,	48,	49,	52,	53,
		34,	35,	38,	39,	50,	51,	54,	55,
		40,	41,	44,	45,	56,	57,	60,	61,
		42,	43,	46,	47,	58,	59,	62,	63
	};
/*Calculations copied from the ACNL Web Save Editor, credits goes to the creator*/
	u32 GetTileOffset(int x, int y) {
		const int Add = 64 * ((y / 8) * 8 * 2 + (x / 8)) + _GrassTile[(y % 8) * 8 + (x % 8)];
		const u32 GrassStart = *(u32 *)(GameHelper::GetCurrentMap() + 0x28);
		return (GrassStart + Add);
	}
	
	void grasscomplete(MenuEntry *entry) {		
		const std::vector<std::string> GrassKB {
			Language::getInstance()->get("GRASS_EDITOR_FILL"),
			Language::getInstance()->get("GRASS_EDITOR_CLEAR")
		};
		
		if(!GameHelper::IsInRoom(0)) {
			MessageBox(Color::Red << Language::getInstance()->get("ONLY_TOWN_ERROR")).SetClear(ClearScreen::Top)();
			return;
		}
		
		const u32 GrassStart = *(u32 *)(GameHelper::GetCurrentMap() + 0x28);
		Keyboard KB(Language::getInstance()->get("GRASS_EDITOR_KB1") << "\n" << Color(0x228B22FF) << 
					Language::getInstance()->get("GRASS_EDITOR_KB2")  << "\n" << Color(0xCD853FFF) << 
					Language::getInstance()->get("GRASS_EDITOR_KB3"), GrassKB);
					
		switch(KB.Open()) {
			case 0:
				std::memset((void *)GrassStart, -1, 0x2800);
				GameHelper::ReloadRoom();
			break;
			case 1:
				std::memset((void *)GrassStart, 0, 0x2800);
				GameHelper::ReloadRoom();
			break;
			default: break;
		}
	}
	
//remove/add grass!!!! :))))))
	void grasseditor(MenuEntry *entry) {
		static bool opt = false;
		static u8 type = 0;

		if(entry->Hotkeys[0].IsPressed()) {
			if(!GameHelper::IsInRoom(0)) {
				OSD::Notify("Error: Only Works In Town!", Color::Red);
				return;
			}
			
			u32 x, y;
			if(PlayerClass::GetInstance()->GetWorldCoords(&x, &y)) {
				Process::Write8(GetTileOffset(x, y), type);
				OSD::Notify(Utils::Format("Changed Grass at: X%02X|Y%02X", (u8)x, (u8)y));
			}
		}
		
		else if(entry->Hotkeys[1].IsPressed()) 
			GameHelper::ReloadRoom();
		
		else if(entry->Hotkeys[2].IsPressed()) {
			switch(opt) {
				case 0:
					type = 0xFF;
					OSD::Notify("Grass: Fill-Mode Active!", Color(0x228B22FF));
					opt = true;
				break;
				case 1:
					type = 0;
					OSD::Notify("Grass: Clear-Mode Active!", Color(0xCD853FFF));
					opt = false;
				break;
			}
		}
	}
	
	const int TimeMax[5] = { 60, 24, 30, 12, 50 };
	static int CurrTime = 0;
	
	bool CheckTimeInput(const void *input, std::string &error) {
		const std::string TimeMode[5] = { 
			Language::getInstance()->get("TIME_MINUTE"),
			Language::getInstance()->get("TIME_HOUR"),
			Language::getInstance()->get("TIME_DAY"),
			Language::getInstance()->get("TIME_MONTH"),
			Language::getInstance()->get("TIME_YEAR")
		};

        u16 in = *static_cast<const u16 *>(input);
        if(in >= TimeMax[CurrTime]) {
			error = Utils::Format(Language::getInstance()->get("TIME_ERROR").c_str(), (TimeMax[CurrTime] - 1), TimeMode[CurrTime].c_str());
            return 0;
        }

        return 1;
    }
	
	void TTKeyboard(MenuEntry *entry) {
		const std::string TimeMode[5] = {
			Language::getInstance()->get("TIME_MINUTE"),
			Language::getInstance()->get("TIME_HOUR"),
			Language::getInstance()->get("TIME_DAY"),
			Language::getInstance()->get("TIME_MONTH"),
			Language::getInstance()->get("TIME_YEAR")
		};

		std::vector<std::string> TTKB {
			Language::getInstance()->get("TIME_BACKWARDS"),
			Language::getInstance()->get("TIME_FORWARD")
		};
		
		u8 timedat[5] = { 0, 0, 0, 0, 0 };
		Keyboard KB("", TTKB);
		int ch = KB.Open();
		if(ch < 0)
			return;
		
		for(int i = 0; i < 5; ++i) {			
			Keyboard KBS(Utils::Format(Language::getInstance()->get("TIME_KB1").c_str(), TimeMode[i].c_str()));
			KBS.IsHexadecimal(false);
			KBS.SetMaxLength(2);
			CurrTime = i;
			KBS.SetCompareCallback(CheckTimeInput);

			int cho = KBS.Open(timedat[i]);
			if(cho < 0)
				return;
		}
		
		GameHelper::SetCurrentTime(ch, timedat[0], timedat[1], timedat[2], timedat[3], timedat[4]);
	}
	
	void TimeTravel(MenuEntry *entry) {
		static u32 PressedTicks = 0;
		int minute = 1;
		
		if(entry->Hotkeys[0].IsDown() || entry->Hotkeys[0].IsPressed()) {
			PressedTicks++;
			if((PressedTicks < 50 ? (PressedTicks % 8) == 1 : (PressedTicks % 3) == 1) || PressedTicks > 100) 
				GameHelper::SetCurrentTime(true, minute, 0, 0, 0, 0);
		}
		
		else if(entry->Hotkeys[1].IsDown() || entry->Hotkeys[1].IsPressed()) {
			PressedTicks++;
			if((PressedTicks < 50 ? (PressedTicks % 8) == 1 : (PressedTicks % 3) == 1) || PressedTicks > 100) 
				GameHelper::SetCurrentTime(false, minute, 0, 0, 0, 0);
		}
	//somehow doesnt work always?
		else if(Controller::IsKeysReleased(entry->Hotkeys[0].GetKeys()) || Controller::IsKeysReleased(entry->Hotkeys[1].GetKeys())) 
			PressedTicks = 0;
	}

	bool ThinkToBuriedItems(Item *item) {
		if(((*item).Flags >> 12) == 8) 
			(*item).Flags &= 0x0FFF;

		const HookContext &curr = HookContext::GetCurrent();
		static Address func(decodeARMBranch(curr.targetAddress, curr.overwrittenInstr));
		return func.Call<bool>(item);
	}

	Item* PickBuriedItems(u32 pInstance, u8 wX, u8 wY) {
		Item* item = GameHelper::GetItemAtWorldCoords(wX, wY);
		if((item->Flags >> 12) == 8)
			item->Flags &= 0x0FFF;

		return item;
	}

	void BuriedInspector(MenuEntry *entry) {
		static Hook BuriedHook, PickBuriedHook;
		static const Address BuriedAddress(0x665534, 0x664A5C, 0x66456C, 0x66456C, 0x66402C, 0x66402C, 0x663BD4, 0x663BD4);
		static const Address PickBuriedAddress(0x59A0BC, 0x5995D4, 0x599104, 0x599104, 0x5989F4, 0x5989F4, 0x5986C8, 0x5986C8);

		if(entry->WasJustActivated()) {
			BuriedHook.Initialize(BuriedAddress.addr, (u32)ThinkToBuriedItems);
			BuriedHook.SetFlags(USE_LR_TO_RETURN);
			BuriedHook.Enable();

			PickBuriedHook.Initialize(PickBuriedAddress.addr, (u32)PickBuriedItems);
			PickBuriedHook.SetFlags(USE_LR_TO_RETURN);
			PickBuriedHook.Enable();
		}
		else if(!entry->IsActivated()) {
			BuriedHook.Disable();
			PickBuriedHook.Disable();
		}
	}
}
