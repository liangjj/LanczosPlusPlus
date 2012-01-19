
/*
// BEGIN LICENSE BLOCK
Copyright (c) 2009 , UT-Battelle, LLC
All rights reserved

[Lanczos++, Version 1.0.0]

*********************************************************
THE SOFTWARE IS SUPPLIED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. 

Please see full open source license included in file LICENSE.
*********************************************************

*/

#ifndef BASIS_FEASBASED_SC_H
#define BASIS_FEASBASED_SC_H
#include "BasisOneSpin.h"

namespace LanczosPlusPlus {

	template<typename GeometryType>
	class BasisFeAsBasedSc {
	public:
		
		typedef BasisOneSpin BasisType;
		typedef BasisType::WordType WordType;
		enum {SPIN_UP,SPIN_DOWN};
		static size_t const ORBITALS  = BasisType::ORBITALS;
		static int const FERMION_SIGN = BasisType::FERMION_SIGN;
		enum {DESTRUCTOR=BasisType::DESTRUCTOR,CONSTRUCTOR=BasisType::CONSTRUCTOR};
		
		
		BasisFeAsBasedSc(const GeometryType& geometry, size_t nup,size_t ndown)
		: basis1_(geometry.numberOfSites(),nup),
		  basis2_(geometry.numberOfSites(),ndown)
		{
//			std::cout<<"Basis1\n";
//			std::cout<<basis1_;
//			std::cout<<"Basis2\n";
//			std::cout<<basis2_;
		}
		
		BasisFeAsBasedSc(const GeometryType& geometry, size_t nup)
		: basis1_(geometry.numberOfSites(),nup),
		  basis2_(geometry.numberOfSites(),nup)
		{
			std::string s = "BasisFeBasedSc::ctor(...): obsolete. ";
			s+= "This probably means that you can't compute the Green function";
			s+= " with this model (sorry). It might be added in the future.\n";
			throw std::runtime_error(s.c_str());
		}
		

		static const WordType& bitmask(size_t i)
		{
			return BasisType::bitmask(i);
		}
		
		size_t dofs() const { return 2*ORBITALS; }

		size_t size() const { return basis1_.size()*basis2_.size(); }
		
		const WordType& operator()(size_t i,size_t spin) const
		{
			size_t y = i/basis1_.size();
			size_t x = i%basis1_.size();
			return (spin==SPIN_UP) ? basis1_[x] : basis2_[y];
		}

		size_t perfectIndex(const std::vector<WordType>& kets) const
		{
			assert(kets.size()==2);
			return  perfectIndex(kets[0],kets[1]);
		}

		size_t perfectIndex(WordType ket1,WordType ket2) const
		{
			return basis1_.perfectIndex(ket1) + basis2_.perfectIndex(ket2)*basis1_.size();
		}
		

		size_t getN(size_t i,size_t spin,size_t orb) const
		{
			size_t y = i/basis1_.size();
			size_t x = i%basis1_.size();
			return (spin==SPIN_UP) ? basis1_.getN(x,orb) : basis2_.getN(y,orb);
		}
		

		size_t getBraIndex(size_t i,size_t what,size_t sector) const
		{
			size_t y = i/basis1_.size();
			size_t x = i%basis1_.size();
			size_t i1 = basis1_.perfectIndex(basis1_[x]);
			size_t i2 = basis2_.perfectIndex(basis2_[y]);
			size_t spin = sector/2;
			size_t orb = (sector & 1);
			if (spin==SPIN_UP) {
				i1 = basis1_.getBraIndex(x,what,orb);
			} else {
				i2 =  basis2_.getBraIndex(y,what,orb);
			}

			return i1 + i2*basis1_.size();
		}
		

		int doSign(size_t i,size_t site,size_t sector) const
		{
			size_t y = i/basis1_.size();
			size_t x = i%basis1_.size();
			size_t spin = sector/2;
			size_t orb = (sector & 1);
			if (spin==SPIN_UP) {
				return basis1_.doSign(x,site,orb);
			}
			size_t c = basis1_.getN(x);
			int ret = 1;
			if (c&1) ret = FERMION_SIGN;
			return ret * basis2_.doSign(y,site,orb);
		}
		

		int doSign(
				WordType ket1,
				WordType ket2,
				size_t i,
				size_t orb1,
				size_t j,
				size_t orb2,
				size_t spin) const
		{
			if (i > j) {
				std::cerr<<"FATAL: At doSign\n";
				std::cerr<<"INFO: i="<<i<<" j="<<j<<std::endl;
				std::cerr<<"AT: "<<__FILE__<<" : "<<__LINE__<<std::endl;
				throw std::runtime_error("FeBasedSc::doSign(...)\n");
			}
			if (spin==SPIN_UP) {
				return basis1_.doSign(ket1,i,orb1,j,orb2);
			}
			return basis2_.doSign(ket2,i,orb1,j,orb2);
		}
		

		size_t isThereAnElectronAt(
				size_t ket1,
				size_t ket2,
				size_t site,
				size_t spin,
				size_t orb) const
		{
			if (spin==SPIN_UP)
				return basis1_.isThereAnElectronAt(ket1,site,orb);
			return basis2_.isThereAnElectronAt(ket2,site,orb);
		}
		
		

	private:
		

		
		
		BasisType basis1_,basis2_;
		
	}; // class BasisFeAsBasedSc
	
} // namespace LanczosPlusPlus
#endif

