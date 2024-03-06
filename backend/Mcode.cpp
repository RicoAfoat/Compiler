#include "Mcode.hpp"

/*MachinInst*/
MachineInst::MachineInst(MachineBasicBlock* mbb,std::string opcode) : mbb(mbb), opcode(opcode) {}
MachineInst::MachineInst(MachineBasicBlock* mbb,std::string opcode, Operand rd) : mbb(mbb), opcode(opcode), rd(rd) {}
MachineInst::MachineInst(MachineBasicBlock* mbb,std::string opcode, Operand rd, Operand rs1) : mbb(mbb), opcode(opcode), rd(rd), rs1(rs1) {}
MachineInst::MachineInst(MachineBasicBlock* mbb,std::string opcode, Operand rd, Operand rs1, Operand rs2) : mbb(mbb), opcode(opcode), rd(rd), rs1(rs1), rs2(rs2) {}
MachineBasicBlock* MachineInst::get_machinebasicblock() {return this->mbb;}
std::string MachineInst::GetOpcode() {return opcode;}
void MachineInst::print() {
    if (opcode == "alloca") {
        return;   
    }
    else if (opcode == "sw") {
        std::cout << "    " << opcode << " " << rd->GetName() << ", -"; 
        std::cout << mbb->get_parent()->get_offset(rs1->GetName()) << "(s0)" << std::endl;
        // std::cout << mbb->get_parent()->get_offset(rs1->GetName()) << "(";
        // std::cout << rs1->GetName() << ")" << std::endl;
        // rs1->SetName("s0");
    }
    else if (opcode == "lw") {
        std::cout << "    " << opcode << " " << rd->GetName() << ", -"; 
        std::cout << mbb->get_parent()->get_offset(rs1->GetName()) << "(s0)" << std::endl;
        // std::cout << mbb->get_parent()->get_offset(rs1->GetName()) << "(";
        // std::cout << rs1->GetName() << ")" << std::endl;
        // rs1->SetName("s0");
    }
    else if (opcode == "fcvt.s.w") {
        std::cout << "    " << opcode << " " << rd->GetName() << ", ";
        std::cout << rs1->GetName() << std::endl;
    }
    else if (opcode == "fcvt.w.s") {
        std::cout << "    " << opcode << " " << rd->GetName() << ", ";
        std::cout << rs1->GetName() << " , rtz" << std::endl;
    }
    else if (opcode == "j") {
        std::cout << "    " << opcode << " ";
        std::cout << rd->GetName() << std::endl;
        std::string temp = this->get_machinebasicblock()->get_block()->GetName();
        this->get_machinebasicblock()->get_block()->SetName(this->get_machinebasicblock()->get_name());
        this->get_machinebasicblock()->set_name(temp);
    }
    else if (opcode == "call" ) {
        std::cout << "    " << opcode << " ";
        std::cout << rd->GetName() << std::endl;
    }
    else if (opcode == "ret") {
        return;
    }
    else if (opcode == "white")
        std::cout << "Error: No Such Instruction." << std::endl;
    else {
        //binary
        std::cout << "    " << opcode << " ";
        std::cout << rd->GetName() << ", " << rs1->GetName() << ", " << rs2->GetName() << std::endl;
    }
}


/*MachineBinaryInst*/
// MachineBinaryInst::MachineBinaryInst(std::string opcode, Operand rd, Operand rs1, Operand rs2)
//     : opcode(opcode), rd(rd), rs1(rs1), rs2(rs2) {
//         // add_use(rd);
//         // add_use(rs1);
//         // add_use(rs2);
//     }
// void MachineBinaryInst::print() {
//     std::cout << opcode << " ";
//     std::cout << rd->GetName() << ", " << rs1->GetName() << ", " << rs2->GetName() << std::endl;
// }

/*MachineCmpInst*/
// MachineCmpInst::MachineCmpInst(std::string opcode, Operand rd, Operand rs1, Operand rs2)
//     : opcode(opcode), rd(rd), rs1(rs1), rs2(rs2) {
//         // add_use(rd);
//         // add_use(rs1);
//         // add_use(rs2);
//     }
// MachineCmpInst::MachineCmpInst(std::string opcode, Operand rd, Operand rs1)
//     : opcode(opcode), rd(rd), rs1(rs1) {
//         // add_use(rd);
//         // add_use(rs1);
//     }

