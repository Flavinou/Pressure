#pragma once

namespace Pressure
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

}

// From glfw3.h
#define PRS_KEY_SPACE              ::Pressure::Key::Space
#define PRS_KEY_APOSTROPHE         ::Pressure::Key::Apostrophe  /* ' */
#define PRS_KEY_COMMA              ::Pressure::Key::Comma  /* , */
#define PRS_KEY_MINUS              ::Pressure::Key::Minus  /* - */
#define PRS_KEY_PERIOD             ::Pressure::Key::Period  /* . */
#define PRS_KEY_SLASH              ::Pressure::Key::Slash  /* / */
#define PRS_KEY_0                  ::Pressure::Key::D0
#define PRS_KEY_1                  ::Pressure::Key::D1
#define PRS_KEY_2                  ::Pressure::Key::D2
#define PRS_KEY_3                  ::Pressure::Key::D3
#define PRS_KEY_4                  ::Pressure::Key::D4
#define PRS_KEY_5                  ::Pressure::Key::D5
#define PRS_KEY_6                  ::Pressure::Key::D6
#define PRS_KEY_7                  ::Pressure::Key::D7
#define PRS_KEY_8                  ::Pressure::Key::D8
#define PRS_KEY_9                  ::Pressure::Key::D9
#define PRS_KEY_SEMICOLON          ::Pressure::Key::Semicolon  /* ; */
#define PRS_KEY_EQUAL              ::Pressure::Key::Equal  /* = */
#define PRS_KEY_A                  ::Pressure::Key::A
#define PRS_KEY_B                  ::Pressure::Key::B
#define PRS_KEY_C                  ::Pressure::Key::C
#define PRS_KEY_D                  ::Pressure::Key::D
#define PRS_KEY_E                  ::Pressure::Key::E
#define PRS_KEY_F                  ::Pressure::Key::F
#define PRS_KEY_G                  ::Pressure::Key::G
#define PRS_KEY_H                  ::Pressure::Key::H
#define PRS_KEY_I                  ::Pressure::Key::I
#define PRS_KEY_J                  ::Pressure::Key::J
#define PRS_KEY_K                  ::Pressure::Key::K
#define PRS_KEY_L                  ::Pressure::Key::L
#define PRS_KEY_M                  ::Pressure::Key::M
#define PRS_KEY_N                  ::Pressure::Key::N
#define PRS_KEY_O                  ::Pressure::Key::O
#define PRS_KEY_P                  ::Pressure::Key::P
#define PRS_KEY_Q                  ::Pressure::Key::Q
#define PRS_KEY_R                  ::Pressure::Key::R
#define PRS_KEY_S                  ::Pressure::Key::S
#define PRS_KEY_T                  ::Pressure::Key::T
#define PRS_KEY_U                  ::Pressure::Key::U
#define PRS_KEY_V                  ::Pressure::Key::V
#define PRS_KEY_W                  ::Pressure::Key::W
#define PRS_KEY_X                  ::Pressure::Key::X
#define PRS_KEY_Y                  ::Pressure::Key::Y
#define PRS_KEY_Z                  ::Pressure::Key::Z
#define PRS_KEY_LEFT_BRACKET       ::Pressure::Key::LeftBracket  /* [ */
#define PRS_KEY_BACKSLASH          ::Pressure::Key::Backslash  /* \ */
#define PRS_KEY_RIGHT_BRACKET      ::Pressure::Key::RightBracket  /* ] */
#define PRS_KEY_GRAVE_ACCENT       ::Pressure::Key::GraveAccent  /* ` */
#define PRS_KEY_WORLD_1            ::Pressure::Key::World1 /* non-US #1 */
#define PRS_KEY_WORLD_2            ::Pressure::Key::World2 /* non-US #2 */

