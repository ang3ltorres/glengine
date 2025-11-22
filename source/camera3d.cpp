#include "camera3d.hpp"

using namespace graphics;
using namespace glm;

Camera3D::Camera3D(float width, float height)
: position(0.0f), yaw(0.0f), pitch(0.0f), roll(0.0f), fov(80.0f), aspect(width / height), nearClip(0.1f), farClip(1000.0f)
{
	updateViewProjection();
}

void Camera3D::move(const vec3& pos)
{
	position = pos;
	updateViewProjection();
}

void Camera3D::rotate(float yaw, float pitch, float roll)
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
	updateViewProjection();
}

vec3 Camera3D::forward() const
{
	float cy = cos(radians(yaw));
	float sy = sin(radians(yaw));
	float cp = cos(radians(pitch));
	float sp = sin(radians(pitch));

	return normalize(vec3(cy * cp, sp, sy * cp));
}

vec3 Camera3D::right() const
{
	return normalize(cross(forward(), vec3(0, 1, 0)));
}

vec3 Camera3D::up() const
{
	return normalize(cross(right(), forward()));
}

void Camera3D::updateView()
{
	vec3 f = forward();
	vec3 upVec = up();

	view = lookAt(position, position + f, upVec);
}

void Camera3D::updateProjection()
{
	projection = perspective(radians(fov), aspect, nearClip, farClip);
}

void Camera3D::updateViewProjection()
{
	updateView();
	updateProjection();
	viewProjection = projection * view;
}
