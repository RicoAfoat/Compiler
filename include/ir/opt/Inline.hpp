#include "CFG.hpp"
#include "LoopInfo.hpp"
#include "dominant.hpp"

class InlineHeuristic{
    public:
    virtual bool CanBeInlined(CallInst*)=0;
    //everytime we call get we'll reanalyze the whole module again to see which can be inlined
    static std::unique_ptr<InlineHeuristic> get(Module&);
};

class InlineHeuristicManager:public InlineHeuristic,public std::vector<std::unique_ptr<InlineHeuristic>>{
    public:
    virtual bool CanBeInlined(CallInst*)override;
    InlineHeuristicManager();
};

/// @note this should be placed at the end of the InlineHeuristic get
class SizeLimit:public InlineHeuristic{
    // if we use inline, we should consider some optimization effect, for maybe the size will shrink dramatically
    const size_t maxframesize=7864320;// 7.5MB
    const size_t maxsize=10000;
    public:
    bool CanBeInlined(CallInst*)override;
    SizeLimit();
};
class NoRecursive:public InlineHeuristic{
    Module& m;
    // std::unordered_set<Function*> recursive;
    public:
    bool CanBeInlined(CallInst*)override;
    NoRecursive(Module&);
};

class Inliner
{
public:
    Inliner(Module& module):m(module){}
    void Run();
    void PrintPass();
    void Inline();
private:
    std::vector<BasicBlock*> CopyBlocks(User* inst);
    void HandleVoidRet(BasicBlock* spiltBlock, std::vector<BasicBlock*>& blocks);
    void HandleRetPhi(BasicBlock* RetBlock, PhiInst* phi, std::vector<BasicBlock*>& blocks);
private:
    Module& m;
    // LoopAnalysis* loopAnalysis;
    void init();
    std::vector<User*> NeedInlineCall;
};