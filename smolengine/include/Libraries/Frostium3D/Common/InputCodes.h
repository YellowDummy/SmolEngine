#pragma once

#include <iostream>

namespace Frostium
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}

	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

// From glfw3.h
#define F_KEY_SPACE           ::Frostium::Key::Space
#define F_KEY_APOSTROPHE      ::Frostium::Key::Apostrophe    /* ' */
#define F_KEY_COMMA           ::Frostium::Key::Comma         /* , */
#define F_KEY_MINUS           ::Frostium::Key::Minus         /* - */
#define F_KEY_PERIOD          ::Frostium::Key::Period        /* . */
#define F_KEY_SLASH           ::Frostium::Key::Slash         /* / */
#define F_KEY_0               ::Frostium::Key::D0
#define F_KEY_1               ::Frostium::Key::D1
#define F_KEY_2               ::Frostium::Key::D2
#define F_KEY_3               ::Frostium::Key::D3
#define F_KEY_4               ::Frostium::Key::D4
#define F_KEY_5               ::Frostium::Key::D5
#define F_KEY_6               ::Frostium::Key::D6
#define F_KEY_7               ::Frostium::Key::D7
#define F_KEY_8               ::Frostium::Key::D8
#define F_KEY_9               ::Frostium::Key::D9
#define F_KEY_SEMICOLON       ::Frostium::Key::Semicolon     /* ; */
#define F_KEY_EQUAL           ::Frostium::Key::Equal         /* = */
#define F_KEY_A               ::Frostium::Key::A
#define F_KEY_B               ::Frostium::Key::B
#define F_KEY_C               ::Frostium::Key::C
#define F_KEY_D               ::Frostium::Key::D
#define F_KEY_E               ::Frostium::Key::E
#define F_KEY_F               ::Frostium::Key::F
#define F_KEY_G               ::Frostium::Key::G
#define F_KEY_H               ::Frostium::Key::H
#define F_KEY_I               ::Frostium::Key::I
#define F_KEY_J               ::Frostium::Key::J
#define F_KEY_K               ::Frostium::Key::K
#define F_KEY_L               ::Frostium::Key::L
#define F_KEY_M               ::Frostium::Key::M
#define F_KEY_N               ::Frostium::Key::N
#define F_KEY_O               ::Frostium::Key::O
#define F_KEY_P               ::Frostium::Key::P
#define F_KEY_Q               ::Frostium::Key::Q
#define F_KEY_R               ::Frostium::Key::R
#define F_KEY_S               ::Frostium::Key::S
#define F_KEY_T               ::Frostium::Key::T
#define F_KEY_U               ::Frostium::Key::U
#define F_KEY_V               ::Frostium::Key::V
#define F_KEY_W               ::Frostium::Key::W
#define F_KEY_X               ::Frostium::Key::X
#define F_KEY_Y               ::Frostium::Key::Y
#define F_KEY_Z               ::Frostium::Key::Z
#define F_KEY_LEFT_BRACKET    ::Frostium::Key::LeftBracket   /* [ */
#define F_KEY_BACKSLASH       ::Frostium::Key::Backslash     /* \ */
#define F_KEY_RIGHT_BRACKET   ::Frostium::Key::RightBracket  /* ] */
#define F_KEY_GRAVE_ACCENT    ::Frostium::Key::GraveAccent   /* ` */
#define F_KEY_WORLD_1         ::Frostium::Key::World1        /* non-US #1 */
#define F_KEY_WORLD_2         ::Frostium::Key::World2        /* non-US #2 */

