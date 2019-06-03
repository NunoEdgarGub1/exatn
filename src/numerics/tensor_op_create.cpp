/** ExaTN::Numerics: Tensor operation: Creates a tensor
REVISION: 2019/06/03

Copyright (C) 2018-2019 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle) **/

#include "tensor_op_create.hpp"

namespace exatn{

namespace numerics{

TensorOpCreate::TensorOpCreate():
 TensorOperation(TensorOpCode::CREATE,1,0)
{
}

bool TensorOpCreate::isSet() const
{
 return (this->getNumOperandsSet() == this->getNumOperands());
}

std::unique_ptr<TensorOperation> TensorOpCreate::createNew()
{
 return std::unique_ptr<TensorOperation>(new TensorOpCreate());
}

} //namespace numerics

} //namespace exatn