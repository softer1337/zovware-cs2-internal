#include "../../features.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/math.hpp"
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../entity/entity_cache.hpp"
#define M_PI       3.14159265358979323846 
static Vec3 AngleToVector(const Vec3& angles) {
    float pitch = angles.x * (M_PI / 180.0f);
    float yaw = angles.y * (M_PI / 180.0f);

    float cp = cosf(pitch);
    float sp = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);

    return Vec3(cp * cy, cp * sy, -sp);
}

void DrawCorner(ImVec2 start, ImVec2 end, ImU32 color)
{
    float w = end.x - start.x;
    float h = end.y - start.y;

    float lw = w * 0.25f;
    float lh = h * 0.25f;

    auto d = ImGui::GetBackgroundDrawList();

    d->AddLine({ start.x, start.y }, { start.x + lw, start.y }, color, 2.f);
    d->AddLine({ start.x, start.y }, { start.x, start.y + lh }, color, 2.f);

    d->AddLine({ end.x, start.y }, { end.x - lw, start.y }, color, 2.f);
    d->AddLine({ end.x, start.y }, { end.x, start.y + lh }, color, 2.f);

    d->AddLine({ start.x, end.y }, { start.x + lw, end.y }, color, 2.f);
    d->AddLine({ start.x, end.y }, { start.x, end.y - lh }, color, 2.f);

    d->AddLine({ end.x, end.y }, { end.x - lw, end.y }, color, 2.f);
    d->AddLine({ end.x, end.y }, { end.x, end.y - lh }, color, 2.f);
}

void Draw3DBox(const Vec3& origin, float width, float height,
    const float viewMatrix[4][4], ImU32 color)
{
    float w = width * 0.5f;

    // 8 ����� �����
    Vec3 points[8] = {
        {origin.x - w, origin.y - w, origin.z},
        {origin.x + w, origin.y - w, origin.z},
        {origin.x + w, origin.y + w, origin.z},
        {origin.x - w, origin.y + w, origin.z},   

        {origin.x - w, origin.y - w, origin.z + height}, 
        {origin.x + w, origin.y - w, origin.z + height}, 
        {origin.x + w, origin.y + w, origin.z + height}, 
        {origin.x - w, origin.y + w, origin.z + height} 
    };

    Vec2 screen[8];

    for (int i = 0; i < 8; i++)
    {
        if (!WorldToScreen(points[i], screen[i], viewMatrix))
            return;
    }

    auto d = ImGui::GetBackgroundDrawList();

    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, 
        {4,5},{5,6},{6,7},{7,4}, 
        {0,4},{1,5},{2,6},{3,7}  
    };

    for (auto& e : edges)
    {
        d->AddLine(
            { screen[e[0]].x, screen[e[0]].y },
            { screen[e[1]].x, screen[e[1]].y },
            color,
            1.5f
        );
    }
}

void drawBox(ImVec2 start, ImVec2 end, Vec3 origin, const float viewMatrix[4][4])
{
    ImU32 color = CFG::VISUAL::ESP::espColor;

    switch (CFG::VISUAL::ESP::espType)
    {
    case EspType::BOX:
        ImGui::GetBackgroundDrawList()->AddRect(
            start, end, color, 0.0f, 0, 2.0f
        );
        break;

    case EspType::CORNER:
        DrawCorner(start, end, color);
        break;

    case EspType::BOX_3D:
        Draw3DBox(origin, 20.0f * 1.6f, 75.0f, viewMatrix, color);
        break;
    }
}