/* FunctFn keys */				
#define F_KEY_ESCAPE          ::Frostium::Key::Escape
#define F_KEY_ENTER           ::Frostium::Key::Enter
#define F_KEY_TAB             ::Frostium::Key::Tab
#define F_KEY_BACKSPACE       ::Frostium::Key::Backspace
#define F_KEY_INSERT          ::Frostium::Key::Insert
#define F_KEY_DELETE          ::Frostium::Key::Delete
#define F_KEY_RIGHT           ::Frostium::Key::Right
#define F_KEY_LEFT            ::Frostium::Key::Left
#define F_KEY_DOWN            ::Frostium::Key::Down
#define F_KEY_UP              ::Frostium::Key::Up
#define F_KEY_PAGE_UP         ::Frostium::Key::PageUp
#define F_KEY_PAGE_DOWN       ::Frostium::Key::PageDown
#define F_KEY_HOME            ::Frostium::Key::Home
#define F_KEY_END             ::Frostium::Key::End
#define F_KEY_CAPS_LOCK       ::Frostium::Key::CapsLock
#define F_KEY_SCROLL_LOCK     ::Frostium::Key::ScrollLock
#define F_KEY_NUM_LOCK        ::Frostium::Key::NumLock
#define F_KEY_PRINT_SCREEN    ::Frostium::Key::PrintScreen
#define F_KEY_PAUSE           ::Frostium::Key::Pause
#define F_KEY_F1              ::Frostium::Key::F1
#define F_KEY_F2              ::Frostium::Key::F2
#define F_KEY_F3              ::Frostium::Key::F3
#define F_KEY_F4              ::Frostium::Key::F4
#define F_KEY_F5              ::Frostium::Key::F5
#define F_KEY_F6              ::Frostium::Key::F6
#define F_KEY_F7              ::Frostium::Key::F7
#define F_KEY_F8              ::Frostium::Key::F8
#define F_KEY_F9              ::Frostium::Key::F9
#define F_KEY_F10             ::Frostium::Key::F10
#define F_KEY_F11             ::Frostium::Key::F11
#define F_KEY_F12             ::Frostium::Key::F12
#define F_KEY_F13             ::Frostium::Key::F13
#define F_KEY_F14             ::Frostium::Key::F14
#define F_KEY_F15             ::Frostium::Key::F15
#define F_KEY_F16             ::Frostium::Key::F16
#define F_KEY_F17             ::Frostium::Key::F17
#define F_KEY_F18             ::Frostium::Key::F18
#define F_KEY_F19             ::Frostium::Key::F19
#define F_KEY_F20             ::Frostium::Key::F20
#define F_KEY_F21             ::Frostium::Key::F21
#define F_KEY_F22             ::Frostium::Key::F22
#define F_KEY_F23             ::Frostium::Key::F23
#define F_KEY_F24             ::Frostium::Key::F24
#define F_KEY_F25             ::Frostium::Key::F25
/* Keypa*/					
#define F_KEY_KP_0            ::Frostium::Key::KP0
#define F_KEY_KP_1            ::Frostium::Key::KP1
#define F_KEY_KP_2            ::Frostium::Key::KP2
#define F_KEY_KP_3            ::Frostium::Key::KP3
#define F_KEY_KP_4            ::Frostium::Key::KP4
#define F_KEY_KP_5            ::Frostium::Key::KP5
#define F_KEY_KP_6            ::Frostium::Key::KP6
#define F_KEY_KP_7            ::Frostium::Key::KP7
#define F_KEY_KP_8            ::Frostium::Key::KP8
#define F_KEY_KP_9            ::Frostium::Key::KP9
#define F_KEY_KP_DECIMAL      ::Frostium::Key::KPDecimal
#define F_KEY_KP_DIVIDE       ::Frostium::Key::KPDivide
#define F_KEY_KP_MULTIPLY     ::Frostium::Key::KPMultiply
#define F_KEY_KP_SUBTRACT     ::Frostium::Key::KPSubtract
#define F_KEY_KP_ADD          ::Frostium::Key::KPAdd
#define F_KEY_KP_ENTER        ::Frostium::Key::KPEnter
#define F_KEY_KP_EQUAL        ::Frostium::Key::KPEqual

#define F_KEY_LEFT_SHIFT      ::Frostium::Key::LeftShift
#define F_KEY_LEFT_CONTROL    ::Frostium::Key::LeftControl
#define F_KEY_LEFT_ALT        ::Frostium::Key::LeftAlt
#define F_KEY_LEFT_SUPER      ::Frostium::Key::LeftSuper
#define F_KEY_RIGHT_SHIFT     ::Frostium::Key::RightShift
#define F_KEY_RIGHT_CONTROL   ::Frostium::Key::RightControl
#define F_KEY_RIGHT_ALT       ::Frostium::Key::RightAlt
#define F_KEY_RIGHT_SUPER     ::Frostium::Key::RightSuper
#define F_KEY_MENU            ::Frostium::Key::Menu

#define F_MOUSE_BUTTON_0      ::Frostium::Mouse::Button0
#define F_MOUSE_BUTTON_1      ::Frostium::Mouse::Button1
#define F_MOUSE_BUTTON_2      ::Frostium::Mouse::Button2
#define F_MOUSE_BUTTON_3      ::Frostium::Mouse::Button3
#define F_MOUSE_BUTTON_4      ::Frostium::Mouse::Button4
#define F_MOUSE_BUTTON_5      ::Frostium::Mouse::Button5
#define F_MOUSE_BUTTON_6      ::Frostium::Mouse::Button6
#define F_MOUSE_BUTTON_7      ::Frostium::Mouse::Button7
#define F_MOUSE_BUTTON_LAST   ::Frostium::Mouse::ButtonLast
#define F_MOUSE_BUTTON_LEFT   ::Frostium::Mouse::ButtonLeft
#define F_MOUSE_BUTTON_RIGHT  ::Frostium::Mouse::ButtonRight
#define F_MOUSE_BUTTON_MIDDLE ::Frostium::Mouse::ButtonMiddle