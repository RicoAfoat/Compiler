//===-- ConstantFolding.h - Fold instructions into constants ----*- C++ -*-===//

// This file declares routines for folding instructions into constants when all
// operands are constants, for example "sub i32 1, 0" -> "1".
//
// Also, to supplement the basic VMCore ConstantExpr simplifications,
// this file declares some additional folding routines that can make use of
// DataLayout information. These functions cannot go in VMCore due to library
// dependency issues.
//
//===----------------------------------------------------------------------===//
#pragma once
#include "CFG.hpp"
class ConstantFolding
{
/// ConstantFoldInstruction - Try to constant fold the specified instruction.
/// If successful, the constant result is returned, if not, null is returned.
/// Note that this fails if not all of the operands are constant.  Otherwise,
/// this function can only fail when attempting to fold instructions like loads
/// and stores, which have no constant expression form.
ConstantData* ConstantFoldInstruction(User* inst, BasicBlock* block);
/// ConstantFoldConstantExpression - Attempt to fold the constant expression.
/// If successful, the constant result is result is returned, if not, null is
/// returned.
ConstantData* ConstantFoldConstantExpression(ConstantExpr* _ConstantExpr);

/// ConstantFoldInstOperands - Attempt to constant fold an instruction with the
/// specified operands.  If successful, the constant result is returned, if not,
/// null is returned.  Note that this function can fail when attempting to
/// fold instructions like loads and stores, which have no constant expression
/// form.
ConstantData *ConstantFoldInstOperands(User* inst, std::vector<ConstantData*> _Operands);

/// ConstantFoldCompareInstOperands - Attempt to constant fold a compare
/// instruction (icmp/fcmp) with the specified operands.  If it fails, it
/// returns a constant expression of the specified operands.
ConstantData *ConstantFoldCompareInstOperands(BasicBlock* Pred, Value* LHS, Value* RHS);

ConstantData *ConstantFoldLoadInst(const LoadInst* _LoadInst);

/// canConstantFoldCallTo - Return true if its even possible to fold a call to
/// the specified function.
bool canConstantFoldCallto(User* inst);

/// ConstantFoldCall - Attempt to constant fold a call to the specified function
/// with the specified arguments, returning null if unsuccessful.
ConstantData *ConstantFoldCall(Function* func, std::vector<ConstantData*> Operands);

/// ConstantFoldBinaryOpOperands - Attempt to constant fold a binary operation with the
/// specified operands. If it fails, it returns a constant expression of the specified
/// operands.
ConstantData *ConstantFoldBinaryOpOperands(unsigned Opcode, Value* LHS, Value* RHS);

/// One of Op0/Op1 is a Constant expr;
/// Attempt to symbolically evaluate the result of a binary operator merging
/// these together
ConstantData *SymbolicallyEvaluateBinop(unsigned Opcode, Value* Op0, Value* Op1);
};
