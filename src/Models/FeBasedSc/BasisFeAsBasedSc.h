
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
#include "BasisOneSpinFeAs.h"

namespace LanczosPlusPlus {

	template<typename GeometryType>
	class BasisFeAsBasedSc {

		static size_t orbitals_;

	public:
		
		typedef BasisOneSpinFeAs BasisType;
		typedef BasisType::WordType WordType;
		enum {SPIN_UP,SPIN_DOWN};
//		static size_t const orbitals_  = BasisType::orbitals_;
		static int const FERMION_SIGN = BasisType::FERMION_SIGN;
		enum {DESTRUCTOR=BasisType::DESTRUCTOR,CONSTRUCTOR=BasisType::CONSTRUCTOR};
		
		
		BasisFeAsBasedSc(const GeometryType& geometry, size_t nup,size_t ndown,size_t orbitals)
		: basis1_(geometry.numberOfSites(),nup,orbitals),
		  basis2_(geometry.numberOfSites(),ndown,orbitals)
		{
			orbitals_ = orbitals;
//			std::cout<<"Basis1\n";
//			std::cout<<basis1_;
//			std::cout<<"Basis2\n";
//			std::cout<<basis2_;
		}
		
		BasisFeAsBasedSc(const GeometryType& geometry, size_t nup,size_t orbitals)
		: basis1_(geometry.numberOfSites(),nup,orbitals),
		  basis2_(geometry.numberOfSites(),nup,orbitals)
		{
			orbitals_ = orbitals;
			std::string s = "BasisFeBasedSc::ctor(...): obsolete. ";
			s+= "This probably means that you can't compute the Green function";
			s+= " with this model (sorry). It might be added in the future.\n";
			throw std::runtime_error(s.c_str());
		}
		

		static const WordType& bitmask(size_t i)
		{
			return BasisType::bitmask(i);
		}
		
		size_t dofs() const { return 2*orbitals_; }

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

		size_t getN(WordType ket,size_t site,size_t spin,size_t orb) const
		{
			return (spin==SPIN_UP) ? basis1_.getN(ket,site,orb) : basis2_.getN(ket,site,orb);
		}

		int getBraIndex(WordType ket1, WordType ket2,size_t what,size_t site,size_t spin,size_t orb) const
		{
			WordType bra  =0;
			bool b = getBra(bra,ket1,ket2,what,site,spin,orb);
			if (!b) return -1;
			return (spin==SPIN_UP) ? perfectIndex(bra,ket2) :
						 perfectIndex(ket1,bra);
		}

		int doSign(size_t i,size_t site,size_t sector) const
		{
			size_t y = i/basis1_.size();
			size_t x = i%basis1_.size();
			size_t spin = sector/orbitals_;
			size_t orb = (sector % orbitals_);
			if (spin==SPIN_UP) {
				return basis1_.doSign(x,site,orb);
			}
			size_t c = basis1_.getN(x);
			int ret = 1;
			if (c&1) ret = FERMION_SIGN;
			return ret * basis2_.doSign(y,site,orb);
		}
		

		int doSign(WordType ket1,
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

		int doSignGf(WordType a, WordType b,size_t ind,size_t spin,size_t orb) const
		{
			if (spin==SPIN_UP) return basis1_.doSignGf(a,ind,orb);

			int s=(PsimagLite::BitManip::count(a) & 1) ? -1 : 1; // Parity of up
			int s2 = basis2_.doSignGf(b,ind,orb);

			return s*s2;
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
		
//		size_t electrons(size_t what) const
//		{
//			return (what==SPIN_UP) ? basis1_.electrons() : basis2_.electrons();
//		}

		bool hasNewParts(std::pair<size_t,size_t>& newParts,
				size_t type,
				size_t spin,
				const std::pair<size_t,size_t>& orbs) const
		{
			int newPart1=basis1_.electrons();
			int newPart2=basis2_.electrons();

			if (spin==SPIN_UP) newPart1 = basis1_.newPart(type,orbs.first);
			else newPart2 = basis2_.newPart(type,orbs.second);

			if (newPart1<0 || newPart2<0) return false;

			if (newPart1==0 && newPart2==0) return false;
			newParts.first = size_t(newPart1);
			newParts.second = size_t(newPart2);
			return true;
		}
		
	private:
		
		bool getBra(WordType& bra,
			    const WordType& ket1,
			    const WordType& ket2,
			    size_t what,
			    size_t site,
			    size_t spin,
			    size_t orb) const
		{
			return (spin==SPIN_UP) ? basis1_.getBra(bra,ket1,what,site,orb) :
						 basis2_.getBra(bra,ket2,what,site,orb);
		}

		BasisType basis1_,basis2_;
	}; // class BasisFeAsBasedSc

	template<typename GeometryType>
	size_t BasisFeAsBasedSc<GeometryType>::orbitals_=2;

} // namespace LanczosPlusPlus
#endif

