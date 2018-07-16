//
// Copyright (c) 2015,2018 CNRS
// Copyright (c) 2015 Wandercraft, 86 rue de Paris 91400 Orsay, France.
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __se3_visitor_hpp__
#define __se3_visitor_hpp__

#define BOOST_FUSION_INVOKE_MAX_ARITY 10
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include "pinocchio/multibody/joint/joint-collection.hpp"


namespace boost {
  namespace fusion {

    // Append the element T at the front of boost fusion vector V.
    template<typename T,typename V>
    typename result_of::push_front<V const, T>::type
    append(T const& t,V const& v) { return push_front(v,t); }

    // Append the elements T1 and T2 at the front of boost fusion vector V.
    template<typename T1,typename T2,typename V>
    typename result_of::push_front<typename result_of::push_front<V const, T2>::type const, T1>::type
    append2(T1 const& t1,T2 const& t2,V const& v) { return push_front(push_front(v,t2),t1); }
  }
}


namespace se3
{
  namespace fusion
  {
    namespace bf = boost::fusion;
    
    template<typename Visitor>
    struct JointVisitor : public boost::static_visitor<>
    {
      Visitor & derived() { return *static_cast<Visitor*>(this); }
      const Visitor & derived() const { return *static_cast<const Visitor*>(this); }
      
      template<typename ModelDerived>
      void operator()(const JointModelBase<ModelDerived> & jmodel) const
      {
        bf::invoke(&Visitor::template algo<ModelDerived>,
                   bf::append2(boost::ref(jmodel),
                               boost::ref(boost::get<typename ModelDerived::JointDataDerived>(derived().jdata)),
                               derived().args));
      }
      
      template<typename JointCollection, typename ArgsTmp>
      static void run(const JointModelTpl<JointCollection> & jmodel,
                      JointDataTpl<JointCollection> & jdata,
                      ArgsTmp args)
      {
        return boost::apply_visitor(Visitor(jdata,args),jmodel);
      }
      
      template<typename JointModelDerived, typename ArgsTmp>
      static void run(const JointModelBase<JointModelDerived> & jmodel,
                      ArgsTmp args)
      {
        Visitor visit(args);
        visit(jmodel.derived());
      }
    };
    
    template<typename Visitor>
    struct JointModelVisitor : public boost::static_visitor<>
    {
      
      Visitor & derived() { return *static_cast<Visitor*>(this); }
      const Visitor & derived() const { return *static_cast<const Visitor*>(this); }
      
      template<typename ModelDerived>
      void operator()(const JointModelBase<ModelDerived> & jmodel) const
      {
        bf::invoke(&Visitor::template algo<ModelDerived>,
                   bf::append(boost::ref(jmodel),
                              derived().args));
      }
      
      template<typename JointCollection, typename ArgsTmp>
      static void run(const JointModelTpl<JointCollection> & jmodel,
                      ArgsTmp args)
      {
        return boost::apply_visitor(Visitor(args),jmodel);
      }
      
      template<typename JointModelDerived, typename ArgsTmp>
      static void run(const JointModelBase<JointModelDerived> & jmodel,
                      ArgsTmp args)
      {
        Visitor visit(args);
        visit(jmodel.derived());
      }
    };
    
  } // namespace fusion
} // namespace se3

#define JOINT_VISITOR_INIT(VISITOR)					                                \
  VISITOR(JointDataVariant & jdata, ArgsType args)                          \
  : jdata(jdata),args(args)                                                 \
  {}                                                                        \
  using se3::fusion::JointVisitor< VISITOR >::run;			                    \
  JointDataVariant & jdata;						                                      \
  ArgsType args

#define JOINT_MODEL_VISITOR_INIT(VISITOR)                                   \
  VISITOR(ArgsType args) : args(args) {}                                 \
  using se3::fusion::JointModelVisitor< VISITOR >::run;                     \
  ArgsType args

#endif // ifndef __se3_visitor_hpp__
