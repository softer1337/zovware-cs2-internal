#include "../../features.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/math.hpp"
#include "../../../sdk/c_csplayerpawn.hpp"
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

    // 8 точек бокса
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

	for (int i = 1; i < 64; i++)
	{
		uintptr_t controller = MEM::GetEntityByIndex(entityList, i);
		if (!controller) continue;

		uint32_t pawnHandle = MEM::read<uint32_t>(controller + offsets::m_hPlayerPawn);
		if (!pawnHandle) continue;

        C_CSPlayerPawn* playerPawn = (C_CSPlayerPawn*)MEM::GetEntityByHandle(entityList, pawnHandle);
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


		}
	}

}