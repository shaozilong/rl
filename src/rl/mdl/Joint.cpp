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

#include <rl/std/algorithm.h>

#include "Frame.h"
#include "Joint.h"

namespace rl
{
	namespace mdl
	{
		Joint::Joint(const ::std::size_t& dofPosition, const ::std::size_t& dofVelocity) :
			a(::rl::math::MotionVector::Zero()),
			c(::rl::math::MotionVector::Zero()),
			D(::rl::math::Matrix::Zero(dofVelocity, dofVelocity)),
			max(::rl::math::Vector::Constant(dofPosition, ::std::numeric_limits<::rl::math::Real>::max())),
			min(::rl::math::Vector::Constant(dofPosition, -::std::numeric_limits<::rl::math::Real>::max())),
			offset(::rl::math::Vector::Zero(dofPosition)),
			q(::rl::math::Vector::Zero(dofPosition)),
			qUnits(dofPosition),
			qd(::rl::math::Vector::Zero(dofVelocity)),
			qdUnits(dofVelocity),
			qdd(::rl::math::Vector::Zero(dofVelocity)),
			qddUnits(dofVelocity),
			S(::rl::math::Matrix::Zero(6, dofVelocity)),
			speed(::rl::math::Vector::Constant(dofVelocity, ::std::numeric_limits<::rl::math::Real>::max())),
			speedUnits(dofVelocity),
			tau(::rl::math::Vector::Zero(dofVelocity)),
			tauUnits(dofVelocity),
			u(::rl::math::Vector::Zero(dofVelocity)),
			U(::rl::math::Matrix::Zero(6, dofVelocity)),
			v(::rl::math::MotionVector::Zero()),
			wraparound(::Eigen::Matrix<bool, ::Eigen::Dynamic, 1>::Constant(dofPosition, false))
		{
		}
		
		Joint::~Joint()
		{
		}
		
		void
		Joint::clamp(::rl::math::VectorRef q) const
		{
			for (::std::ptrdiff_t i = 0; i < q.size(); ++i)
			{
				if (this->wraparound(i))
				{
					::rl::math::Real range = ::std::abs(this->max(i) - this->min(i));
					
					while (q(i) > this->max(i))
					{
						q(i) -= range;
					}
					
					while (q(i) < this->min(i))
					{
						q(i) += range;
					}
				}
				else
				{
					q(i) = ::rl::std17::clamp(q(i), this->min(i), this->max(i));
				}
			}
		}
		
		::rl::math::Real
		Joint::distance(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2) const
		{
			return (q2 - q1).norm();
		}
		
		void
		Joint::forwardAcceleration()
		{
			// X * a + aj + cj + v x vj
			this->out->a = this->x * this->in->a + this->a + this->c + this->out->v.cross(this->v);
		}
		
		void
		Joint::forwardDynamics1()
		{
			this->forwardVelocity();
			// cj + v x vj
			this->out->c = this->c + this->out->v.cross(this->v);
		}
		
		void
		Joint::forwardDynamics2()
		{
			// I^A * S
			this->U = this->out->iA.matrix() * this->S;
			// S^T * U
			this->D = this->S.transpose() * this->U;
			// tau - S^T * p^A
			this->u = this->tau - this->S.transpose() * this->out->pA.matrix();
			// I^A - U * D^-1 * U^T
			::rl::math::ArticulatedBodyInertia ia(this->out->iA - ::rl::math::ArticulatedBodyInertia(this->U * this->D.inverse() * this->U.transpose()));
			// p^A + I^a * c + U * D^-1 * u
			::rl::math::ForceVector pa(this->out->pA + ia * this->out->c + ::rl::math::ForceVector(this->U * this->D.inverse() * this->u));
			// I^A + X^* * I^a * X
			this->in->iA = this->in->iA + this->x / ia;
			// p^A + X^* * p^a
			this->in->pA = this->in->pA + this->x / pa;
		}
		
		void
		Joint::forwardDynamics3()
		{
			// X * a + c
			::rl::math::MotionVector a(this->x * this->in->a + this->out->c);
			// D^-1 * (u - U^T * a')
			this->qdd = this->D.inverse() * (this->u - this->U.transpose() * a.matrix());
			// S * qdd
			this->a = this->S * this->qdd;
			// a' + S * qdd
			this->out->a = a + this->a;
		}
		
		void
		Joint::forwardVelocity()
		{
			// X * v + vj
			this->out->v = this->x * this->in->v + this->v;
		}
		
		void
		Joint::generatePositionGaussian(const ::rl::math::ConstVectorRef& rand, const ::rl::math::ConstVectorRef& mean, const ::rl::math::ConstVectorRef& sigma, ::rl::math::VectorRef q) const
		{
			for (::std::ptrdiff_t i = 0; i < q.size(); ++i)
			{
				q(i) = mean(i) + rand(i) * sigma(i);
			}
			
			this->clamp(q);
		}
		
		void
		Joint::generatePositionUniform(const ::rl::math::ConstVectorRef& rand, ::rl::math::VectorRef q) const
		{
			for (::std::ptrdiff_t i = 0; i < q.size(); ++i)
			{
				q(i) = this->min(i) + rand(i) * (this->max(i) - this->min(i));
			}
		}
		
