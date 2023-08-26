#ifndef _ATTRIBUTE_X64_H_
#define _ATTRIBUTE_X64_H_
#include "include/interface/iattribute.h"

#ifdef _WIN64
extern "C"
{
	void __stdcall x64asm_Set64(void* _this, INT_PTR value, void* func);
	INT64 __stdcall x64asm_Get64(void* _this, void* func);
}

namespace UIx64
{
	inline void SetLong(void* _this, long value, pfnLongSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline long GetLong(void* _this, pfnLongGetter func) {
		return static_cast<long>(x64asm_Get64(_this, func));
	}

	inline void SetBool(void* _this, bool value, pfnBoolSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline bool GetBool(void* _this, pfnBoolGetter func) {
		return x64asm_Get64(_this, func) ? true:false;
	}

	inline void Set9Region(void* _this, C9Region* value, pfn9RegionSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void Get9Region(void* _this, C9Region* value, pfn9RegionGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetRect(void* _this, Rect* value, pfnRectSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetRect(void* _this, Rect* value, pfnRectGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetPoint(void* _this, Point* value, pfnPointSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetPoint(void* _this, Point* value, pfnPointGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetSize(void* _this, Size* value, pfnSizeSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetSize(void* _this, Size* value, pfnSizeGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetString(void* _this, const char* value, pfnStringSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline const char* GetString(void* _this, pfnStringGetter func) {
		return (const char*)x64asm_Get64(_this, func);
	}

	typedef void(__thiscall *pfnStringSetter)(const char*);
	typedef const char*(__thiscall *pfnStringGetter)();
	typedef void(__thiscall *pfnStringExSetter)(const char*, int);
}
#endif

#endif // _ATTRIBUTE_X64_H_