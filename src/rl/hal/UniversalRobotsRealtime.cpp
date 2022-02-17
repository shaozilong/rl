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

#include <array>

#include "DeviceException.h"
#include "UniversalRobotsRealtime.h"

namespace rl
{
	namespace hal
	{
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_RESET;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_SHUTTING_DOWN;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_PART_D_CALIBRATION;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_BACKDRIVE;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_POWER_OFF;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_NOT_RESPONDING;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_MOTOR_INITIALISATION;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_BOOTING;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_PART_D_CALIBRATION_ERROR;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_BOOTLOADER;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_CALIBRATION;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_VIOLATION;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_FAULT;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_RUNNING;
		constexpr UniversalRobotsRealtime::JointMode UniversalRobotsRealtime::JOINT_MODE_IDLE;
		
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_STOPPING;
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_STOPPED;
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_PLAYING;
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_PAUSING;
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_PAUSED;
		constexpr UniversalRobotsRealtime::ProgramState UniversalRobotsRealtime::PROGRAM_STATE_RESUMING;
		
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_NO_CONTROLLER;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_DISCONNECTED;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_CONFIRM_SAFETY;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_BOOTING;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_POWER_OFF;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_POWER_ON;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_IDLE;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_BACKDRIVE;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_RUNNING;
		constexpr UniversalRobotsRealtime::RobotMode UniversalRobotsRealtime::ROBOT_MODE_UPDATING_FIRMWARE;
		
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_NORMAL;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_REDUCED;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_PROTECTIVE_STOP;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_RECOVERY;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_SAFEGUARD_STOP;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_SYSTEM_EMERGENCY_STOP;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_ROBOT_EMERGENCY_STOP;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_VIOLATION;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_FAULT;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_VALIDATE_JOINT_ID;
		constexpr UniversalRobotsRealtime::SafetyMode UniversalRobotsRealtime::SAFETY_MODE_UNDEFINED_SAFETY_MODE;
		
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_NORMAL;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_REDUCED;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_PROTECTIVE_STOP;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_RECOVERY;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_SAFEGUARD_STOP;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_SYSTEM_EMERGENCY_STOP;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_ROBOT_EMERGENCY_STOP;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_VIOLATION;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_FAULT;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_VALIDATE_JOINT_ID;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_UNDEFINED_SAFETY_MODE;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_AUTOMATIC_MODE_SAFEGUARD_STOP;
		constexpr UniversalRobotsRealtime::SafetyStatus UniversalRobotsRealtime::SAFETY_STATUS_SYSTEM_THREE_POSITION_ENABLING_STOP;
		
		UniversalRobotsRealtime::UniversalRobotsRealtime(const ::std::string& address) :
			AxisController(6),
			DigitalInput(),
			DigitalOutput(),
			CartesianForceSensor(6),
			CartesianPositionSensor(6),
			CartesianVelocitySensor(6),
			CyclicDevice(::std::chrono::milliseconds(8)),
			DigitalInputReader(),
			DigitalOutputReader(),
			JointCurrentSensor(6),
			JointPositionSensor(6),
			JointVelocitySensor(6),
			in(),
			socket(Socket::Tcp(Socket::Address::Ipv4(address, 30003)))
		{
		}
		
		UniversalRobotsRealtime::~UniversalRobotsRealtime()
		{
			if (this->isRunning())
			{
				this->stop();
			}
			
			if (this->isConnected())
			{
				this->close();
			}
		}
		
		void
		UniversalRobotsRealtime::close()
		{
			this->socket.close();
			this->setConnected(false);
		}
		
		void
		UniversalRobotsRealtime::doScript(const ::std::string& script)
		{
			this->socket.send(script.c_str(), script.size());
		}
		
		::rl::math::ForceVector
		UniversalRobotsRealtime::getCartesianForce() const
		{
			::rl::math::ForceVector f;
			f.force().x() = this->in.tcpForce[0];
			f.force().y() = this->in.tcpForce[1];
			f.force().z() = this->in.tcpForce[2];
			f.moment().x() = this->in.tcpForce[3];
			f.moment().y() = this->in.tcpForce[4];
			f.moment().z() = this->in.tcpForce[5];
			return f;
		}
		
		::rl::math::Transform
		UniversalRobotsRealtime::getCartesianPosition() const
		{
			::rl::math::Transform x;
			x.setIdentity();
			
			::rl::math::Vector3 orientation(this->in.toolVectorActual[3], this->in.toolVectorActual[4], this->in.toolVectorActual[5]);
			::rl::math::Real norm = orientation.norm();
			
			if (::std::abs(norm) <= ::std::numeric_limits<::rl::math::Real>::epsilon())
			{
				x.linear().setIdentity();
			}
			else
			{
				x.linear() = ::rl::math::AngleAxis(norm, orientation.normalized()).matrix();
			}
			
			x.translation().x() = this->in.toolVectorActual[0];
			x.translation().y() = this->in.toolVectorActual[1];
			x.translation().z() = this->in.toolVectorActual[2];
			
			return x;
		}
		
