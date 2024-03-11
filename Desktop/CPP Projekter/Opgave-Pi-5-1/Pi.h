#ifndef PI_H
#define PI_H

class Pi
{
public:
    double calcPi();
    double calcPi(int n);
    void setGentagelser(int gentagelser);

private:
    double piResult;
    int n;
    int gentagelser;
};

#endif // PI_H
