#include"InstSelection.hpp"

bool is_int(Operand op) {
    if (op->GetType()->GetTypeEnum() == InnerDataType::IR_Value_INT) 
        return true;
    else return false;
}
bool is_float(Operand op) {
    if (op->GetType()->GetTypeEnum() == InnerDataType::IR_Value_Float) 
        return true;
    else return false;
}
//指令选择
MachineInst* InstSelect(MachineBasicBlock* parent, User& inst) {
    MachineInst* machineinst = nullptr;
     if (auto Tempinst = dynamic_cast<AllocaInst*>(&inst)) {
        machineinst = new MachineInst(parent, "alloca");
    }
    else if (auto Tempinst = dynamic_cast<StoreInst*>(&inst)) {
        machineinst = MatchStoreInst(parent,Tempinst);
    }
    else if (auto Tempinst = dynamic_cast<LoadInst*>(&inst)) {
        machineinst = MatchLoadInst(parent, Tempinst);
    }
    else if (auto Tempinst = dynamic_cast<FPTSI*>(&inst)) {
        machineinst = MatchFPTSI(parent, Tempinst);
    }
    else if (auto Tempinst = dynamic_cast<SITFP*>(&inst)) {
        machineinst = MatchSITFP(parent, Tempinst);
    }
    // else if (auto Tempinst = dynamic_cast<UnCondInst*>(inst)) {
    //     machineinst = MatchUnCondInst(inst);
    // }  
    // else if (auto Tempinst = dynamic_cast<CondInst*>(&inst)) {
    //     machineinst = MatchCondInst(Tempinst);
    // }
    else if (auto Tempinst = dynamic_cast<CallInst*>(&inst)) {
        machineinst = MatchCallInst(parent, Tempinst);
    }
    // else if (auto Tempinst = dynamic_cast<GetElementPtrInst*>(inst)) {
    //     machineinst = MatchCallInst(inst);
    // }
    else if (auto Tempinst = dynamic_cast<BinaryInst*>(&inst)) {
        machineinst = MatchBinaryInst(parent, Tempinst);
    }
    else if (auto Tempinst = dynamic_cast<RetInst*>(&inst)) {
        machineinst = MatchRetInst(parent, Tempinst);
    }
    else {
        //std::cout << "Error: No Such Instruction." << std::endl;
        machineinst = new MachineInst(parent, "white");
    }
    return machineinst;
} 

MachineInst* MatchStoreInst(MachineBasicBlock* parent, StoreInst* inst) {
    std::string op = "sw";
    Operand rd = (inst->Getuselist())[0]->GetValue();
    Operand rs1 = (inst->Getuselist())[1]->GetValue();
    MachineInst* machineinst = new MachineInst(parent, op, rd, rs1);
    return machineinst;
}
MachineInst* MatchLoadInst(MachineBasicBlock* parent, LoadInst* inst) {
    std::string op = "lw";
    Operand rd = inst->GetDef();        
    Operand rs1 = (inst->Getuselist())[0]->GetValue();
    MachineInst* machineinst = new MachineInst(parent, op, rd, rs1);
    return machineinst;
}
MachineInst* MatchFPTSI(MachineBasicBlock* parent,FPTSI* inst) {
    std::string op = "fcvt.s.w";
    Operand rd = inst->GetDef();
    Operand rs1 = (inst->Getuselist())[0]->GetValue();
    MachineInst* machineinst = new MachineInst(parent, op, rd, rs1);
    return machineinst;
}
MachineInst* MatchSITFP(MachineBasicBlock* parent,SITFP* inst) {
    std::string op = "fcvt.w.s";
    Operand rd = inst->GetDef();
    Operand rs1 = (inst->Getuselist())[0]->GetValue();
    MachineInst* machineinst = new MachineInst(parent, op, rd, rs1);
    return machineinst;
}
MachineInst* MatchUnCondInst(MachineBasicBlock* parent, UnCondInst* inst) {
    std::string op = "j";
    std::string temp = parent->get_name();
    parent->get_block()->SetName(temp);
    parent->set_name(temp);
    MachineInst* machineinst = new MachineInst(parent, op);
    return machineinst;
}
// MachineInst* MatchCondInst(CondInst* inst) {

// }

