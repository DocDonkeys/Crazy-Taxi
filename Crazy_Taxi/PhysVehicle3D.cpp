#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color.Set(0.8f, 0.8f, 0.8f);

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);	
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());
	
	chassis.color.Set(0.8f, 0.8f, 0.0f);
	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();


	Cube sign (info.sign_size.x, info.sign_size.y, info.sign_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&sign.transform);
	btVector3 sign_offset(info.sign_offset.x, info.sign_offset.y, info.sign_offset.z);
	sign_offset = sign_offset.rotate(q.getAxis(), q.getAngle());

	sign.color.Set(0.2f, 0.2f, 0.2f);
	sign.transform.M[12] += sign_offset.getX();
	sign.transform.M[13] += sign_offset.getY();
	sign.transform.M[14] += sign_offset.getZ();



	chassis.Render();
	sign.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

vec3 PhysVehicle3D::GetPosition()
{
	vec3 pos;

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	//chassis.transform.M[12] += offset.getX();
	//chassis.transform.M[13] += offset.getY();
	//chassis.transform.M[14] += offset.getZ();

	pos.x = chassis.transform.M[12];
	pos.y = chassis.transform.M[13];
	pos.z = chassis.transform.M[14];

	return pos;
}

vec3 PhysVehicle3D::GetForwardVec()
{
	vec3 vec;
	btVector3 btVec = vehicle->getForwardVector();

	/*vec.x = int(btVec.x);
	vec.y = int(btVec.y);
	vec.z = int(btVec.z);*/

	vec.x = btVec.getX();
	vec.y = btVec.getY();
	vec.z = btVec.getZ();
	return vec;
}
