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

	inline void SetRect(void* _this, RECT* value, pfnRectSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetRect(void* _this, RECT* value, pfnRectGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetPoint(void* _this, POINT* value, pfnPointSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetPoint(void* _this, POINT* value, pfnPointGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetSize(void* _this, SIZE* value, pfnSizeSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline void GetSize(void* _this, SIZE* value, pfnSizeGetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}

	inline void SetString(void* _this, const wchar_t* value, pfnStringSetter func) {
		x64asm_Set64(_this, (INT_PTR)value, func);
	}
	inline const wchar_t* GetString(void* _this, pfnStringGetter func) {
		return (const wchar_t*)x64asm_Get64(_this, func);
	}

	typedef void(__thiscall *pfnStringSetter)(const wchar_t*);
	typedef const wchar_t*(__thiscall *pfnStringGetter)();
	typedef void(__thiscall *pfnStringExSetter)(const wchar_t*, int);
}
#endif

#endif // _ATTRIBUTE_X64_H_