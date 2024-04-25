#include "PhiElimination.hpp"
#include "RISCVMIR.hpp"
#include <forward_list>
#include <queue>
#include "TrivalOperation.hpp"

RISCVBasicBlock* PhiElimination::find_copy_block(BasicBlock* from,BasicBlock* to){
    if(CopyBlockFinder.find(from)==CopyBlockFinder.end())
        CopyBlockFinder[from]=std::map<BasicBlock*,RISCVBasicBlock*>();
    if(CopyBlockFinder[from].find(to)==CopyBlockFinder[from].end()){
        auto mfunc=cxt.mapping(from->GetParent())->as<RISCVFunction>();
        auto mfrom=cxt.mapping(from)->as<RISCVBasicBlock>();
        auto mto=cxt.mapping(to)->as<RISCVBasicBlock>();
        
        auto critialmbb=new RISCVBasicBlock();
        mfrom->replace_succ(mto,critialmbb);
        auto uncond=new RISCVMIR(RISCVMIR::_j);
        uncond->AddOperand(mto);
        critialmbb->push_back(uncond);

        mfunc->push_back(critialmbb);

        CopyBlockFinder[from][to]=critialmbb;
    }
    return CopyBlockFinder[from][to];
}

void PhiElimination::runOnGraph(BasicBlock* pred,BasicBlock* succ,std::vector<std::pair<RISCVMOperand*,RISCVMOperand*>>& vec){

    /// 如果对 phi 函数的理解没问题的话应该是内向基环树

    struct Node{
        std::forward_list<int> nxt;
        int indo=0;
    };
    std::map<RISCVMOperand*,int> mp;
    for(int i=0,limi=vec.size();i<limi;i++)
        mp[vec[i].second]=i;
    std::vector<Node> graph(vec.size());
    for(int i=0,limi=vec.size();i<limi;i++)
        if(mp.find(vec[i].first)!=mp.end()){
            auto src=i,dst=mp[vec[i].first];
            graph[src].nxt.push_front(dst);
            graph[dst].indo++;
        }
    
    std::queue<int> que;
    
    for(int i=0,limi=vec.size();i<limi;i++)
        if(graph[i].indo==0)
            que.push(i);

    std::map<RISCVMOperand*,RISCVMOperand*> stagedRegister;

    /// @note Get the basicblock for emitting copy inst
    RISCVBasicBlock* emitMBB;
    if(auto cond=dynamic_cast<CondInst*>(pred->back()))
        /// @todo try split critial edge
        // emitMBB=;
        emitMBB=find_copy_block(pred,succ);
    else emitMBB=cxt.mapping(pred)->as<RISCVBasicBlock>();
    
    for(int i=0,j=0,limi=vec.size();i<limi;){
        auto visit=[&](int ind){
            
            i++;

            auto [src,dst]=vec[ind];
            if(stagedRegister.find(src)!=stagedRegister.end())
                src=stagedRegister[src];

            if(graph[i].indo!=0){
                /// @note stage register
                auto stageR=cxt.createVReg(src->GetType());
                emitMBB->push_before_branch(emitCopy(stageR,dst));
                stagedRegister[dst]=stageR;
            }

            emitMBB->push_before_branch(emitCopy(dst->as<VirRegister>(),src));

            auto& node=graph[ind];
            for(auto nxt:node.nxt){
                auto& nnode=graph[nxt];
                nnode.indo--;
                if(nnode.indo==0)
                    que.push(nxt);
            }
        };
        while(!que.empty()){
            int cur=que.front();que.pop();
            visit(cur);
        }
        for(;j<limi;j++){
            if(graph[j].indo){
                visit(j);
                if(!que.empty())break;
            }
        }
    }
}

void PhiElimination::runonBasicBlock(BasicBlock* bb){
    /// @note for pred bb, need copy first 2 second
    /// @note phi 的 src 可能是 IMM,想想办法
    std::map<BasicBlock*,std::vector<std::pair<RISCVMOperand*,RISCVMOperand*>>> phiMap;
    for(auto inst:*bb){
        if(auto phi=dynamic_cast<PhiInst*>(inst)){
            auto& phirec=phi->PhiRecord;
            for(auto& [idx,pair]:phirec){
                auto [val,bb]=pair;
                auto src=cxt.mapping(val);
                auto dst=cxt.mapping(phi);
                phiMap[bb].emplace_back(src,dst);
            }
        }
        else break;
    }
    for(auto &[src,vec]:phiMap)
        runOnGraph(src,bb,vec);
}

bool PhiElimination::run(Function* f){
    for(auto bb:*f)
        runonBasicBlock(bb);
}