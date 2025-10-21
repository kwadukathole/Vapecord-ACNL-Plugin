#include <cmath>
#include <algorithm>
#include "cheats.hpp"
#include "Helpers/ItemSequence.hpp"

#include "Helpers/Dropper.hpp"
#include "Helpers/Wrapper.hpp"
#include "Helpers/Game.hpp"
#include "Helpers/Inventory.hpp"
#include "Helpers/IDList.hpp"
#include "Helpers/Animation.hpp"
#include "Helpers/Player.hpp"
#include "Color.h"

namespace CTRPluginFramework {
//Item Sequencer
	void Entry_itemsequence(MenuEntry *entry) {	
		std::vector<std::string> cmnOpt = { "" };

		cmnOpt[0] = (ItemSequence::Enabled() ? Color(pGreen) << Language::getInstance()->get("VECTOR_ENABLED") : Color(pRed) << Language::getInstance()->get("VECTOR_DISABLED"));

		Keyboard optKb(Language::getInstance()->get("KEY_CHOOSE_OPTION"), cmnOpt);

		int op = optKb.Open();
		if(op < 0)
			return;
		
		ItemSequence::Switch(ItemSequence::Enabled() ? false : true);
		Entry_itemsequence(entry);
	}
//Drop Modifier /*Radius calculations done by Nico*/
	void dropMod(MenuEntry *entry) {		
		const std::vector<std::string> g_dropmod = {
			Language::getInstance()->get("VECTOR_DROP_PICK"), 
			Language::getInstance()->get("VECTOR_DROP_PULL"),
			Language::getInstance()->get("VECTOR_DROP_DROP"),
			Language::getInstance()->get("VECTOR_DROP_BURY"),
			Language::getInstance()->get("VECTOR_DROP_PLANT"),
			Language::getInstance()->get("VECTOR_DROP_PATTERN"),
			Language::getInstance()->get("VECTOR_DROP_ROCK"),
			Language::getInstance()->get("VECTOR_DROP_DIG"),
		};
		
		const std::vector<std::string> shapeOpt = {
			Language::getInstance()->get("VECTOR_SHAPE_FULL_SQUARE"), //0x15 max
			Language::getInstance()->get("VECTOR_SHAPE_CIRCLE"), //9 max
			Language::getInstance()->get("VECTOR_SHAPE_HORI_LINE"), //0x10 max
			Language::getInstance()->get("VECTOR_SHAPE_VERT_LINE"), //0x10 max
			Language::getInstance()->get("VECTOR_SHAPE_SQUAREE"), //0x10 max
			Language::getInstance()->get("VECTOR_SHAPE_NE_TO_SW"), //0x10 max
			Language::getInstance()->get("VECTOR_SHAPE_NW_TO_SE"), //0x10 max
			Language::getInstance()->get("VECTOR_SHAPE_RESET"),
		};

		if(entry->WasJustActivated()) {
			Process::Patch(Address("DROPM3").addr, 0xE1A00000);
			Process::Write8(Address("DROPM4").addr, 6); //set anim after bury
		}
		
	//Modify Drop Type	
		if(entry->Hotkeys[0].IsPressed()) { 
			Keyboard optKb(Language::getInstance()->get("KEY_CHOOSE_OPTION"), g_dropmod);
			
			int res = optKb.Open();
			if(res < 0)
				return;

			waitAnim = DropTypes[res].dropAnim;
			DropType = DropTypes[res].dropID;
			Process::Write8(Address("DROPM5").addr, DropType);
			return;
		}

	//Modify Drop Radius
		if(entry->Hotkeys[1].IsPressed()) {
			if(IsIndoorsBool) {
				OSD::Notify("Drop Radius Modifier is not usable indoors!", Color::Red);
				return;
			}

			u8 range, count, val;

			Keyboard optKb(Language::getInstance()->get("KEY_CHOOSE_OPTION"), shapeOpt);
			
			int index = optKb.Open();
			switch(index) {
				default: return;
			//Full Square
				case 0: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x15", true, 1, val, 0)) {
						if(val > 0x15)
							val = 0x15;

						val = val & 0xF;
						range = val * 2 + 1;
						count = range * range;
						Process::Write8(Address("DROPM6").addr, count);
						Process::Write8(Address("DROPM7").addr, count);
						for(s8 i = 0 - (s8)val; i <= (s8)val; ++i) {
							for(u8 j = 0; j < range; ++j) {
								Process::Write8(Address("DROPPATTERN").addr + j + ((i + val) * range), (0 - (s8)val) + j);
								Process::Write8(Address("DROPPATTERN").addr + count + j + ((i + val) * range), i);
							}
						}	
					} 
					DropPatternON = true;
				} break;
			//circle	
				case 1: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 9", true, 1, val, 0)) {
						if(val > 9)
							val = 9;

						val = val & 0xFF;
						count = 2 * val * val - 6 * val + 16;
						Process::Write8(Address("DROPM6").addr, count);
						Process::Write8(Address("DROPM7").addr, count);
						for(u8 i = 0; i < count; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + i, (s8)nearbyint((float)(val * cos((6.28318530 / count) * i))));
							Process::Write8(Address("DROPPATTERN").addr + count + i, (s8)nearbyint((float)(val * sin((6.28318530 / count) * i))));
						}
					}
					DropPatternON = true;
				} break;
			//horizontal line	
				case 2: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x10", true, 2, val, 0)) {
						if(val > 0x10)
							val = 0x10;

						range = val * 2 + 1;
						Process::Write8(Address("DROPM6").addr, range);
						Process::Write8(Address("DROPM7").addr, range);
						for(s8 i = 0 - (s8)val; i <= (s8)val; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + (i + val), i);
							Process::Write8(Address("DROPPATTERN").addr + range + (i + val), 0);
						}

					}
					DropPatternON = true;
				} break;
			//Vertical line	
				case 3: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x10", true, 2, val, 0)) {
						if(val > 0x10)
							val = 0x10;

						range = val * 2 + 1;
						Process::Write8(Address("DROPM6").addr, range);
						Process::Write8(Address("DROPM7").addr, range);
						for(s8 i = 0 - (s8)val; i <= (s8)val; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + (i + val), 0);
							Process::Write8(Address("DROPPATTERN").addr + range + (i + val), i);
						}

					}
					DropPatternON = true;
				} break;	
			//square	
				case 4: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x10", true, 2, val, 0)) {
						if(val > 0x10)
							val = 0x10;

						range = val * 2 + 1;
						count = range * 4;
						Process::Write8(Address("DROPM6").addr, count);
						Process::Write8(Address("DROPM7").addr, count);
						for(u8 i = 0; i < range; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + i * 4, i - val);
							Process::Write8(Address("DROPPATTERN").addr + count + i * 4, 0 - val);
							Process::Write8(Address("DROPPATTERN").addr + 1 + i * 4, 0 - val);
							Process::Write8(Address("DROPPATTERN").addr + 1 + count + i * 4, i - val);
							Process::Write8(Address("DROPPATTERN").addr + 2 + i * 4, i - val);
							Process::Write8(Address("DROPPATTERN").addr + 2 + count + i * 4, val);
							Process::Write8(Address("DROPPATTERN").addr + 3 + i * 4, val);
							Process::Write8(Address("DROPPATTERN").addr + 3 + count + i * 4, i - val);
						}
					}
					DropPatternON = true;
				} break;
			//NE to SW line	
				case 5: {
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x10", true, 2, val, 0)) {
						if(val > 0x10)
							val = 0x10;

						range = val * 2 + 1;
						Process::Write8(Address("DROPM6").addr, range);
						Process::Write8(Address("DROPM7").addr, range);
						for(s8 i = 0 - (s8)val; i <= (s8)val; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + (i + val), -i);
							Process::Write8(Address("DROPPATTERN").addr + range + (i + val), i);
						}
					}
					DropPatternON = true;
				} break;
			//NW to SE line	
				case 6:	{
					if(Wrap::KB<u8>(Language::getInstance()->get("DROP_MODS_RADIUS") << "\nMax Value: 0x10", true, 2, val, 0)) {
						if(val > 0x10)
							val = 0x10;

						range = val * 2 + 1;
						Process::Write8(Address("DROPM6").addr, range);
						Process::Write8(Address("DROPM7").addr, range);
						for(s8 i = 0 - (s8)val; i <= (s8)val; ++i) {
							Process::Write8(Address("DROPPATTERN").addr + (i + val), i);
							Process::Write8(Address("DROPPATTERN").addr + range + (i + val), i);
						}
					}
					DropPatternON = true;
				} break;
			//reset drop pattern	
				case 7: {
					Process::Write8(Address("DROPM6").addr, 9);
					Process::Write8(Address("DROPM7").addr, 9);
					Process::Patch(Address("DROPM8").addr, 0x0A000006);
					Process::Patch(Address("DROPM9").addr, 0xE7971100);
					
					Dropper::RestorePattern();
					DropPatternON = false;
				} return;
			}
			
			Process::Patch(Address("DROPM8").addr, 0xEA000006);
			Process::Patch(Address("DROPM9").addr, 0xE1A01000);
			return;
		}

	//set replace item	
		if(entry->Hotkeys[2].IsPressed()) {
			if(Wrap::KB<u32>(Language::getInstance()->get("DROP_MODS_ENTER_ID"), true, 8, *(u32 *)&ItemIDToReplace, 0x7FFE))
				OSD::Notify("Now replacing: " << (ItemIDToReplace == ReplaceEverything ? "everything" : Utils::Format("%08X", ItemIDToReplace)));
		}

	//set Item sequence	
		if(entry->Hotkeys[3].IsPressed()) 
			ItemSequence::OpenIS();

		if(GameHelper::GetActualPlayerIndex() == 0) 
			Process::Patch(Address("DROPM2").addr, 0x13A00001);
		else 
			Process::Patch(Address("DROPM2").addr, 0x13A00000);
		
		if(!entry->IsActivated()) {
			Process::Patch(Address("DROPM2").addr, 0x13A00001);
			Process::Patch(Address("DROPM3").addr, 0x0A000003);
			Process::Patch(Address("DROPM4").addr, 0xE3A01016);
			Process::Patch(Address("DROPM5").addr, 0xE3A0000C);
			Process::Patch(Address("DROPM6").addr, 0xE3540009);
			Process::Patch(Address("DROPM7").addr, 0xE28AB009);
			Process::Patch(Address("DROPM8").addr, 0x0A000006);
			Process::Patch(Address("DROPM9").addr, 0xE7971100);

			Dropper::RestorePattern();
		}
	}
