#pragma once
#include <cstdint>
#include "../math.hpp"
#include <array>
#include "../c_csplayerpawn.hpp"

struct Ray_t
{
public:
	Vec3 m_vecStart;
	Vec3 m_vecEnd;
	Vec3 m_vecMins;
	Vec3 m_vecMaxs;
	char pad[0x4];
	std::uint8_t UnkType;
};


class CHitBox
{
public:
	const char* szName; // 0x0
	const char* szSurfaceProperty; // 0x8
	const char* szBoneName; // 0x10
	Vec3 vecMinBounds; // 0x18
	Vec3 vecMaxBounds; // 0x24
	float flShapeRadius; // 0x30
	uint32_t uBoneNameHash; // 0x34
	int nGroupId; // 0x38
	uint8_t nShapeType; // 0x3C
	bool bTranslationOnly; // 0x3D	
private:
	char pad0[2];
public:
	uint32_t uCRC; // 0x40	
	uint32_t colRender; // 0x44	
	uint16_t nHitBoxIndex; // 0x48	
};



class alignas(16) CGameTrace
{
public:
	void* pSurfaceProperties;
	C_CSPlayerPawn* pHitEntity;
	CHitBox* pHitBox;
private:
	char pad_0x38[0x10];
public:
	uint32_t nSurfaceFlags;
private:
	char pad_0x24[0x4A];
public:
	Vec3 vecStart;
	Vec3 vecEnd;
	Vec3 vecNormal;
	Vec3 vecPosition;
private:
	char pad_0x4[0x4];
public:
	float flFraction;
private:
	char pad_0x6[0x12];
public:
	bool m_all_solid;
private:
	char pad_0x9[0x4D];
public:

	inline bool DidHit() const
	{
		return (flFraction < 1.0f || m_all_solid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}
};
struct TraceArrElement_t {
	char pad0[0x30];
};

struct TraceArrayElement_t { char data[0x30]; };
struct BulletModulateEntry_t {
	float startFrac; float endFrac; float damage; int maxSecondaryTraces;
	uint16_t surfStart; uint16_t surfEnd; uint8_t flags; uint8_t pad[3];
};
struct BulletModArray_t {
	int size; char pad4[4]; BulletModulateEntry_t* data; char pad16[8];
};
struct alignas(16) TraceData_t {
	char pad0[24];
	TraceArrayElement_t m_Arr[0x80];
	char pad6168[8];
	BulletModArray_t mod_array;
	BulletModulateEntry_t mod_inline[8];
	Vec3 tail_start;
	Vec3 tail_end;
	char _pad6200[12];
};

struct RnQueryShapeAttr_t
{
	std::uint64_t m_nInteractsWith;                    // 0x00
	std::uint64_t m_nInteractsExclude;                 // 0x08
	std::uint64_t m_nInteractsAs;                      // 0x10
	std::uint32_t m_nEntityIdsToIgnore[2];             // 0x18
	std::uint32_t m_nOwnerIdsToIgnore[2];              // 0x20
	std::uint16_t m_nHierarchyIds[2];                  // 0x28
	std::uint16_t m_nIncludedDetailLayers = 0xFFFF;    // 0x2C
	std::uint8_t m_nTargetDetailLayer = 0;             // 0x2E
	std::uint8_t m_nObjectSetMask;                     // 0x2F
	std::uint8_t m_nCollisionGroup;                    // 0x30
	bool m_bHitSolid : 1;                              // 0x31
	bool m_bHitSolidRequiresGenerateContacts : 1;      // 0x31
	bool m_bHitTrigger : 1;                            // 0x31
	bool m_bShouldIgnoreDisabledPairs : 1;             // 0x31
	bool m_bIgnoreIfBothInteractWithHitBoxes : 1;      // 0x31
	bool m_bForceHitEverything : 1;                    // 0x31
	bool m_bUnknown : 1;                               // 0x31
private:
	bool pad_bitfield_31_7 : 1;
};
static_assert(sizeof(RnQueryShapeAttr_t) == 0x38);


struct alignas(16) Trace_Filter_t
{
	char pad[164];
}; // 0x48



struct HandleBulletPenetrationData_t {
	float damage;       
	float penetration;     
	float rangeModifier;     
	float tailLength;        
	int penetrationCount;  
	bool penetrationStopped; 
	char pad[3];
};

struct UpdateValue_t {
	float flPreviousLenght{};
	float flCurrentLenght{};
	char pad1[0x8];
	std::int16_t nHandleIndex{};
	char pad2[0x6];
};

class Tracing
{
public:
	bool TraceShape(Ray_t* pRay, Vec3 vecStart, Vec3 vecEnd, Trace_Filter_t* pFilter, CGameTrace* pGameTrace);

	void Init(Trace_Filter_t& Filter, C_CSPlayerPawn* Skip, uint64_t Mask, uint8_t Layer, uint16_t Idk);

	void InitializeTraceInfo(CGameTrace* pGameTrace);

	void GetTraceInfo(TraceData_t* trace, CGameTrace* hit, const float unknown_float, void* unknown);

	void CreateTrace(TraceData_t* const trace, const Vec3 start, const Vec3 end, const Trace_Filter_t* filler, const int penetration_count);

	bool HandleBulletPenetration(TraceData_t* const pTraceData, HandleBulletPenetrationData_t* pHandleBulletPenetrationData, BulletModulateEntry_t* const pModValue, int team_num, const bool bDrawShowimpacts);

	void InitTraceData(TraceData_t* pTraceData);

	bool ClipRayToEntity(Ray_t* pRay, Vec3 vecStart, Vec3 vecEnd, C_CSPlayerPawn* pEntity, Trace_Filter_t* pFilter, CGameTrace* pGameTrace);

};
CGameTrace traceRay(Vec3 start, Vec3 end, uintptr_t ignore);
bool isVisibleBone(int boneIndex, uintptr_t localPawn, uintptr_t target);