MachineInst* MatchCallInst(MachineBasicBlock* parent, CallInst* inst) {
    Operand rd = inst->Getuselist()[0]->GetValue();
    MachineInst* machineinst = new MachineInst(parent, "call", rd);
    return machineinst;
}

MachineInst* MatchRetInst(MachineBasicBlock* parent, RetInst* inst) {
    Operand rs = inst->Getuselist()[0]->GetValue();
    std::cout << "    lw a0, " << rs->GetName() << std::endl; 
    //size_t of = parent->get_parent()->get_offset(rs->GetName());
    //std::cout << "    sw a0, -" << of << "(s0)" << std::endl; 
    MachineInst* machineinst = new MachineInst(parent, "ret");
    return machineinst;
}

// MachineInst* ConvertToMachineInst (std::variant<MachineBinaryInst*, MachineCmpInst*>& variant) {
//     return std::visit([](auto&& arg) -> MachineInst* {
//         using T = std::decay_t<decltype(arg)>;
//         if constexpr (std::is_same_v<T, MachineBinaryInst*>) {
//             return static_cast<MachineInst*>(arg);
//         } else if constexpr (std::is_same_v<T, MachineCmpInst*>) {
//             return static_cast<MachineInst*>(arg);
//         } else {
//             return nullptr;
//         }
//     }, variant);    
// }
MachineInst* MatchBinaryInst(MachineBasicBlock* parent, BinaryInst* inst) {
    MachineInst* machineinst = nullptr;
    std::string op = inst->GetOperation();
    std::string opcode;
    Operand rd = inst->GetDef();
    Operand rs1 = (inst->Getuselist())[0]->GetValue();
    Operand rs2 = (inst->Getuselist())[1]->GetValue();
    if (op == "Op_Add") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "addw";
        else if (is_float(rs1) && is_float(rs2)) 
            opcode = "fadd.s";
        else opcode = "white";
    }
    else if (op == "Op_Sub") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "subw";
        else if (is_float(rs1) && is_float(rs2)) 
            opcode = "fsub.s";
        else opcode = "white";
    }
    else if (op == "Op_Mul") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "mulw";
        else if (is_float(rs1) && is_float(rs2)) 
            opcode = "fmul.s";
        else opcode = "white";
    }
    else if (op == "Op_Div") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "divw";
        else if (is_float(rs1) && is_float(rs2)) 
            opcode = "fdiv.s";
        else opcode = "white";
    }  
    else if (op == "Op_Mod") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "remw";
        // else if (is_float(rs1) && is_float(rs2)) 
        //     opcode = "frem.s";
        else opcode = "white";
    }  
    else if (op == "Op_And") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "and";
        // else if (is_float(rs1) && is_float(rs2)) 
        //     opcode = " ";
        else opcode = "white";
    }  
    else if (op == "Op_Or") {
        if (is_int(rs1) && is_int(rs2)) 
            opcode = "or";
        else opcode = "white";
    }  
    //To Do 
    // else if (op == "Op_E") {
    //     if (is_int(rs1) && is_int(rs2)) {
    //         //MachineInst* inst = new MachineInst("?", rd, rs1, rs2);
    //         //return inst;
    //     }
    //     else if (0) {
    //         //To Do
    //         //imm
    //     }
    // }
    // else if (op == "Op_NE") {
    //     if (is_int(rs1) && is_int(rs2)) {

    //     }
    //     else if (0) {
    //     }
    // }
    // else if (op == "Op_GE") {
    //     if (is_int(rs1) && is_int(rs2)) {

    //     }
    //     else if (0) {
    //     }
    // }
    // else if (op == "Op_L") {
    //     if (is_int(rs1) && is_int(rs2)) {

    //     }
    //     else if (0) {
    //     }
    // }
    // else if (op == "Op_LE") {
    //     if (is_int(rs1) && is_int(rs2)) {

    //     }
    //     else if (0) {
    //     }
    // }
    // else if (op == "Op_G") {
    //     if (is_int(rs1) && is_int(rs2)) {

    //     }
    //     else if (0) {
    //     }
    // }
    else {
        std::cout << "Error: No Such Binaryinst!" << std::endl;
        machineinst = new MachineInst(parent, "white");
        return machineinst; 
    }
    machineinst = new MachineInst(parent, opcode, rd, rs1, rs2);
    return machineinst;
}



