#pragma once

namespace Frostium
{
	enum Flags
	{
		//Features
		Features_Renderer_2D_Flags   = 1,
		Features_Renderer_3D_Flags   = 2,
		Features_ImGui_Flags         = 4,
		Features_HDR_Flags           = 8
	};

	inline Flags operator~ (Flags a) { return (Flags)~(int)a; }

	inline Flags operator| (Flags a, Flags b) { return (Flags)((int)a | (int)b); }
	inline Flags operator& (Flags a, Flags b) { return (Flags)((int)a & (int)b); }
	inline Flags operator^ (Flags a, Flags b) { return (Flags)((int)a ^ (int)b); }

	inline Flags& operator|= (Flags& a, Flags b) { return (Flags&)((int&)a |= (int)b); }
	inline Flags& operator&= (Flags& a, Flags b) { return (Flags&)((int&)a &= (int)b); }
	inline Flags& operator^= (Flags& a, Flags b) { return (Flags&)((int&)a ^= (int)b); }
}