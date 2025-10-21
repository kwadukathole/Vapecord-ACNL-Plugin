#include "cheats.hpp"
#include "Helpers/GameKeyboard.hpp"

#include "Files.h"
#include "NonHacker.hpp"

namespace CTRPluginFramework {
//Chat Bubbles Don't Disappear /*Credits to Levi*/
	void bubblesDisappear(MenuEntry *entry) { 
		static const Address bubble("BUBBLE");
		if(entry->WasJustActivated()) 
			Process::Patch(bubble.addr, 0xE1A00000);
		else if(!entry->IsActivated()) 
			Process::Patch(bubble.addr, 0x0A000006);
	}

	static std::string Holder = "";
//65 char is max (0x82)
	void ChatCopyPaste(MenuEntry *entry) {
		if(entry->WasJustActivated()) {
			if(!File::Exists(Utils::Format(PATH_CBOARD, Address::regionName.c_str()))) 
				File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

			Holder.clear();
			File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::READ);
			LineReader reader(f_board);
			reader(Holder);
			Holder.resize(65);
			Holder.shrink_to_fit();

			f_board.Flush();
			f_board.Close();
		}

		if(entry->Hotkeys[0].IsPressed()) {
			Holder.clear();
			File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::READ);
			LineReader reader(f_board);
			reader(Holder);
			Holder.resize(65);
			Holder.shrink_to_fit();

			f_board.Flush();
			f_board.Close();

			if(GameKeyboard::Write(Holder))
				OSD::Notify("Pasted: " << Holder, Color(0x0091FFFF));
		}

		else if(entry->Hotkeys[1].IsPressed()) {
			Holder.clear();
			if(GameKeyboard::CopySelected(Holder)) {
				File::Remove(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));
				File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

				File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::WRITE);

				LineWriter writer(f_board);
				writer << Holder;
				writer.Flush();
				writer.Close();

				f_board.Flush();
				f_board.Close();

				OSD::Notify("Copied: " << Holder, Color(0xFF0077FF));
			}
		}

		else if(entry->Hotkeys[2].IsPressed()) {
			Holder.clear();
			if(GameKeyboard::CopySelected(Holder)) {
				GameKeyboard::DeleteSelected();

				File::Remove(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));
				File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

				File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::WRITE);
				
				LineWriter writer(f_board);
				writer << Holder;
				writer.Flush();
				writer.Close();

				f_board.Flush();
				f_board.Close();

				OSD::Notify("Cut: " << Holder, Color(0x00FF6FFF));
			}
		}

		else if(entry->Hotkeys[3].IsPressed()) {
			if(GameKeyboard::DeleteSelected())
				OSD::Notify("Deleted Selected", Color::Red);
		}
	}

//Force Send Chat
	void Forcesendchat(MenuEntry *entry) {
		static Address callchat("CALLCHAT");
		if(entry->WasJustActivated())
			Process::Patch(Address("DISABLECHATREMOVAL").addr, 0xEA000000);

		if(entry->Hotkeys[0].IsDown()) {
			if(!GameKeyboard::IsOpen())
				return;

			callchat.Call<void>(1, 2);
		}

		if(!entry->IsActivated())
			Process::Patch(Address("DISABLECHATREMOVAL").addr, 0xE5900000);
	}

//ShowChatMessage
	static std::vector<std::string> chat_text = { "", "", "", "", "" };

	void GetMessage_p1(void) {	
		NonHacker *nHack1 = new NonHacker(0);
		std::string PlayerText1 = nHack1->GetPlayerMessage();
		if(PlayerText1.empty()) PlayerText1 = chat_text[0];
		
		chat_text[0] = (PlayerText1);
		delete nHack1;
	}

	void GetMessage_p2(void) {	
		NonHacker *nHack2 = new NonHacker(1);
		std::string PlayerText2 = nHack2->GetPlayerMessage();
		if(PlayerText2.empty()) PlayerText2 = chat_text[1];
		
		chat_text[1] = (PlayerText2);
		delete nHack2;
	}

	void GetMessage_p3(void) {	
		NonHacker *nHack3 = new NonHacker(2);
		std::string PlayerText3 = nHack3->GetPlayerMessage();
		if(PlayerText3.empty()) PlayerText3 = chat_text[2];
		
		chat_text[2] = (PlayerText3);
		delete nHack3;
	}

	void GetMessage_p4(void) {	
		NonHacker *nHack4 = new NonHacker(3);
		std::string PlayerText4 = nHack4->GetPlayerMessage();
		if(PlayerText4.empty()) PlayerText4 = chat_text[3];
		
		chat_text[3] = (PlayerText4);
		delete nHack4;
	}
	
	bool messageOSD(const Screen &screen) {
		if(!screen.IsTop)
			return 0;
		
		static constexpr u8 YPositions1[4] = { 189, 201, 213, 225 };

		screen.DrawSysfont(pColor[0] << "P1: " << Color(0xFFFFFFFF) << chat_text[0], 0, YPositions1[0]);
		screen.DrawSysfont(pColor[1] << "P2: " << Color(0xFFFFFFFF) << chat_text[1], 0, YPositions1[1]);
		screen.DrawSysfont(pColor[2] << "P3: " << Color(0xFFFFFFFF) << chat_text[2], 0, YPositions1[2]);
		screen.DrawSysfont(pColor[3] << "P4: " << Color(0xFFFFFFFF) << chat_text[3], 0, YPositions1[3]);

		return 1;
	}

	void ShowChatMessage(MenuEntry *entry) {

		if(entry->WasJustActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu += GetMessage_p1;
			*menu += GetMessage_p2;
			*menu += GetMessage_p3;
			*menu += GetMessage_p4;
			OSD::Run(messageOSD); 
		}
		else if(!entry->IsActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu -= GetMessage_p1;
			*menu -= GetMessage_p2;
			*menu -= GetMessage_p3;
			*menu -= GetMessage_p4;
			OSD::Stop(messageOSD); 
		}
	}

