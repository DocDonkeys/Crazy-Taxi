#include "PhysBody3D.h"
#include "glmath.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
PhysBody3D::PhysBody3D(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}


//Function to set rbody as sensor
void PhysBody3D::SetAsSensor(bool is_sensor)
{
	if (this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if (is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() &~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

vec3 PhysBody3D::GetPosition()
{
	vec3 pos;
	Cube auxCube(1,1,1);
	this->GetTransform(&auxCube.transform);



	pos.x = auxCube.transform[12];
	pos.y = auxCube.transform[13];
	pos.z = auxCube.transform[14];


	return pos;
}

void PhysBody3D::SetRotation(vec3 axis, float angle)
{
	/*btQuaternion q;
	q.setRotation(axis, angle);
	float matrix;
	GetTransform(&matrix);
	btTransform t = matrix;
	t.setRotation(q);*/

	//body->setWorldTransform(t);
}