//Drop Items	
	void instantDrop(MenuEntry *entry) {
		if(entry->Hotkeys[0].IsPressed()) 
			Wrap::KB<u32>(Language::getInstance()->get("ENTER_ID"), true, 8, *(u32 *)&dropitem, *(u32 *)&dropitem, ItemChange);
		
		if(turbo ? entry->Hotkeys[1].IsDown() : entry->Hotkeys[1].IsPressed()) {//Key::L + Key::DPadDown
			u32 wX, wY, u0;
			if(Dropper::DropCheck(&wX, &wY, &u0, 0, 0) && !GameHelper::GameSaving()) 
				Dropper::PlaceItemWrapper(DropType, ItemIDToReplace, &dropitem, &dropitem, wX, wY, 0, 0, 0, 0, 0, waitAnim, 0xA5);
		}
	}
//Auto Drop	
	void autoDrop(MenuEntry *entry) {
		static bool enabled = false;

		u8 drop;
		u32 wX, wY, u0;
		
		if(entry->Hotkeys[0].IsPressed()) 
			Wrap::KB<u32>(Language::getInstance()->get("ENTER_ID"), true, 8, *(u32 *)&dropitem, *(u32 *)&dropitem, ItemChange);
		
		else if(entry->Hotkeys[1].IsPressed()) {
			enabled = !enabled;
			OSD::Notify("Auto drop " << (enabled ? Color::Green << "ON" : Color::Red << "OFF"));
		}
		
		if(enabled) {
			if(Dropper::DropCheck(&wX, &wY, &u0, 0, 0) && !GameHelper::GameSaving()) 
				Dropper::PlaceItemWrapper(DropType, ItemIDToReplace, &dropitem, &dropitem, wX, wY, 0, 0, 0, 0, 0, waitAnim, 0xA5);
		}
	}
