#ifndef HEIGHT_CHANGE
#define HEIGHT_CHANGE

#include "timestamped.h"

class HeightChange : public Timestamped
{
public:
    HeightChange() {}
    HeightChange(double z);

    double z();

    bool processed();
    void processed(bool p);
private:
    double _z;
    bool _processed;
};

#endif