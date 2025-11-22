#include "camera2d.hpp"

using namespace graphics;
using namespace glm;

Camera2D::Camera2D(unsigned int width, unsigned int height)
: position(0.0f, 0.0f), zoom(1.0f), width(width), height(height), view(0.0f), projection(0.0f), viewProjection(0.0f)
{
	updateViewProjectionMatrix();
}

void Camera2D::setPosition(const vec2 &position)
{
	this->position = position;
	updateViewProjectionMatrix();
}

void Camera2D::move(const vec2 &delta)
{
	position -= delta;
	updateViewProjectionMatrix();
}

void Camera2D::setZoom(float zoom)
{
	this->zoom = zoom;
	updateViewProjectionMatrix();
}

void Camera2D::updateViewProjectionMatrix()
{
	updateView();
	updateProjection();
	viewProjection = projection * view;;
}

void Camera2D::updateView()
{
	view = translate(mat4(1.0f), vec3(position, 0.0f));
}

void Camera2D::updateProjection()
{
	float left   = 0.0f;
	float right  = width / zoom;
	float bottom = 0.0f;
	float top    = height / zoom;

	projection = ortho(left, right, top, bottom, -1.0f, 1.0f);
}