/* Function keys */
#define PRS_KEY_ESCAPE             ::Pressure::Key::Escape
#define PRS_KEY_ENTER              ::Pressure::Key::Enter
#define PRS_KEY_TAB                ::Pressure::Key::Tab
#define PRS_KEY_BACKSPACE          ::Pressure::Key::Backspace
#define PRS_KEY_INSERT             ::Pressure::Key::Insert
#define PRS_KEY_DELETE             ::Pressure::Key::Delete
#define PRS_KEY_RIGHT              ::Pressure::Key::Right
#define PRS_KEY_LEFT               ::Pressure::Key::Left
#define PRS_KEY_DOWN               ::Pressure::Key::Down
#define PRS_KEY_UP                 ::Pressure::Key::Up
#define PRS_KEY_PAGE_UP            ::Pressure::Key::PageUp
#define PRS_KEY_PAGE_DOWN          ::Pressure::Key::PageDown
#define PRS_KEY_HOME               ::Pressure::Key::Home
#define PRS_KEY_END                ::Pressure::Key::End
#define PRS_KEY_CAPS_LOCK          ::Pressure::Key::CapsLock
#define PRS_KEY_SCROLL_LOCK        ::Pressure::Key::ScrollLock
#define PRS_KEY_NUM_LOCK           ::Pressure::Key::NumLock
#define PRS_KEY_PRINT_SCREEN       ::Pressure::Key::PrintScreen
#define PRS_KEY_PAUSE              ::Pressure::Key::Pause
#define PRS_KEY_F1                 ::Pressure::Key::F1
#define PRS_KEY_F2                 ::Pressure::Key::F2
#define PRS_KEY_F3                 ::Pressure::Key::F3
#define PRS_KEY_F4                 ::Pressure::Key::F4
#define PRS_KEY_F5                 ::Pressure::Key::F5
#define PRS_KEY_F6                 ::Pressure::Key::F6
#define PRS_KEY_F7                 ::Pressure::Key::F7
#define PRS_KEY_F8                 ::Pressure::Key::F8
#define PRS_KEY_F9                 ::Pressure::Key::F9
#define PRS_KEY_F10                ::Pressure::Key::F10
#define PRS_KEY_F11                ::Pressure::Key::F11
#define PRS_KEY_F12                ::Pressure::Key::F12
#define PRS_KEY_F13                ::Pressure::Key::F13
#define PRS_KEY_F14                ::Pressure::Key::F14
#define PRS_KEY_F15                ::Pressure::Key::F15
#define PRS_KEY_F16                ::Pressure::Key::F16
#define PRS_KEY_F17                ::Pressure::Key::F17
#define PRS_KEY_F18                ::Pressure::Key::F18
#define PRS_KEY_F19                ::Pressure::Key::F19
#define PRS_KEY_F20                ::Pressure::Key::F20
#define PRS_KEY_F21                ::Pressure::Key::F21
#define PRS_KEY_F22                ::Pressure::Key::F22
#define PRS_KEY_F23                ::Pressure::Key::F23
#define PRS_KEY_F24                ::Pressure::Key::F24
#define PRS_KEY_F25                ::Pressure::Key::F25

/* Keypad */
#define PRS_KEY_KP_0               ::Pressure::Key::KP0
#define PRS_KEY_KP_1               ::Pressure::Key::KP1
#define PRS_KEY_KP_2               ::Pressure::Key::KP2
#define PRS_KEY_KP_3               ::Pressure::Key::KP3
#define PRS_KEY_KP_4               ::Pressure::Key::KP4
#define PRS_KEY_KP_5               ::Pressure::Key::KP5
#define PRS_KEY_KP_6               ::Pressure::Key::KP6
#define PRS_KEY_KP_7               ::Pressure::Key::KP7
#define PRS_KEY_KP_8               ::Pressure::Key::KP8
#define PRS_KEY_KP_9               ::Pressure::Key::KP9
#define PRS_KEY_KP_DECIMAL         ::Pressure::Key::KPDecimal
#define PRS_KEY_KP_DIVIDE          ::Pressure::Key::KPDivide
#define PRS_KEY_KP_MULTIPLY        ::Pressure::Key::KPMultiply
#define PRS_KEY_KP_SUBTRACT        ::Pressure::Key::KPSubtract
#define PRS_KEY_KP_ADD             ::Pressure::Key::KPAdd
#define PRS_KEY_KP_ENTER           ::Pressure::Key::KPEnter
#define PRS_KEY_KP_EQUAL           ::Pressure::Key::KPEqual

#define PRS_KEY_LEFT_SHIFT         ::Pressure::Key::LeftShift
#define PRS_KEY_LEFT_CONTROL       ::Pressure::Key::LeftControl
#define PRS_KEY_LEFT_ALT           ::Pressure::Key::LeftAlt
#define PRS_KEY_LEFT_SUPER         ::Pressure::Key::LeftSuper
#define PRS_KEY_RIGHT_SHIFT        ::Pressure::Key::RightShift
#define PRS_KEY_RIGHT_CONTROL      ::Pressure::Key::RightControl
#define PRS_KEY_RIGHT_ALT          ::Pressure::Key::RightAlt
#define PRS_KEY_RIGHT_SUPER        ::Pressure::Key::RightSuper
#define PRS_KEY_MENU               ::Pressure::Key::Menu