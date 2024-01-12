#include "Type.hpp"
#include <iostream>
#include <cassert>
#include <map>

/*Type*/
Type::Type(InnerDataType _tp):tp(_tp){}
InnerDataType Type::GetTypeEnum(){return tp;}
int Type::get_layer(){return 0;}
Type* Type::NewTypeByEnum(InnerDataType _tp){
    switch (_tp)
    {
    case IR_Value_INT:return IntType::NewIntTypeGet(); 
    case IR_Value_VOID:return VoidType::NewVoidTypeGet();
    case IR_Value_Float:return FloatType::NewFloatTypeGet();
    default:assert(0);
    }
}
void Type::align_print()
{
    switch (tp)
    {
    case IR_PTR:
        std::cout<<"align 8";
        break;
    case IR_Value_INT:
    case IR_Value_Float:
    case IR_ARRAY:
        std::cout<<"align 4";
        break;
    default:
        assert(0);
        break;
    }
}

/*IntType*/
IntType::IntType():Type(IR_Value_INT){}
IntType* IntType::NewIntTypeGet(){
    static IntType single;
    return &single;
}
void IntType::print(){
    std::cout<<"i32";
}

/*FloatType*/
FloatType::FloatType():Type(IR_Value_Float){}
FloatType* FloatType::NewFloatTypeGet(){
    static FloatType single;
    return &single;
}
void FloatType::print(){
    std::cout<<"float";
}

/*Bool*/
BoolType::BoolType():Type(IR_Value_INT){}
BoolType* BoolType::NewBoolTypeGet(){
    static BoolType single;
    return &single;
}
void BoolType::print(){
    std::cout<<"i1";
}

/*Void*/
VoidType::VoidType():Type(IR_Value_VOID){}
VoidType* VoidType::NewVoidTypeGet(){
    static VoidType single;
    return &single;
}
void VoidType::print(){
    std::cout<<"void";
}

/*HasSubType*/
HasSubType::HasSubType(InnerDataType tp_enum,Type* _subtype):Type(tp_enum),subtype(_subtype),layer(_subtype->get_layer()+1){}
int HasSubType::get_layer(){return layer;}
Type* HasSubType::GetSubType(){
    return subtype;
}

/*PointerType*/
PointerType* PointerType::NewPointerTypeGet(Type* _subtype){
    static std::map<Type*,PointerType*> single;
    auto& tmp=single[_subtype];
    if(tmp==nullptr)tmp=new PointerType(_subtype);
    return tmp;
}
PointerType::PointerType(Type* _subtype):HasSubType(IR_PTR,_subtype){}
void PointerType::print(){
    subtype->print();
    std::cout<<"*";
}

/*ArrayType*/
ArrayType* ArrayType::NewArrayTypeGet(int NumEle,Type* _subtype){
    using Key=std::pair<int,Type*>;
    static std::map<Key,ArrayType*> single;
    auto& tmp = single[Key(NumEle, _subtype)];
    if(tmp==nullptr)tmp=new ArrayType(NumEle, _subtype);
    return tmp;
}
ArrayType::ArrayType(int _numEle,Type* _subtype):NumEle(_numEle),HasSubType(IR_ARRAY,_subtype){}
void ArrayType::print(){
    std::cout<<"["<<NumEle<<" x ";
    subtype->print();
    std::cout<<"]";
}