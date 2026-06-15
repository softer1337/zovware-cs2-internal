#pragma once
#include <cstdint>
#include "../math.hpp"
#include <array>
#include "../c_csplayerpawn.hpp"
#include "../econsdk.hpp"

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
	char        _pad_0000[0x38];           // 0x00
	int32_t     m_hitgroup;                // 0x38
	char        _pad_003c[0x04];           // 0x3C
	int32_t     m_hitbox_id;               // 0x40
	char        _pad_0044[0x5C];           // 0x44
};


struct alignas(16) CGameTrace {
	void* m_p_surface;              // 0x00
	C_CSPlayerPawn* m_pHitEntity;             // 0x08 
	CHitBox* m_p_hitbox_data;          // 0x10
	char             _pad1[0x38];              // 0x18
	uint32_t         m_contents;               // 0x50
	char             _pad2[0x24];              // 0x54
	Vec3           m_start_pos;              // 0x78
	Vec3           m_end_pos;                // 0x84
	Vec3           m_normal;                 // 0x90
	Vec3           m_pos;                    // 0x9C
	char             _pad3[4];                 // 0xA8
	float            fraction;                 // 0xAC
	int32_t          m_debug_or_surface_index; // 0xB0
	uint16_t         m_hitbox_bone_or_index;   // 0xB4
	uint8_t          m_ray_type;               // 0xB6
	uint8_t          m_hit_flag;               // 0xB7
	char             _pad_tail[0x88];          // 0xB8

	inline void Zero() {
		std::memset(this, 0, sizeof(CGameTrace));
	}
	inline bool DidHit() const
	{
		return (fraction < 1.0f);
	}

	inline bool IsVisible() const
	{
		return (fraction > 0.97f);
	}

};
static_assert(sizeof(CGameTrace) == 0x140, "CGameTrace size mismatch");

struct TraceArrElement_t
{
	char pad_0000[0x30];
};

class c_trace_info {
public:
	float m_unk;
	float m_distance;
	float m_damage;
	std::uint32_t m_pen_count;
	c_base_handle m_handle;
	std::uint32_t m_penetration_flags;
};


struct BulletModulateEntry_t {
	float startFrac; float endFrac; float damage; int maxSecondaryTraces;
	uint16_t surfStart; uint16_t surfEnd; uint8_t flags; uint8_t pad[3];
};
struct c_segment_holder {
	Vec3 m_hit_normal;
	float m_fraction;
	void* m_body;
	void* m_shape;
	uint16_t m_bone_index;
	c_base_handle m_entity_handle;
	__int16 m_surface_count;
	bool m_start_in_solid;
	bool m_hit_point;
};

struct TraceData_t {
	std::int32_t m_filled_segment_count{ };
	float m_unk2{ 52.0f };
	c_segment_holder* m_trace_segments_ptr{ };
	std::int32_t m_trace_segments_count{ 128 };
	std::int32_t m_trace_segments_count2{ static_cast<std::int32_t>(0x80000000) };
	TraceArrElement_t m_trace_segments[0x80] = {};
	void* m_unk_zeroed;
	int m_surfaces_count; // 0x1820
	c_trace_info* m_trace_info; // 0x1828
	int m_unk_i; // 0x1830
	void* m_unkn_ptr; // 0x1838
	char pad_0000[0xB8]; // 0x1840
	Vec3 m_start; // 0x18F8
	Vec3 m_end; // 0x1904
	char pad_0001[0x50];
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


struct Trace_Filter_t
{
	char pad_0000[8];
	int64_t m_mask;
	std::array<int64_t, 2> m_ptr{};
	std::array<int32_t, 4> m_skip_handles{};
	std::array<int16_t, 2> m_arr_collisions{};
	uint8_t m_ptr2{};
	uint8_t m_ptr3{};
	uint8_t m_ptr4{};
	uint8_t m_ptr5{};
	uint8_t m_collision{};
}; 



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

	void GetTraceInfo(TraceData_t* trace, CGameTrace* hit, const float unknown_float, c_segment_holder* unknown);

	void CreateTrace(TraceData_t* const trace, const Vec3 start, const Vec3 end, const Trace_Filter_t* filler, const int penetration_count);

	bool HandleBulletPenetration(TraceData_t* const pTraceData, HandleBulletPenetrationData_t* pHandleBulletPenetrationData, BulletModulateEntry_t* const pModValue, int team_num, const bool bDrawShowimpacts);

	TraceData_t* InitTraceData(TraceData_t* pTraceData);

	bool ClipRayToEntity(Ray_t* pRay, Vec3 vecStart, Vec3 vecEnd, C_CSPlayerPawn* pEntity, Trace_Filter_t* pFilter, CGameTrace* pGameTrace);

	Trace_Filter_t* InitializeTraceFilter(Trace_Filter_t* filter, C_CSPlayerPawn* Skip, int64_t Mask, uint8_t Layer1, uint8_t Layer2);

	uint64_t DamageToPoint(TraceData_t* trace_data, float damage, float penetration, float range_modifier, int team_num);

};
CGameTrace traceRay(Vec3 start, Vec3 end, uintptr_t ignore);
bool isVisibleBone(int boneIndex, uintptr_t localPawn, uintptr_t target);