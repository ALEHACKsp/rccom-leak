#pragma once
#include <vector>
#include "imgui.h"
#include <Windows.h>;
#include "Utils.h"
using namespace std;
struct Msg
{
	string Message;
	ULONGLONG TickAdded;
	ImVec2 SideRectPos;
};
vector<Msg>NotificationsList;
int CursorPos = 50;
void DrawNotifity(Msg* Message, float TextWidth)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 TextSize = ImGui::CalcTextSize(Message->Message.c_str(), 0, false, TextWidth);
	ULONGLONG CurrentTick = GetTickCount64();

	if (CurrentTick > Message->TickAdded + 2100)//? Add side rect pos
	{
		if (Message->SideRectPos.x > 0)
		{
			Message->SideRectPos.x -= 10;//? right side
		}
		else if(Message->SideRectPos.y > 0)
		{
			Message->SideRectPos.y -= 10;//? left side
		}
	}
	else
	{
		if (Message->SideRectPos.y < (TextSize.y * 0.7) + 10)//? left side
			Message->SideRectPos.y += 10;
		else if (Message->SideRectPos.x < (TextSize.y * 0.7))//? right side
		{
			Message->SideRectPos.x += 10;
		}
	}
	//? Draw message
	draw_list->AddRectFilled(ImVec2(Utils.Width - Message->SideRectPos.x, CursorPos), ImVec2(Utils.Width, CursorPos + 40),
		ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 0.94f)));
	draw_list->AddText(ImVec2(Utils.Width - (Message->SideRectPos.x * 0.95), CursorPos + (TextSize.x / 2)),
		ImGui::GetColorU32(ImVec4(1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f)), Message->Message.c_str());
	/////////////////////
	draw_list->AddRectFilled(ImVec2(Utils.Width - Message->SideRectPos.x, CursorPos), ImVec2(Utils.Width - Message->SideRectPos.y, CursorPos + 40),//? Side rect
		ImGui::GetColorU32(ImVec4(0.151f, 0.835f, 0.877f, 1.00f)));
	/////////////////////
	if (!Message->SideRectPos.y)
	{
		NotificationsList.erase(NotificationsList.begin());
	}
	CursorPos += 50;
}
void AddNotifity(const char* Message)
{
	Msg Temp;
	Temp.Message = string(Message);
	Temp.TickAdded = GetTickCount64();
	NotificationsList.push_back(Temp);
}
void DrawNotifications()
{
	if (NotificationsList.size() > 0)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushFont(io.Fonts->Fonts[2]);
		for (auto& i : NotificationsList)
		{
			DrawNotifity(&i, 16);
			//NotificationsList.erase(NotificationsList.begin());
		}
		ImGui::PopFont();
	}
	CursorPos = 50;
}