/*MachineBasicBlock*/
MachineBasicBlock::MachineBasicBlock(BasicBlock* block, MachineFunction* parent)
    : block(block), mfuc(parent) {}
void MachineBasicBlock::set_lable(int func_num, int block_num) {
    name = ".LBB" + std::to_string(func_num) + "_" + std::to_string(block_num);
}
std::string MachineBasicBlock::get_name() {return this->name;}
void MachineBasicBlock::set_name(std::string name) {this->name = name;}
BasicBlock* MachineBasicBlock::get_block() {return this->block;}
MachineFunction* MachineBasicBlock::get_parent() {return this->mfuc;}
void MachineBasicBlock::print_block_lable(int func_num, int block_num) {
    set_lable(func_num, block_num);
    std::cout << name << ":" << std::endl;
}

/*MachineFunction*/
MachineFunction::MachineFunction(Function* func) : func(func), offset(0), alloca_num(0), stacksize(0) {}
void MachineFunction::set_offset_map(std::string name, size_t offset) {
    offsetMap.insert(std::pair<std::string, size_t>(name, offset));
}
void MachineFunction::set_alloca_and_num() {
    alloca_num = 0;
    offset = 16;
    for (auto& Block : this->func->GetBasicBlock()) {
        for (auto Inst : *Block) {
            if (auto Tempinst = dynamic_cast<AllocaInst*>(Inst)) {
                if(Tempinst->GetType()->GetTypeEnum() == InnerDataType::IR_Value_VOID) {
                    continue;
                }
                this->offset += dynamic_cast<PointerType*>(Tempinst->GetType())->GetSubType()->get_size();
                set_offset_map(Tempinst->GetName(), offset);
                this->alloca_num += 1;
            }
        }
    }
}
void MachineFunction::set_stacksize() {
    size_t temp = offset % 16;
    stacksize += offset +(16 - temp);
}
size_t MachineFunction::get_offset(std::string name) {
    return offsetMap.find(name)->second;
}
int MachineFunction::get_allocanum() {return this->alloca_num;}
int MachineFunction::get_stacksize() {return this->stacksize;}
void MachineFunction::print_func_name() {
    std::cout << this->func->GetName() << ":" << std::endl;
}
void MachineFunction::print_stack_frame() {
    this->set_alloca_and_num();
    this->set_stacksize();
    std::cout << "    addi sp, sp, -" << this->get_stacksize() << std::endl;
    std::cout << "    sd ra, " << this->get_stacksize() - 8 << "(sp)" << std::endl;
    std::cout << "    sd s0, " << this->get_stacksize() - 16 << "(sp)" << std::endl;
    std::cout << "    addi s0, sp, " << this->get_stacksize() << std::endl;
}
void MachineFunction::print_stack_offset() {
    //To DO : 考虑寄存器个数
    // std::map<size_t, std::string>::iterator item;
    // int tmp = 16;
    // int a = 0;
    // for (item = offsetMap.begin(); item != offsetMap.end(); item++) {
    //     if ((item->first - tmp) == 4) {
    //         std::cout << "    sw a" << a << ", -" << item->first << "(s0)" << std::endl;
    //     }
    //     else if ((item->first - tmp) == 8) {
    //         std::cout << "    sw a" << a << ", -" << item->first << "(s0)" << std::endl;
    //     }
    //     else {
    //         //To Do
    //         //array
    //     }
    //     a++;
    //     if( a < 8 ) {
    //         continue;
    //     }
    //     else {
    //         std::cout << "Error: There are not enough regiters." << std::endl;
    //     }
    //     tmp = item->first;
    // }
}
void MachineFunction::print_func_end() {
    std::cout << "    ld ra, " << this->get_stacksize() - 8 << "(sp)" << std::endl;
    std::cout << "    ld s0, " << this->get_stacksize() - 16 << "(sp)" << std::endl;
    std::cout << "    addi sp, sp, " << this->get_stacksize() << std::endl;
    std::cout << "    ret" << std::endl;
}