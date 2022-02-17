//
// Copyright (c) 2009, Markus Rickert
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef RL_KIN_PUMA_H
#define RL_KIN_PUMA_H

#include "Kinematics.h"

namespace rl
{
	namespace kin
	{
		/**
		 * TRR base and TRT wrist.
		 */
		class RL_KIN_EXPORT Puma : public Kinematics
		{
		public:
			enum class Arm
			{
				left = -1,
				right = 1
			};
			
			RL_KIN_DEPRECATED static constexpr Arm ARM_LEFT = Arm::left;
			RL_KIN_DEPRECATED static constexpr Arm ARM_RIGHT = Arm::right;
			
			enum class Elbow
			{
				above = 1,
				below = -1
			};
			
			RL_KIN_DEPRECATED static constexpr Elbow ELBOW_ABOVE = Elbow::above;
			RL_KIN_DEPRECATED static constexpr Elbow ELBOW_BELOW = Elbow::below;
			
			enum class Wrist
			{
				flip = 1,
				nonflip = -1
			};
			
			RL_KIN_DEPRECATED static constexpr Wrist WRIST_FLIP = Wrist::flip;
			RL_KIN_DEPRECATED static constexpr Wrist WRIST_NONFLIP = Wrist::nonflip;
			
			Puma();
			
			virtual ~Puma();
			
			Arm getArm() const;
			
			Elbow getElbow() const;
			
			Wrist getWrist() const;
			
			bool inversePosition(const ::rl::math::Transform& x, ::rl::math::Vector& q, const bool& ignoreLimits = false);
			
			bool isSingular() const;
			
			void parameters(const ::rl::math::Vector& q, Arm& arm, Elbow& elbow, Wrist& wrist) const;
			
			void setArm(const Arm& arm);
			
			void setElbow(const Elbow& elbow);
			
			void setWrist(const Wrist& wrist);
			
		protected:
			void update();
			
		private:
			template<typename T> T atan2(const T& y, const T& x) const
			{
				T a = ::std::atan2(y, x);
				return (::std::abs(a) <= static_cast<::rl::math::Real>(1.0e-6)) ? 0 : a;
			}
			
			template<typename T> T cos(const T& x) const
			{
				T c = ::std::cos(x);
				return (::std::abs(c) <= static_cast<::rl::math::Real>(1.0e-6)) ? 0 : c;
			}
			
			template<typename T> T sin(const T& x) const
			{
				T s = ::std::sin(x);
				return (::std::abs(s) <= static_cast<::rl::math::Real>(1.0e-6)) ? 0 : s;
			}
			
			Arm arm;
			
			Elbow elbow;
			
			Wrist wrist;
		};
	}
}

#endif // RL_KIN_PUMA_H
