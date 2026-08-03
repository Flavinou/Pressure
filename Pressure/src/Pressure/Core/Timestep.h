#pragma once

namespace Pressure
{

    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : m_Time(time)
        {
        }

        operator float() const { return m_Time; }

        float GetSeconds() const { return m_Time; }
        float GetMilliseconds() const { return m_Time * 1000.0f; }
		Timestep& operator*=(float scalar) { m_Time *= scalar; return *this; }

    private:
        float m_Time;
    };

}