		::rl::math::MotionVector
		UniversalRobotsRealtime::getCartesianVelocity() const
		{
			::rl::math::MotionVector v;
			v.linear().x() = this->in.tcpSpeedActual[0];
			v.linear().y() = this->in.tcpSpeedActual[1];
			v.linear().z() = this->in.tcpSpeedActual[2];
			v.angular().x() = this->in.tcpSpeedActual[3];
			v.angular().y() = this->in.tcpSpeedActual[4];
			v.angular().z() = this->in.tcpSpeedActual[5];
			return v;
		}
		
		::boost::dynamic_bitset<>
		UniversalRobotsRealtime::getDigitalInput() const
		{
			return ::boost::dynamic_bitset<>(64, this->in.digitalInputBits);
		}
		
		bool
		UniversalRobotsRealtime::getDigitalInput(const ::std::size_t& i) const
		{
			return (this->in.digitalInputBits & (1ULL << i)) ? true : false;
		}
		
		::std::size_t
		UniversalRobotsRealtime::getDigitalInputCount() const
		{
			return 64;
		}
		
		::boost::dynamic_bitset<>
		UniversalRobotsRealtime::getDigitalOutput() const
		{
			return ::boost::dynamic_bitset<>(64, this->in.digitalOutputs);
		}
		
		bool
		UniversalRobotsRealtime::getDigitalOutput(const ::std::size_t& i) const
		{
			return (this->in.digitalOutputs & (1ULL << i)) ? true : false;
		}
		
		::std::size_t
		UniversalRobotsRealtime::getDigitalOutputCount() const
		{
			return 64;
		}
		
		::rl::math::Vector
		UniversalRobotsRealtime::getJointCurrent() const
		{
			::rl::math::Vector i(this->getDof());
			
			for (::std::size_t j = 0; j < 6; ++j)
			{
				i(j) = this->in.iActual[j];
			}
			
			return i;
		}
		
		UniversalRobotsRealtime::JointMode
		UniversalRobotsRealtime::getJointMode(const ::std::size_t& i) const
		{
			return static_cast<JointMode>(static_cast<int>(this->in.jointModes[i]));
		}
		
		::rl::math::Vector
		UniversalRobotsRealtime::getJointPosition() const
		{
			::rl::math::Vector q(this->getDof());
			
			for (::std::size_t i = 0; i < 6; ++i)
			{
				q(i) = this->in.qActual[i];
			}
			
			return q;
		}
		
		::rl::math::Vector
		UniversalRobotsRealtime::getJointVelocity() const
		{
			::rl::math::Vector qd(this->getDof());
			
			for (::std::size_t i = 0; i < 6; ++i)
			{
				qd(i) = this->in.qdActual[i];
			}
			
			return qd;
		}
		
		UniversalRobotsRealtime::ProgramState
		UniversalRobotsRealtime::getProgramState() const
		{
			return static_cast<ProgramState>(static_cast<int>(this->in.programState));
		}
		
		UniversalRobotsRealtime::RobotMode
		UniversalRobotsRealtime::getRobotMode() const
		{
			return static_cast<RobotMode>(static_cast<int>(this->in.robotMode));
		}
		
		UniversalRobotsRealtime::SafetyMode
		UniversalRobotsRealtime::getSafetyMode() const
		{
			return static_cast<SafetyMode>(static_cast<int>(this->in.safetyMode));
		}
		
		UniversalRobotsRealtime::SafetyStatus
		UniversalRobotsRealtime::getSafetyStatus() const
		{
			return static_cast<SafetyStatus>(static_cast<int>(this->in.safetyStatus));
		}
		
		void
		UniversalRobotsRealtime::open()
		{
			this->socket.open();
			this->socket.connect();
			this->socket.setOption(Socket::Option::nodelay, 1);
			this->setConnected(true);
		}
		
		void
		UniversalRobotsRealtime::start()
		{
			this->setRunning(true);
		}
		
