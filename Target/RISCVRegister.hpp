#pragma once
#include "RISCVMOperand.hpp"

/// @brief 这个写成接口吧...
class Register:public RISCVMOperand{
    public:
    Register(RISCVType _tp):RISCVMOperand(_tp){};
    virtual bool isPhysical()=0;
};

class PhyRegister:public Register{
    public:
    enum PhyReg{
        x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,
        x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,
        f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,
        f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,f30,f31
    }regenum;
    PhyRegister(PhyReg);
    static PhyRegister* GetPhyReg(PhyReg);
    void print()final;
    bool isPhysical()final{return true;};
};

class VirRegister:public Register{
    int counter;
    public:
    VirRegister(RISCVType);
    void print()final;
    bool isPhysical()final{return false;};
};