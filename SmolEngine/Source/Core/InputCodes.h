#pragma once

#include <iostream>

namespace SmolEngine
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
#define S_KEY_SPACE           ::SmolEngine::Key::Space
#define S_KEY_APOSTROPHE      ::SmolEngine::Key::Apostrophe    /* ' */
#define S_KEY_COMMA           ::SmolEngine::Key::Comma         /* , */
#define S_KEY_MINUS           ::SmolEngine::Key::Minus         /* - */
#define S_KEY_PERIOD          ::SmolEngine::Key::Period        /* . */
#define S_KEY_SLASH           ::SmolEngine::Key::Slash         /* / */
#define S_KEY_0               ::SmolEngine::Key::D0
#define S_KEY_1               ::SmolEngine::Key::D1
#define S_KEY_2               ::SmolEngine::Key::D2
#define S_KEY_3               ::SmolEngine::Key::D3
#define S_KEY_4               ::SmolEngine::Key::D4
#define S_KEY_5               ::SmolEngine::Key::D5
#define S_KEY_6               ::SmolEngine::Key::D6
#define S_KEY_7               ::SmolEngine::Key::D7
#define S_KEY_8               ::SmolEngine::Key::D8
#define S_KEY_9               ::SmolEngine::Key::D9
#define S_KEY_SEMICOLON       ::SmolEngine::Key::Semicolon     /* ; */
#define S_KEY_EQUAL           ::SmolEngine::Key::Equal         /* = */
#define S_KEY_A               ::SmolEngine::Key::A
#define S_KEY_B               ::SmolEngine::Key::B
#define S_KEY_C               ::SmolEngine::Key::C
#define S_KEY_D               ::SmolEngine::Key::D
#define S_KEY_E               ::SmolEngine::Key::E
#define S_KEY_F               ::SmolEngine::Key::F
#define S_KEY_G               ::SmolEngine::Key::G
#define S_KEY_H               ::SmolEngine::Key::H
#define S_KEY_I               ::SmolEngine::Key::I
#define S_KEY_J               ::SmolEngine::Key::J
#define S_KEY_K               ::SmolEngine::Key::K
#define S_KEY_L               ::SmolEngine::Key::L
#define S_KEY_M               ::SmolEngine::Key::M
#define S_KEY_N               ::SmolEngine::Key::N
#define S_KEY_O               ::SmolEngine::Key::O
#define S_KEY_P               ::SmolEngine::Key::P
#define S_KEY_Q               ::SmolEngine::Key::Q
#define S_KEY_R               ::SmolEngine::Key::R
#define S_KEY_S               ::SmolEngine::Key::S
#define S_KEY_T               ::SmolEngine::Key::T
#define S_KEY_U               ::SmolEngine::Key::U
#define S_KEY_V               ::SmolEngine::Key::V
#define S_KEY_W               ::SmolEngine::Key::W
#define S_KEY_X               ::SmolEngine::Key::X
#define S_KEY_Y               ::SmolEngine::Key::Y
#define S_KEY_Z               ::SmolEngine::Key::Z
#define S_KEY_LEFT_BRACKET    ::SmolEngine::Key::LeftBracket   /* [ */
#define S_KEY_BACKSLASH       ::SmolEngine::Key::Backslash     /* \ */
#define S_KEY_RIGHT_BRACKET   ::SmolEngine::Key::RightBracket  /* ] */
#define S_KEY_GRAVE_ACCENT    ::SmolEngine::Key::GraveAccent   /* ` */
#define S_KEY_WORLD_1         ::SmolEngine::Key::World1        /* non-US #1 */
#define S_KEY_WORLD_2         ::SmolEngine::Key::World2        /* non-US #2 */