// チャット ボタン表示関係
	int ButtonList[5][3] = 
		{
			{16, 5, 4}, //Copy
			{48, 5, 5}, //Paste
			{96, 5, 3}, //Cut
			{128, 5, 6}, //Delete
			{170, 5, 4}, //Send

		};

	int TouchButton(void)
	{
		static UIntRect CopyCoord(ButtonList[0][0], ButtonList[0][1], ButtonList[0][2]*6, 11);
		static UIntRect PasteCoord(ButtonList[1][0], ButtonList[1][1], ButtonList[1][2]*6, 11);
		static UIntRect CutCoord(ButtonList[2][0], ButtonList[2][1], ButtonList[2][2]*6, 11);
		static UIntRect DeleteCoord(ButtonList[3][0], ButtonList[3][1], ButtonList[3][2]*6, 11);
		static UIntRect SendCoord(ButtonList[4][0], ButtonList[4][1], ButtonList[4][2]*6, 11);
		if (CopyCoord.Contains(Touch::GetPosition())) return 1;
		if (PasteCoord.Contains(Touch::GetPosition())) return 2;
		if (CutCoord.Contains(Touch::GetPosition())) return 3;
		if (DeleteCoord.Contains(Touch::GetPosition())) return 4;
		if (SendCoord.Contains(Touch::GetPosition())) return 5;
		return 0;
	}

	bool DrawChatButton(const Screen &scr)
	{ // input 下 54
		if(!GameKeyboard::IsOpen()) return false;
		if ( scr.IsTop ) return false;
		scr.Draw( "Copy", ButtonList[0][0], ButtonList[0][1], Color::Yellow, Color::Black);
		scr.Draw( "Paste", ButtonList[1][0], ButtonList[1][1], Color::Yellow, Color::Black);
		scr.Draw( "Cut", ButtonList[2][0], ButtonList[2][1], Color::Yellow, Color::Black);
		scr.Draw( "Delete", ButtonList[3][0], ButtonList[3][1], Color::Yellow, Color::Black);
		scr.Draw( "Send", ButtonList[4][0], ButtonList[4][1], Color::Yellow, Color::Black);
		
		return true;
	}

	// チャットに便利なボタンを追加します。
	// Add a convenient button to chat.
	void ChatButton(MenuEntry *entry) {
		static Address callchat("CALLCHAT");

		PluginMenu *menu = PluginMenu::GetRunningInstance();
		if(entry->WasJustActivated()) {
			OSD::Run(DrawChatButton); 
			if(!File::Exists(Utils::Format(PATH_CBOARD, Address::regionName.c_str()))) 
				File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

			Holder.clear();
			File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::READ);
			LineReader reader(f_board);
			reader(Holder);
			Holder.resize(65);
			Holder.shrink_to_fit();

			f_board.Flush();
			f_board.Close();
		}
		else if(!entry->IsActivated()) {
			OSD::Stop(DrawChatButton); 
		}
		if (Controller::IsKeyPressed(Touchpad) && GameKeyboard::IsOpen())
		{
			int ButtonID = TouchButton();
			switch (ButtonID) {
				case 1: // Copy
					{
						Holder.clear();
						if (GameKeyboard::CopySelected(Holder)) {
							File::Remove(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));
							File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

							File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::WRITE);

							LineWriter writer(f_board);
							writer << Holder;
							writer.Flush();
							writer.Close();

							f_board.Flush();
							f_board.Close();
							OSD::Notify("Copied", Color(0xFF0077FF));
						}
					}
					break;
				case 2: // Paste
					{
						Holder.clear();
						File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::READ);
						LineReader reader(f_board);
						reader(Holder);
						Holder.resize(65);
						Holder.shrink_to_fit();

						f_board.Flush();
						f_board.Close();

						GameKeyboard::Write(Holder);
					}
					break;
				case 3: // Cut
					{
						Holder.clear();
						if (GameKeyboard::CopySelected(Holder)){
							GameKeyboard::DeleteSelected();

							File::Remove(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));
							File::Create(Utils::Format(PATH_CBOARD, Address::regionName.c_str()));

							File f_board(Utils::Format(PATH_CBOARD, Address::regionName.c_str()), File::WRITE);
							
							LineWriter writer(f_board);
							writer << Holder;
							writer.Flush();
							writer.Close();

							f_board.Flush();
							f_board.Close();

							OSD::Notify("Cut", Color(0x00FF6FFF));
						}
					}
					break;
				case 4: // Delete
					GameKeyboard::Delete();
					break;
				case 5: // Send
					callchat.Call<void>(1, 2);
					break;
				default:
					break;
			}
		}
	}
}
