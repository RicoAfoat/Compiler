#pragma once
#include "LegalizeConstInt.hpp"

LegalizeConstInt::LegalizeConstInt(RISCVLoweringContext& _ctx)
    :ctx(_ctx) {}
void LegalizeConstInt::LegConstInt(RISCVMIR* inst, Imm* constdata,mylist<RISCVBasicBlock,RISCVMIR>::iterator it) {
    if(inst->GetOpcode()==RISCVMIR::RISCVISA::ret) return;
    else if(inst->GetOpcode()==RISCVMIR::RISCVISA::call) return;
    
    int inttemp = dynamic_cast<ConstIRInt*>(constdata->Getdata())->GetVal();
    if(inttemp>=-2048 && inttemp<2048) {
        if(inst->GetOpcode()==RISCVMIR::RISCVISA::mv) inst->SetMopcode(RISCVMIR::RISCVISA::li); 
        return;
    }
    else {
        int mod = inttemp % 4096;
        if(mod==0) {
            if(inst->GetOpcode() == RISCVMIR::RISCVISA::li) {
                return;
            }
            else if(inst->GetOpcode() == RISCVMIR::RISCVISA::_addi ||\
                    inst->GetOpcode() == RISCVMIR::RISCVISA::_addiw) {
                RISCVMIR* li = new RISCVMIR(RISCVMIR::RISCVISA::li);
                VirRegister* vreg = new VirRegister(RISCVType::riscv_i32);
                li->SetDef(vreg);
                li->AddOperand(constdata);
                it.insert_before(li);
                for(int i=0; i<inst->GetOperandSize(); i++) {
                    if(Imm* imm = dynamic_cast<Imm*>(inst->GetOperand(i))) {
                        if(imm->Getdata()==constdata->Getdata()) {
                            inst->SetOperand(i,vreg);
                            break;
                        }
                    } 
                }
            }
            else {
                inst->SetMopcode(RISCVMIR::RISCVISA::li);
            }
        } else if((mod>0&&mod<2048)||(mod>=-2048&&mod<0)) {
            VirRegister* vreg = ctx.createVReg(RISCVType::riscv_i32);
            Imm* const_imm = new Imm(ConstIRInt::GetNewConstant(inttemp-mod));
            RISCVMIR* li = new RISCVMIR(RISCVMIR::RISCVISA::li);
            li->SetDef(vreg);
            li->AddOperand(const_imm);
            it.insert_before(li);
            Imm* mod_imm = new Imm(ConstIRInt::GetNewConstant(mod));
            RISCVMIR* addi = new RISCVMIR(RISCVMIR::RISCVISA::_addiw);
            addi->SetDef(vreg);
            addi->AddOperand(mod_imm);
            it.insert_before(addi);
            for(int i=0; i<inst->GetOperandSize(); i++) {
                while(inst->GetOperand(i)==constdata) {
                    inst->SetOperand(i,vreg);
                    return;
                }
            }
        } else if (mod >=2048 && mod <4095) {
            VirRegister* vreg = ctx.createVReg(RISCVType::riscv_i32);
            Imm* const_imm = new Imm(ConstIRInt::GetNewConstant(inttemp-mod-4096));
            RISCVMIR* li = new RISCVMIR(RISCVMIR::RISCVISA::li);
            li->SetDef(vreg);
            li->AddOperand(const_imm);
            it.insert_before(li);
            Imm* mod_imm = new Imm(ConstIRInt::GetNewConstant(mod-4096));
            RISCVMIR* addi = new RISCVMIR(RISCVMIR::RISCVISA::_addiw);
            addi->SetDef(vreg);
            addi->AddOperand(mod_imm);
            it.insert_before(addi);
            for(int i=0; i<inst->GetOperandSize(); i++) {
                while(inst->GetOperand(i)==constdata) {
                    inst->SetOperand(i,vreg);
                    return;
                }
            }
        } else if (mod>-4095&&mod<-2048) {
            VirRegister* vreg = ctx.createVReg(RISCVType::riscv_i32);
            Imm* const_imm = new Imm(ConstIRInt::GetNewConstant(inttemp-mod-4096));
            RISCVMIR* li = new RISCVMIR(RISCVMIR::RISCVISA::li);
            li->SetDef(vreg);
            li->AddOperand(const_imm);
            it.insert_before(li);
            Imm* mod_imm = new Imm(ConstIRInt::GetNewConstant(mod+4096));
            RISCVMIR* addi = new RISCVMIR(RISCVMIR::RISCVISA::_addiw);
            addi->SetDef(vreg);
            addi->AddOperand(mod_imm);
            it.insert_before(addi);
            for(int i=0; i<inst->GetOperandSize(); i++) {
                while(inst->GetOperand(i)==constdata) {
                    inst->SetOperand(i,vreg);
                    return;
                }
            }
        } else assert(0&&"error imm");
    }
    return;
}

bool LegalizeConstInt::run() {
    for(auto& func : ctx.GetFunctions()) {
        for(auto block : *func) {
            for(mylist<RISCVBasicBlock,RISCVMIR>::iterator it=block->begin();it!=block->end();++it) {
                RISCVMIR* inst = *it;
                for(int i=0; i<inst->GetOperandSize(); i++) {
                    if(Imm* constdata = dynamic_cast<Imm*>(inst->GetOperand(i))) {
                        if(ConstIRInt* constint = dynamic_cast<ConstIRInt*>(constdata->Getdata())) {
                            LegConstInt(inst, constdata, it);
                        }
                    }
                }
            }
        }
    }
    return false;
}