void drawHP(ImVec2 start, ImVec2 end, int health) {
	if (!CFG::VISUAL::ESP::drawHealthBar) return;

	float barWidth = 3.0f;
	ImVec2 barStart = ImVec2(start.x - 5.0f, start.y);
	ImVec2 barEnd = ImVec2(start.x - 2.0f, end.y);
	
	// Background
	ImGui::GetBackgroundDrawList()->AddRect(barStart, barEnd, ImColor(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0, 1.5f);
	
	// Health bar
	float barHeight = barEnd.y - barStart.y;
	float healthPercent = health / 100.0f;
	float healthBarHeight = barHeight * healthPercent;
	
	ImVec2 healthEnd = ImVec2(barEnd.x, barEnd.y - healthBarHeight);
	
	ImU32 healthColor;
	if (health > 75) healthColor = ImColor(0.0f, 1.0f, 0.0f, 1.0f);
	else if (health > 50) healthColor = ImColor(1.0f, 1.0f, 0.0f, 1.0f);
	else if (health > 25) healthColor = ImColor(1.0f, 0.5f, 0.0f, 1.0f);
	else healthColor = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
	
	ImGui::GetBackgroundDrawList()->AddRectFilled(healthEnd, barEnd, healthColor);
	
	// Health text
	char healthText[16];
	//sprintf_s(healthText, "%d", health);
	ImGui::GetBackgroundDrawList()->AddText(
		ImGui::GetFont(),
		ImGui::GetFontSize(),
		ImVec2(start.x - 30.0f, start.y),
		ImColor(1.0f, 1.0f, 1.0f, 1.0f),
		healthText
	);
}

void drawAmmo(ImVec2 start, ImVec2 end, C_CSPlayerPawn* player) {
	//if (!CFG::VISUAL::ESP::drawAmmo || !player) return;

	//// Get active weapon and ammo info
	//CPlayer_WeaponServices* weaponServices = player->m_pWeaponServices();
	//if (!weaponServices) return;

	//C_CSWeaponBase* activeWeapon = (C_CSWeaponBase*)weaponServices->m_hActiveWeapon().get();
	//if (!activeWeapon) return;

	//int ammoInClip = activeWeapon->m_iClip1();
	//int ammoInReserve = weaponServices->m_iAmmo()[0];

	//char ammoText[32];
	//sprintf_s(ammoText, "%d/%d", ammoInClip, ammoInReserve);

	//ImGui::GetBackgroundDrawList()->AddText(
	//	ImGui::GetFont(),
	//	ImGui::GetFontSize(),
	//	ImVec2(start.x, end.y + 3.0f),
	//	ImColor(1.0f, 1.0f, 1.0f, 1.0f),
	//	ammoText
	//);
}

void drawNickname(ImVec2 start, C_CSPlayerPawn* player) {
	if (!CFG::VISUAL::ESP::drawNickname || !player) return;

	C_CSPlayerController* controller = (C_CSPlayerController*)player->m_hController().get();
	if (!controller) return;

	const char* nickname = controller->m_iszPlayerName();
	if (!nickname) nickname = "Unknown";

	ImGui::GetBackgroundDrawList()->AddText(
		ImGui::GetFont(),
		ImGui::GetFontSize(),
		ImVec2(start.x, start.y - 15.0f),
		ImColor(1.0f, 1.0f, 1.0f, 1.0f),
		nickname
	);
}

void FEATURES::VISUAL::ESP::onRender() {
	if (!CFG::VISUAL::ESP::isEspEnabled) return;
    if (!MEM::IsInGame()) return;

	uintptr_t client = MEM::GetClient();
	if (!client) return;

	C_CSPlayerPawn* localPawn = (C_CSPlayerPawn*)MEM::GetLocalPawn();
	if (!localPawn) return;

	int localTeam = localPawn->m_iTeamNum();

	uintptr_t entityList = MEM::read<uintptr_t>(client + offsets::dwEntityList);
	if (!entityList) return;

	float viewMatrix[4][4];
	memcpy(&viewMatrix, (void*)(client + offsets::dwViewMatrix), sizeof(viewMatrix));

    for (int i = 0; i < CachedPlayers.size(); i++)
    {
        C_CSPlayerPawn* playerPawn = CachedPlayers[i].pawn;
		if (!playerPawn || playerPawn == localPawn) continue;
		

        int health = playerPawn->m_iHealth();
		if (health <= 0 || health > 100) continue;

		int team = playerPawn->m_iTeamNum();
		bool isTeammate = (team == localTeam);
		if (isTeammate) continue;


		Vec3 feet = playerPawn->m_vOldOrigin();
		Vec3 head = feet;
		head.z += 75.0f;

		Vec2 screenFeet, screenHead;

		if (WorldToScreen(feet, screenFeet, viewMatrix) && WorldToScreen(head, screenHead, viewMatrix)) {
			float height = screenFeet.y - screenHead.y;
			float width = height / 2.0f;

			float x = screenHead.x - width / 2.0f;
			float y = screenHead.y;
		
			drawBox(ImVec2(x, y), ImVec2(x + width, y + height), feet, viewMatrix);
			drawHP(ImVec2(x, y), ImVec2(x + width, y + height), health);
			drawAmmo(ImVec2(x, y), ImVec2(x + width, y + height), playerPawn);
			drawNickname(ImVec2(x, y), playerPawn);
		}
	}

}