//Touch Drop
	void touchDrop(MenuEntry *entry) {
		if(entry->Hotkeys[0].IsPressed()) 
			Wrap::KB<u32>(Language::getInstance()->get("ENTER_ID"), true, 8, *(u32 *)&dropitem, *(u32 *)&dropitem, ItemChange);
		
		if((turbo ? Touch::IsDown() : Controller::IsKeyPressed(Key::Touchpad)) && GameHelper::MapBoolCheck() == 1) {
			UIntVector pos = Touch::GetPosition();
			u32 worldx, worldy; 
			switch(GameHelper::RoomCheck()) {
				default: return;
				case 0:
					if(pos.x >= 68 && pos.y >= 50 && pos.x <= 245 && pos.y <= 192) {
						worldx = ((pos.x - 68) / 2.2125) + 0x10;
						worldy = ((pos.y - 50) / 2.2125) + 0x10;
					}
					else 
						return;
				break;
				
				case 0x68:
					if(pos.x >= 116 && pos.y >= 70 && pos.x <= 201 && pos.y <= 157) {
						worldx = ((pos.x - 116) / 2.65) + 0x10;
						worldy = ((pos.y - 70) / 2.65) + 0x10;
					}
					else 
						return;
				break;
			}
			
			Dropper::PlaceItemWrapper(DropType, ItemIDToReplace, &dropitem, &dropitem, worldx, worldy, 0, 0, 0, 0, 0, waitAnim, 0xA5);
		}
	}
