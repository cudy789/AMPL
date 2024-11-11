#include "AMPL.h"

int main(int argc, char *argv[])
{
    AMPL &m_ample = AMPL::GetInstance();

    m_ample.Setup("../config.yml", "../at14_6.fmap");

    m_ample.Start();

    m_ample.Join();

    return 0;
}
