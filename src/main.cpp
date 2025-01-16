#include "MAPLE.h"

int main(int argc, char *argv[])
{
    MAPLE &m_maple = MAPLE::GetInstance();

    m_maple.Setup("../config.yml");

    m_maple.Start();

    // Get the estimated robot pose
    // m_maple.GetRobotPose();

    m_maple.Join();

    return 0;
}