/* FunctSn keys */				
#define S_KEY_ESCAPE          ::SmolEngine::Key::Escape
#define S_KEY_ENTER           ::SmolEngine::Key::Enter
#define S_KEY_TAB             ::SmolEngine::Key::Tab
#define S_KEY_BACKSPACE       ::SmolEngine::Key::Backspace
#define S_KEY_INSERT          ::SmolEngine::Key::Insert
#define S_KEY_DELETE          ::SmolEngine::Key::Delete
#define S_KEY_RIGHT           ::SmolEngine::Key::Right
#define S_KEY_LEFT            ::SmolEngine::Key::Left
#define S_KEY_DOWN            ::SmolEngine::Key::Down
#define S_KEY_UP              ::SmolEngine::Key::Up
#define S_KEY_PAGE_UP         ::SmolEngine::Key::PageUp
#define S_KEY_PAGE_DOWN       ::SmolEngine::Key::PageDown
#define S_KEY_HOME            ::SmolEngine::Key::Home
#define S_KEY_END             ::SmolEngine::Key::End
#define S_KEY_CAPS_LOCK       ::SmolEngine::Key::CapsLock
#define S_KEY_SCROLL_LOCK     ::SmolEngine::Key::ScrollLock
#define S_KEY_NUM_LOCK        ::SmolEngine::Key::NumLock
#define S_KEY_PRINT_SCREEN    ::SmolEngine::Key::PrintScreen
#define S_KEY_PAUSE           ::SmolEngine::Key::Pause
#define S_KEY_F1              ::SmolEngine::Key::F1
#define S_KEY_F2              ::SmolEngine::Key::F2
#define S_KEY_F3              ::SmolEngine::Key::F3
#define S_KEY_F4              ::SmolEngine::Key::F4
#define S_KEY_F5              ::SmolEngine::Key::F5
#define S_KEY_F6              ::SmolEngine::Key::F6
#define S_KEY_F7              ::SmolEngine::Key::F7
#define S_KEY_F8              ::SmolEngine::Key::F8
#define S_KEY_F9              ::SmolEngine::Key::F9
#define S_KEY_F10             ::SmolEngine::Key::F10
#define S_KEY_F11             ::SmolEngine::Key::F11
#define S_KEY_F12             ::SmolEngine::Key::F12
#define S_KEY_F13             ::SmolEngine::Key::F13
#define S_KEY_F14             ::SmolEngine::Key::F14
#define S_KEY_F15             ::SmolEngine::Key::F15
#define S_KEY_F16             ::SmolEngine::Key::F16
#define S_KEY_F17             ::SmolEngine::Key::F17
#define S_KEY_F18             ::SmolEngine::Key::F18
#define S_KEY_F19             ::SmolEngine::Key::F19
#define S_KEY_F20             ::SmolEngine::Key::F20
#define S_KEY_F21             ::SmolEngine::Key::F21
#define S_KEY_F22             ::SmolEngine::Key::F22
#define S_KEY_F23             ::SmolEngine::Key::F23
#define S_KEY_F24             ::SmolEngine::Key::F24
#define S_KEY_F25             ::SmolEngine::Key::F25
/* KeypaS*/					
#define S_KEY_KP_0            ::SmolEngine::Key::KP0
#define S_KEY_KP_1            ::SmolEngine::Key::KP1
#define S_KEY_KP_2            ::SmolEngine::Key::KP2
#define S_KEY_KP_3            ::SmolEngine::Key::KP3
#define S_KEY_KP_4            ::SmolEngine::Key::KP4
#define S_KEY_KP_5            ::SmolEngine::Key::KP5
#define S_KEY_KP_6            ::SmolEngine::Key::KP6
#define S_KEY_KP_7            ::SmolEngine::Key::KP7
#define S_KEY_KP_8            ::SmolEngine::Key::KP8
#define S_KEY_KP_9            ::SmolEngine::Key::KP9
#define S_KEY_KP_DECIMAL      ::SmolEngine::Key::KPDecimal
#define S_KEY_KP_DIVIDE       ::SmolEngine::Key::KPDivide
#define S_KEY_KP_MULTIPLY     ::SmolEngine::Key::KPMultiply
#define S_KEY_KP_SUBTRACT     ::SmolEngine::Key::KPSubtract
#define S_KEY_KP_ADD          ::SmolEngine::Key::KPAdd
#define S_KEY_KP_ENTER        ::SmolEngine::Key::KPEnter
#define S_KEY_KP_EQUAL        ::SmolEngine::Key::KPEqual

#define S_KEY_LEFT_SHIFT      ::SmolEngine::Key::LeftShift
#define S_KEY_LEFT_CONTROL    ::SmolEngine::Key::LeftControl
#define S_KEY_LEFT_ALT        ::SmolEngine::Key::LeftAlt
#define S_KEY_LEFT_SUPER      ::SmolEngine::Key::LeftSuper
#define S_KEY_RIGHT_SHIFT     ::SmolEngine::Key::RightShift
#define S_KEY_RIGHT_CONTROL   ::SmolEngine::Key::RightControl
#define S_KEY_RIGHT_ALT       ::SmolEngine::Key::RightAlt
#define S_KEY_RIGHT_SUPER     ::SmolEngine::Key::RightSuper
#define S_KEY_MENU            ::SmolEngine::Key::Menu

#define S_MOUSE_BUTTON_0      ::SmolEngine::Mouse::Button0
#define S_MOUSE_BUTTON_1      ::SmolEngine::Mouse::Button1
#define S_MOUSE_BUTTON_2      ::SmolEngine::Mouse::Button2
#define S_MOUSE_BUTTON_3      ::SmolEngine::Mouse::Button3
#define S_MOUSE_BUTTON_4      ::SmolEngine::Mouse::Button4
#define S_MOUSE_BUTTON_5      ::SmolEngine::Mouse::Button5
#define S_MOUSE_BUTTON_6      ::SmolEngine::Mouse::Button6
#define S_MOUSE_BUTTON_7      ::SmolEngine::Mouse::Button7
#define S_MOUSE_BUTTON_LAST   ::SmolEngine::Mouse::ButtonLast
#define S_MOUSE_BUTTON_LEFT   ::SmolEngine::Mouse::ButtonLeft
#define S_MOUSE_BUTTON_RIGHT  ::SmolEngine::Mouse::ButtonRight
#define S_MOUSE_BUTTON_MIDDLE ::SmolEngine::Mouse::ButtonMiddle