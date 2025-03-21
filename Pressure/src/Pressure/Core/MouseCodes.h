#pragma once

namespace Pressure
{
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
#define PRS_MOUSE_BUTTON_0         ::Pressure::Mouse::Button0
#define PRS_MOUSE_BUTTON_1         ::Pressure::Mouse::Button1
#define PRS_MOUSE_BUTTON_2         ::Pressure::Mouse::Button2
#define PRS_MOUSE_BUTTON_3         ::Pressure::Mouse::Button3
#define PRS_MOUSE_BUTTON_4         ::Pressure::Mouse::Button4
#define PRS_MOUSE_BUTTON_5         ::Pressure::Mouse::Button5
#define PRS_MOUSE_BUTTON_6         ::Pressure::Mouse::Button6
#define PRS_MOUSE_BUTTON_7         ::Pressure::Mouse::Button7
#define PRS_MOUSE_BUTTON_8         ::Pressure::Mouse::Button8
#define PRS_MOUSE_BUTTON_LAST      ::Pressure::Mouse::ButtonLast
#define PRS_MOUSE_BUTTON_LEFT      ::Pressure::Mouse::ButtonLeft
#define PRS_MOUSE_BUTTON_RIGHT     ::Pressure::Mouse::ButtonRight
#define PRS_MOUSE_BUTTON_MIDDLE    ::Pressure::Mouse::ButtonMiddle