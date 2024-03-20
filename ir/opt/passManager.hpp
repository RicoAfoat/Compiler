#pragma once
#include "ConstantFold.hpp"
#include "ConstantProp.hpp"
#include "IDF.hpp"
#include "LivenessAnalysis.hpp"
#include "PromoteMemtoRegister.hpp"
#include "SSAPRE.hpp"
#include "dominant.hpp"
#include "DCE.hpp"
#include "ADCE.hpp"
#include "PassManagerBase.hpp"
#include "../../yacc/parser.hpp"
#include "LoopInfo.hpp"
#include "../Analysis/DealCriticalEdges.hpp"
class PassManager:public PassManagerBase {
public:
  PassManager() : InitpassRecorder(50) {}

  void IncludePass(int pass);
  void InitPass();
  void RunOnFunction();
  void Anlaysis();
  void PrintPass(){};
  void setAnalsis(bool choi){
    Analysis=choi;
  }
private:
  bool Analysis=false; 
  std::vector<int> InitpassRecorder;
  std::unique_ptr<LoopInfo> m_loopAnlay;
  std::unique_ptr<dominance> m_dom;
  std::unique_ptr<PRE> m_pre;
  std::unique_ptr<ConstantProp> m_constprop;
  std::unique_ptr<ADCE> m_adce;
  std::unique_ptr<DCE> m_dce;
  std::unique_ptr<LivenessAnalysis> m_liveness;
  std::unique_ptr<ElimitCriticalEdge> m_eliedg;
};