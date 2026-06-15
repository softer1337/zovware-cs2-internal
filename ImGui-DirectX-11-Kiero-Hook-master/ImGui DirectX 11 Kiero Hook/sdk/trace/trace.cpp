#include "trace.h"
#include "../../core/hooks/pattern.h"
#include "../offsets.h"
#include "../bone.hpp"


bool Tracing::TraceShape(Ray_t* pRay, Vec3 vecStart, Vec3 vecEnd, Trace_Filter_t* pFilter, CGameTrace* pGameTrace)
{
	using fnTraceShape = bool(__fastcall*)(void*, Ray_t*, Vec3*, Vec3*, Trace_Filter_t*, CGameTrace*);
	static fnTraceShape oTraceShape = reinterpret_cast<fnTraceShape>(PatternScan("client.dll", "48 89 5C 24 ? 48 89 4C 24 ? 55 57"));


	return oTraceShape(this, pRay, &vecStart, &vecEnd, pFilter, pGameTrace);
}

void Tracing::Init(Trace_Filter_t& Filter, C_CSPlayerPawn* Skip, uint64_t Mask, uint8_t Layer, uint16_t Idk)
{
	using InitTraceFilterFunction = Trace_Filter_t * (__fastcall*)(Trace_Filter_t&, void*, uint64_t, uint8_t, uint16_t);
	static InitTraceFilterFunction InitTraceFilterFn = nullptr;

	if (InitTraceFilterFn == nullptr)
		InitTraceFilterFn = reinterpret_cast<InitTraceFilterFunction>(PatternScan("client.dll", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 41 ? 33 FF 24"));

	if (InitTraceFilterFn)
		InitTraceFilterFn(Filter, Skip, Mask, Layer, Idk);
}

TraceData_t* Tracing::InitTraceData(TraceData_t* pTraceData)
{
	using InitTraceData_t = TraceData_t*(__fastcall*)(TraceData_t*);
	static InitTraceData_t fnInitTraceData = reinterpret_cast<InitTraceData_t>(PatternScan("client.dll", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8D 79 ? 33 F6 C7 47"));
	return fnInitTraceData(pTraceData);
}

bool Tracing::ClipRayToEntity(Ray_t* pRay, Vec3 vecStart, Vec3 vecEnd, C_CSPlayerPawn* pEntity, Trace_Filter_t* pFilter, CGameTrace* pGameTrace)
{
	using ClipRayToEntity_t = bool(__fastcall*)(void*, Ray_t*, Vec3*, Vec3*, void*, Trace_Filter_t*, CGameTrace*);
	static ClipRayToEntity_t fnClipRayToEntity = reinterpret_cast<ClipRayToEntity_t>(PatternScan("client.dll", "48 8B C4 48 89 58 ?? 48 89 68 ?? 56 57 41 54 48 81 EC"));
	return fnClipRayToEntity(this, pRay, &vecStart, &vecEnd, pEntity, pFilter, pGameTrace);
}

Trace_Filter_t* Tracing::InitializeTraceFilter(Trace_Filter_t* filter, C_CSPlayerPawn* Skip, int64_t Mask, uint8_t Layer1, uint8_t Layer2)
{
	using InitializeTraceFilter_t = Trace_Filter_t*(__fastcall*)(Trace_Filter_t*, C_CSPlayerPawn*, int64_t, uint8_t, uint8_t);
	static InitializeTraceFilter_t fnInitializeTraceFilter = reinterpret_cast<InitializeTraceFilter_t>(PatternScan("client.dll", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 41 ? 33 FF C7 41 ?"));
	return fnInitializeTraceFilter(filter, Skip, Mask, Layer1, Layer2);
}

uint64_t Tracing::DamageToPoint(TraceData_t* trace_data, float damage, float penetration, float range_modifier, int team_num)
{
	using DamageToPoint_t = uint64_t(__fastcall*)(TraceData_t*, float, float, float, int, int, void*);
	static DamageToPoint_t nigger = (DamageToPoint_t)PatternScan("client.dll", "40 53 57 41 56 48 83 EC ? 8B 84 24");

	return nigger(trace_data, damage, penetration, range_modifier, 4, team_num, nullptr);
}

void Tracing::InitializeTraceInfo(CGameTrace* pGameTrace)
{
	using InitializeTraceInfo_t = void(__fastcall*)(CGameTrace*);
	static InitializeTraceInfo_t fnInitializeTraceInfo = reinterpret_cast<InitializeTraceInfo_t>(PatternScan("client.dll", "40 55 41 55 41 57 48 83 EC 30"));

	fnInitializeTraceInfo(pGameTrace);
}

void Tracing::GetTraceInfo(TraceData_t* trace, CGameTrace* hit, const float unknown_float, c_segment_holder* unknown) {
	using GetTraceInfo_t = void(__fastcall*)(TraceData_t*, CGameTrace*, float, c_segment_holder*);
	static GetTraceInfo_t fnGetTraceInfo = reinterpret_cast<GetTraceInfo_t>(PatternScan("client.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B E9 0F 29 74 24"));
	__debugbreak();
	return fnGetTraceInfo(trace, hit, unknown_float, unknown);
}

void Tracing::CreateTrace(TraceData_t* const trace, const Vec3 start, const Vec3 end, const Trace_Filter_t* filler, const int penetration_count) {
	//using CreateTrace_t = void(__fastcall*)(TraceData_t*, Vec3, Vec3, Trace_Filter_t, int, bool);
	using CreateTrace_t =
		__int64(__fastcall*)(
			TraceData_t* trace,
			const Vec3 start,
			const Vec3 end,
			const Trace_Filter_t* filter,
			int penetration_count,
			bool unk
			);
	static CreateTrace_t fnCreateTrace = reinterpret_cast<CreateTrace_t>(PatternScan("client.dll", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 50 F2 0F 10 02"));

	fnCreateTrace(trace, start, end, filler, 4, true);
}


bool Tracing::HandleBulletPenetration(TraceData_t* const pTraceData, HandleBulletPenetrationData_t* pHandleBulletPenetrationData, BulletModulateEntry_t* const pModValue, int team_num, const bool bDrawShowimpacts) {
	using HandleBulletPenetration_t = bool(__fastcall*)(TraceData_t*, HandleBulletPenetrationData_t*, BulletModulateEntry_t*, int, void*);
	static HandleBulletPenetration_t fnHandleBulletPenetration = reinterpret_cast<HandleBulletPenetration_t>(PatternScan("client.dll", "48 8B C4 44 89 48 ? 48 89 50 ? 48 89 48 ? 55 57"));

	return fnHandleBulletPenetration(pTraceData, pHandleBulletPenetrationData, pModValue,team_num , nullptr);
}

bool IsValidPos(const Vec3& v)
{
	return std::isfinite(v.x) &&
		std::isfinite(v.y) &&
		std::isfinite(v.z);
}

bool IsInWorldBounds(const Vec3& v)
{
	if (!IsValidPos(v))
		return false;

	const float limit = 50000.f;

	return fabs(v.x) < limit &&
		fabs(v.y) < limit &&
		fabs(v.z) < limit;
}

CGameTrace traceRay(Vec3 start, Vec3 end, uintptr_t ignore) {

	Ray_t Ray;
	CGameTrace GameTrace;
	Trace_Filter_t Filter;

	if (!IsInWorldBounds(end)) return{};
	if (!IsInWorldBounds(start)) return{};

	Tracing* tracing = Interface::GetTraceManager();
	tracing->Init(Filter, reinterpret_cast<C_CSPlayerPawn*>(ignore), 0x1C100B, 0x4, 7);
	tracing->TraceShape(&Ray, start, end, &Filter, &GameTrace);

	return GameTrace;

}

bool isVisibleBone(int boneIndex, uintptr_t localPawn, uintptr_t target) {
	Vec3 start = MEM::read<Vec3>(localPawn + offsets::m_vOldOrigin) + MEM::read<Vec3>(localPawn + offsets::m_vecViewOffset);

	Vec3 end = GetBonePosition(boneIndex, target);

	CGameTrace GameTrace = traceRay(start, end, localPawn);

	return GameTrace.IsVisible();

}