//Slot Drop
	void ShowInvSlotID(MenuEntry *entry) {
		static bool enabled = false;
		static Item dropitemid = {0x7FFE, 0};
		 
	//Auto Drop Hotkeys
		if(entry->Hotkeys[0].IsPressed()) {
			enabled = !enabled;
			OSD::Notify("Multi Slot Drop " << (enabled ? Color::Green << "ON" : Color::Red << "OFF"));
		}
		
		u8 slot = 0;
		if(Inventory::GetSelectedSlot(slot)) {
			Inventory::ReadSlot(slot, dropitemid);
			if(IDList::ValidID(*(u16 *)&dropitemid, 0x295B, 0x292F)) 
				GameHelper::ToOutdoorFlowers(dropitemid);

		//Auto Drop
			u32 wX, wY, u0;
			if(enabled) {
				if(Dropper::DropCheck(&wX, &wY, &u0, 0, 0) && !GameHelper::GameSaving()) 
					Dropper::PlaceItemWrapper(DropType, ItemIDToReplace, &dropitemid, &dropitemid, wX, wY, 0, 0, 0, 0, 0, waitAnim, 0xA5);
			}
			
		//Single Drop
			if(turbo ? entry->Hotkeys[1].IsDown() : entry->Hotkeys[1].IsPressed()) {
				if(Dropper::DropCheck(&wX, &wY, &u0, 0, 0) && !GameHelper::GameSaving()) 
					Dropper::PlaceItemWrapper(DropType, ItemIDToReplace, &dropitemid, &dropitemid, wX, wY, 0, 0, 0, 0, 0, waitAnim, 0xA5);
			}
		}	
	}
}