		void
		UniversalRobotsRealtime::step()
		{
			::std::array<::std::uint8_t, 1220> buffer;
			
			this->socket.recv(buffer.data(), sizeof(this->in.messageSize));
#if !defined(__APPLE__) && !defined(__QNX__) && !defined(WIN32) && !defined(__CYGWIN__)
			this->socket.setOption(Socket::Option::quickack, 1);
#endif // !__APPLE__ && !__QNX__ && !WIN32 && !__CYGWIN__
			
			::std::uint8_t* ptr = buffer.data();
			this->in.unserialize(ptr, this->in.messageSize);
			
			switch (this->in.messageSize)
			{
			case 756:
			case 764:
			case 812:
			case 1044:
			case 1060:
			case 1108:
			case 1116:
			case 1140:
			case 1220:
				this->socket.recv(ptr, this->in.messageSize - sizeof(this->in.messageSize));
#if !defined(__APPLE__) && !defined(__QNX__) && !defined(WIN32) && !defined(__CYGWIN__)
				this->socket.setOption(Socket::Option::quickack, 1);
#endif // !__APPLE__ && !__QNX__ && !WIN32 && !__CYGWIN__
				this->in.unserialize(ptr);
				break;
			default:
				throw DeviceException("UniversalRobotsRealtime::step() - Incorrect message size " + ::std::to_string(this->in.messageSize));
				break;
			}
		}
		
		void
		UniversalRobotsRealtime::stop()
		{
			this->setRunning(false);
		}
		
		void
		UniversalRobotsRealtime::Message::unserialize(::std::uint8_t* ptr)
		{
			this->unserialize(ptr, this->time);
			this->unserialize(ptr, this->qTarget);
			this->unserialize(ptr, this->qdTarget);
			this->unserialize(ptr, this->qddTarget);
			this->unserialize(ptr, this->iTarget);
			this->unserialize(ptr, this->mTarget);
			this->unserialize(ptr, this->qActual);
			this->unserialize(ptr, this->qdActual);
			this->unserialize(ptr, this->iActual);
			
			switch (this->messageSize)
			{
			case 756:
				ptr += 3 * sizeof(double);
				ptr += 15 * sizeof(double);
				this->unserialize(ptr, this->tcpForce);
				this->unserialize(ptr, this->toolVectorActual);
				this->unserialize(ptr, this->tcpSpeedActual);
				break;
			case 764:
			case 812:
				this->unserialize(ptr, this->toolAccelerometerValues);
				ptr += 15 * sizeof(double);
				this->unserialize(ptr, this->tcpForce);
				this->unserialize(ptr, this->toolVectorActual);
				this->unserialize(ptr, this->tcpSpeedActual);
				break;
			case 1044:
			case 1060:
			case 1108:
			case 1116:
			case 1140:
			case 1220:
				this->unserialize(ptr, this->iControl);
				this->unserialize(ptr, this->toolVectorActual);
				this->unserialize(ptr, this->tcpSpeedActual);
				this->unserialize(ptr, this->tcpForce);
				this->unserialize(ptr, this->toolVectorTarget);
				this->unserialize(ptr, this->tcpSpeedTarget);
				break;
			default:
				break;
			}
			
			this->unserialize(ptr, this->digitalInputBits);
			this->unserialize(ptr, this->motorTemperatures);
			this->unserialize(ptr, this->controllerTimer);
			this->unserialize(ptr, this->testValue);
			
			if (this->messageSize < 764)
			{
				return;
			}
			
			this->unserialize(ptr, this->robotMode);
			
			if (this->messageSize < 812)
			{
				return;
			}
			
			this->unserialize(ptr, this->jointModes);
			
			if (this->messageSize < 1044)
			{
				return;
			}
			
			this->unserialize(ptr, this->safetyMode);
			ptr += 6 * sizeof(double);
			this->unserialize(ptr, this->toolAccelerometerValues);
			ptr += 6 * sizeof(double);
			this->unserialize(ptr, this->speedScaling);
			this->unserialize(ptr, this->linearMomentumNorm);
			ptr += 1 * sizeof(double);
			ptr += 1 * sizeof(double);
			this->unserialize(ptr, this->vMain);
			this->unserialize(ptr, this->vRobot);
			this->unserialize(ptr, this->iRobot);
			this->unserialize(ptr, this->vActual);
			
			if (this->messageSize < 1060)
			{
				return;
			}
			
			this->unserialize(ptr, this->digitalOutputs);
			this->unserialize(ptr, this->programState);
			
			if (this->messageSize < 1108)
			{
				return;
			}
			
			this->unserialize(ptr, this->elbowPosition);
			this->unserialize(ptr, this->elbowVelocity);
			
			if (this->messageSize < 1116)
			{
				return;
			}
			
			this->unserialize(ptr, this->safetyStatus);
			
			if (this->messageSize < 1140)
			{
				return;
			}
			
			ptr += 3 * sizeof(double);
			
			if (this->messageSize < 1220)
			{
				return;
			}
			
			this->unserialize(ptr, this->payloadMass);
			this->unserialize(ptr, this->payloadCog);
			this->unserialize(ptr, this->payloadInertia);
		}
		
		template<>
		void
		UniversalRobotsRealtime::Message::unserialize(::std::uint8_t*& ptr, ::std::int64_t& t)
		{
			::std::memcpy(&t, ptr, sizeof(t));
			ptr += sizeof(t);
		}
	}
}
