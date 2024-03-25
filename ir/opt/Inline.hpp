#include "CFG.hpp"
#include "LoopInfo.hpp"
#include "dominant.hpp"
#define INT_MAX   __INT_MAX__
#define INT_MIN (-__INT_MAX__ -1)
class Inline : public dominance
{
public:
    void RunOnFunction();
    void PrintPass();
    Inline(Function* f, int num) : func(f), dominance(f, num){}
private:
    int BlockNum;
    Function* func;
    void RemoveDeadFunction(Function* func);
    void inlineCalls(CallInst* call);
    bool shouldInline(CallInst* call);
    std::stack<CallInst*> callStack;
};


class CallGraph:public Module
{
    Module& M;
public: 
    // CallGraph()=default;
    typedef std::map<Function*, std::unique_ptr<CallGraphNode>> FunctionMapTy;
    
    FunctionMapTy FunctionMap;

    /// @brief Root is the root of the graph, or the external node if a 'main' \
    function couldn't be found.
    CallGraphNode* Root;

    /// @brief This node has edges to all external functions and those internal \
    functions that have their address taken.
    CallGraphNode* ExternalCallingNode;
    
    /// @brief This node has edges to it from all functions making indirecrt \
    calls or calling an external function.
    std::unique_ptr<CallGraphNode> CallsExternalNode;

    /// @brief Replace the function represented by this node by another
    
    /// This does not rescan the body of the function, so it is suitable
    /// when splicing the body of one function to another while also 
    /// updating all callers from the old function to the new.

    void spliceFunction(Function* From, Function* To);

    /// @brief Add a function to the call graph, and link the node 
    /// to all of the function it calls.

    void addToCallGraph(Function* F);

public:
    CallGraph(Module& M);
    CallGraph(CallGraph&& Arg);
    ~CallGraph();

    Module& GetModule() const { return M; }
    typedef FunctionMapTy::iterator iterator;
    typedef FunctionMapTy::const_iterator const_iterator;
    iterator begin() { return FunctionMap.begin(); }
    iterator end() { return FunctionMap.end(); }
    std::reverse_iterator<iterator> rbegin() { return FunctionMap.rbegin(); }
    std::reverse_iterator<iterator> rend() { return FunctionMap.rend(); }
    
    CallGraphNode* operator[](Function* func);

    /// @brief Get the External Node who is calling the internal function.
    CallGraphNode* getExternalCallingNode() const { return ExternalCallingNode;}
    /// @brief Get the Internal Node who is calling the external function.
    CallGraphNode* getCallsExternalNode() const { return CallsExternalNode.get(); }

    /// @brief Update CallGraph Info

    Function* EraseFromModule(CallGraphNode* node);

    /// @brief Simlar to operator[], but this will insert a callgraphNode
    CallGraphNode* GetOrInsertFunc(Function* func);
};

class CallGraphNode
{
    friend class CallGraph;
    Function* F;
    /// @brief A pair of callinst and call graph node
    typedef std::pair<CallInst*, CallGraphNode*> CallRecord;
    
    /// @brief Record Vector to store CalledFunctions
    typedef std::vector<CallRecord> CalledFuncsVector;

        std::vector<CallRecord> CalledFunctions;
    public:
        CallGraphNode(Function* F) : F(F) {}

        typedef std::vector<CallRecord>::iterator iterator;
        
        Function* getFunc() const { return F; }

        iterator begin() { return CalledFunctions.begin(); }
        iterator end() { return CalledFunctions.end(); }
        std::reverse_iterator<iterator> rbegin() { return CalledFunctions.rbegin(); }
        std::reverse_iterator<iterator> rend() { return CalledFunctions.rend(); }

        bool is_empty() const { return CalledFunctions.empty(); }
        unsigned size() const { return (unsigned)CalledFunctions.size(); }

        CallGraphNode* operator[](unsigned i) const;

        /// @brief Update callgraph
        void removeAllCalledFunctions();

        void MoveCallFuncFrom(CallGraphNode *Node);

        void AddCallFunc(CallInst* inst);

        void DelCallEdge(iterator iter);
};

class CallGraphSCC
{
    const CallGraph& Callgraph;

    void *Context;
    std::vector<CallGraphNode*> Nodes;

public:
    CallGraphSCC(CallGraph& cg, void *context) : Callgraph(cg), Context(context) {}


    bool isSingular() const { return Nodes.size() == 1; }
    unsigned size() const { return Nodes.size(); }

    /// @brief ReplaceNdoe - this informs the SCC and the pass mamager that the \
    specified Old node has been deleted, and New is to be used in its place.
    void ReplaceNode(CallGraphNode* Old, CallGraphNode* New); ///todo update

    typedef std::vector<CallGraphNode*>::const_iterator iterator;
    iterator begin() { return Nodes.begin(); }
    iterator end() { return Nodes.end(); }

    const CallGraph& getCallGraph() const{ return Callgraph; }
};

namespace InlineConstants
{
    // Various magic constants used to adjust heuristics.
    const int InstrCost = 5;
    const int CallCost = 25;
    const int IndirectCallCost = 100;
    const int LastCallToStaticBonus = -15000;
    const int ColdccPenalty = 2000;
    const int NoreturnCost = 10000;
    /// Do not inline functions which allocate this many bytes on the stack
    /// when the caller is recursive.
    const int TotalAllocaSizeRecursiveCaller = 1024;
}

class InlineCost
{
    enum SentineValues
    {
        AlwaysInlineCost = INT_MIN,
        NeverInlineCost = INT_MAX
    };
    
};