		void
		Joint::generatePositionUniform(const ::rl::math::ConstVectorRef& rand, const ::rl::math::ConstVectorRef& min, const ::rl::math::ConstVectorRef& max, ::rl::math::VectorRef q) const
		{
			for (::std::ptrdiff_t i = 0; i < q.size(); ++i)
			{
				q(i) = min(i) + rand(i) * (max(i) - min(i));
			}
		}
		
		const ::rl::math::Vector&
		Joint::getAcceleration() const
		{
			return this->qdd;
		}
		
		const ::Eigen::Matrix<::rl::math::Units, ::Eigen::Dynamic, 1>&
		Joint::getAccelerationUnits() const
		{
			return this->qddUnits;
		}
		
		::std::size_t
		Joint::getDof() const
		{
			return this->qd.size();
		}
		
		::std::size_t
		Joint::getDofPosition() const
		{
			return this->q.size();
		}
		
		const ::rl::math::Vector&
		Joint::getMaximum() const
		{
			return this->max;
		}
		
		const ::rl::math::Vector&
		Joint::getMinimum() const
		{
			return this->min;
		}
		
		const ::rl::math::Vector&
		Joint::getOffset() const
		{
			return this->offset;
		}
		
		const ::rl::math::Vector&
		Joint::getPosition() const
		{
			return this->q;
		}
		
		const ::Eigen::Matrix<::rl::math::Units, ::Eigen::Dynamic, 1>&
		Joint::getPositionUnits() const
		{
			return this->qUnits;
		}
		
		const ::rl::math::Vector&
		Joint::getSpeed() const
		{
			return this->speed;
		}
		
		const ::Eigen::Matrix<::rl::math::Units, ::Eigen::Dynamic, 1>&
		Joint::getSpeedUnits() const
		{
			return this->speedUnits;
		}
		
		const ::rl::math::Vector&
		Joint::getTorque() const
		{
			return this->tau;
		}
		
		const ::Eigen::Matrix<::rl::math::Units, ::Eigen::Dynamic, 1>&
		Joint::getTorqueUnits() const
		{
			return this->tauUnits;
		}
		
		const ::rl::math::Vector&
		Joint::getVelocity() const
		{
			return this->qd;
		}
		
		const ::Eigen::Matrix<::rl::math::Units, ::Eigen::Dynamic, 1>&
		Joint::getVelocityUnits() const
		{
			return this->qdUnits;
		}
		
		const ::Eigen::Matrix<bool, ::Eigen::Dynamic, 1>&
		Joint::getWraparound() const
		{
			return this->wraparound;
		}
		
		void
		Joint::interpolate(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2, const ::rl::math::Real& alpha, ::rl::math::VectorRef q) const
		{
			q = (1 - alpha) * q1 + alpha * q2;
		}
		
		void
		Joint::inverseForce()
		{
			// S^T * f
			this->tau = this->S.transpose() * this->out->f.matrix();
			
			// f + X * f
			this->in->f = this->in->f + this->x / this->out->f;
		}
		
		bool
		Joint::isValid(const ::rl::math::ConstVectorRef& q) const
		{
			for (::std::ptrdiff_t i = 0; i < q.size(); ++i)
			{
				if (q(i) < this->min(i) || q(i) > this->max(i))
				{
					return false;
				}
			}
			
			return true;
		}
		
		void
		Joint::normalize(::rl::math::VectorRef q) const
		{
		}
		
		void
		Joint::setAcceleration(const ::rl::math::ConstVectorRef& qdd)
		{
			this->qdd = qdd;
			
			// S * qdd
			this->a = this->S * this->qdd;
		}
		
		void
		Joint::setMaximum(const ::rl::math::ConstVectorRef& max)
		{
			this->max = max;
		}
		
		void
		Joint::setMinimum(const ::rl::math::ConstVectorRef& min)
		{
			this->min = min;
		}
		
		void
		Joint::setOffset(const ::rl::math::ConstVectorRef& offset)
		{
			this->offset = offset;
		}
		
		void
		Joint::setSpeed(const ::rl::math::ConstVectorRef& speed)
		{
			this->speed = speed;
		}
		
		void
		Joint::setTorque(const ::rl::math::ConstVectorRef& tau)
		{
			this->tau = tau;
		}
		
		void
		Joint::setVelocity(const ::rl::math::ConstVectorRef& qd)
		{
			this->qd = qd;
			
			// S * qd
			this->v = this->S * this->qd;
		}
		
		void
		Joint::setWraparound(const ::Eigen::Ref<const ::Eigen::Matrix<bool, ::Eigen::Dynamic, 1>>& wraparound)
		{
			this->wraparound = wraparound;
		}
		
		void
		Joint::step(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& dq, ::rl::math::VectorRef q2) const
		{
			q2 = q1 + dq;
			this->clamp(q2);
		}
		
		::rl::math::Real
		Joint::transformedDistance(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2) const
		{
			return (q2 - q1).squaredNorm();
		}
	}
}
