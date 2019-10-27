/** ExaTN::Numerics: Tensor network expansion
REVISION: 2019/10/27

Copyright (C) 2018-2019 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle) **/

#include "tensor_expansion.hpp"

#include <cassert>

namespace exatn{

namespace numerics{

bool TensorExpansion::appendComponent(std::shared_ptr<TensorNetwork> network, //in: tensor network
                                      const std::complex<double> coefficient) //in: expansion coefficient
{
 auto output_tensor = network->getTensor(0);
 const auto output_tensor_rank = output_tensor->getRank();
 //Check validity:
 if(!(components_.empty())){
  auto first_tensor = components_[0].network->getTensor(0);
  const auto first_tensor_rank = first_tensor->getRank();
  if(first_tensor_rank != output_tensor_rank){
   std::cout << "#ERROR(exatn::numerics::TensorExpansion::appendComponent): Tensor rank mismatch: "
             << first_tensor_rank << " versus " << output_tensor_rank << std::endl;
   assert(false);
  }
  auto congruent = output_tensor->isCongruentTo(*first_tensor);
  if(!congruent){
   std::cout << "#ERROR(exatn::numerics::TensorExpansion::appendComponent): Tensor shape mismatch!" << std::endl;
   assert(false);
  }
  const auto * output_legs = network->getTensorConnections(0);
  const auto * first_legs = components_[0].network->getTensorConnections(0);
  congruent = tensorLegsAreCongruent(output_legs,first_legs);
  if(!congruent){
   std::cout << "#ERROR(exatn::numerics::TensorExpansion::appendComponent): Tensor leg direction mismatch!" << std::endl;
   assert(false);
  }
 }
 //Append new component:
 components_.emplace_back(ExpansionComponent{network,coefficient});
 return true;
}


void TensorExpansion::conjugate()
{
 for(auto & component: components_){
  component.network->conjugate();
  component.coefficient = std::conj(component.coefficient);
 }
 ket_ = !ket_;
 return;
}


TensorExpansion::TensorExpansion(const TensorExpansion & expansion,       //in: tensor network expansion in some tensor space
                                 const TensorOperator & tensor_operator): //in: tensor network operator
 ket_(expansion.isKet())
{
 bool appended;
 for(auto term = expansion.cbegin(); term != expansion.cend(); ++term){
  for(auto oper = tensor_operator.cbegin(); oper != tensor_operator.cend(); ++oper){
   auto product = std::make_shared<TensorNetwork>(*(term->network));
   if(ket_){
    appended = product->appendTensorNetwork(TensorNetwork(*(oper->network)),oper->ket_legs);
    assert(appended);
    //`Reshufle bra legs coming from the tensor operator
   }else{
    appended = product->appendTensorNetwork(TensorNetwork(*(oper->network)),oper->bra_legs);
    assert(appended);
    //`Reshufle ket legs coming from the tensor operator
   }
   product->rename(oper->network->getName() + "*" + term->network->getName());
   appended = this->appendComponent(product,(oper->coefficient)*(term->coefficient));
   assert(appended);
  }
 }
}


void TensorExpansion::constructDirectProductTensorExpansion(const TensorExpansion & left_expansion,
                                                            const TensorExpansion & right_expansion)
{
 if(left_expansion.getNumComponents() == 0 || right_expansion.getNumComponents() == 0){
  std::cout << "#ERROR(exatn::numerics::TensorExpansion::constructDirectProductTensorExpansion): Empty input expansion!"
            << std::endl;
  assert(false);
 }
 bool appended;
 std::vector<std::pair<unsigned int, unsigned int>> pairing;
 for(auto left = left_expansion.cbegin(); left != left_expansion.cend(); ++left){
  for(auto right = right_expansion.cbegin(); right != right_expansion.cend(); ++right){
   auto product = std::make_shared<TensorNetwork>(*(left->network));
   appended = product->appendTensorNetwork(TensorNetwork(*(right->network)),pairing);
   assert(appended);
   product->rename(left->network->getName() + "*" + right->network->getName());
   appended = this->appendComponent(product,(left->coefficient)*(right->coefficient));
   assert(appended);
  }
 }
 return;
}


void TensorExpansion::constructInnerProductTensorExpansion(const TensorExpansion & left_expansion,
                                                           const TensorExpansion & right_expansion)
{
 if(left_expansion.getNumComponents() == 0 || right_expansion.getNumComponents() == 0){
  std::cout << "#ERROR(exatn::numerics::TensorExpansion::constructInnerProductTensorExpansion): Empty input expansion!"
            << std::endl;
  assert(false);
 }
 auto rank = left_expansion.cbegin()->network->getRank();
 assert(rank > 0);
 bool appended;
 std::vector<std::pair<unsigned int, unsigned int>> pairing(rank);
 for(unsigned int i = 0; i < rank; ++i) pairing[i] = {i,i};
 for(auto left = left_expansion.cbegin(); left != left_expansion.cend(); ++left){
  for(auto right = right_expansion.cbegin(); right != right_expansion.cend(); ++right){
   assert(right->network->getRank() == rank);
   auto product = std::make_shared<TensorNetwork>(*(right->network));
   appended = product->appendTensorNetwork(TensorNetwork(*(left->network)),pairing);
   assert(appended);
   product->rename(left->network->getName() + "*" + right->network->getName());
   appended = this->appendComponent(product,(left->coefficient)*(right->coefficient));
   assert(appended);
  }
 }
 return;
}


TensorExpansion::TensorExpansion(const TensorExpansion & left_expansion,  //in: tensor network expansion in some tensor space
                                 const TensorExpansion & right_expansion) //in: tensor network expansion from the same or dual space
{
 if((left_expansion.isKet() && right_expansion.isKet()) || (left_expansion.isBra() && right_expansion.isBra())){
  constructDirectProductTensorExpansion(left_expansion,right_expansion);
  ket_ = left_expansion.isKet();
 }else{
  constructInnerProductTensorExpansion(left_expansion,right_expansion);
  ket_ = true; //inner product tensor expansion is formally marked as ket but it is irrelevant
 }
}


TensorExpansion::TensorExpansion(const TensorExpansion & left_expansion,  //in: tensor network expansion in some tensor space
                                 const TensorExpansion & right_expansion, //in: tensor network expansion from the dual tensor space
                                 const TensorOperator & tensor_operator)  //in: tensor network operator
{
 constructInnerProductTensorExpansion(left_expansion,TensorExpansion(right_expansion,tensor_operator));
 ket_ = true; //inner product tensor expansion is formally marked as ket but it is irrelevant
}

} //namespace numerics

} //namespace exatn
