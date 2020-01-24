#pragma once
#include <include/glm.h>
#include <include/math.h>

namespace Tema
{
	class Camera
	{
		public:
			Camera()
			{
				position = glm::vec3(0, 2, 5);
				forward = glm::vec3(0, 0, -1);
				up		= glm::vec3(0, 1, 0);
				right	= glm::vec3(1, 0, 0);
			}

			Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				Set(position, center, up);
			}

			~Camera()
			{ }

			void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				this->position = position;
				forward = glm::normalize(center-position);
				right	= glm::cross(forward, up);
				this->up = glm::cross(right,forward);
			}

			void MoveForward(float distance)
			{
				glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
				position += dir * distance;
			}

			void TranslateForward(float distance)
			{
				position += glm::normalize(forward) * distance;
			}

			void TranslateUpword(float distance)
			{
				position += glm::normalize(up) * distance;
			}

			void TranslateRight(float distance)
			{
				position += glm::normalize(right) * distance;
			}

			void RotateFirstPerson_OX(float angle)
			{
				glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1), angle, right);

				forward = glm::normalize(rotationMatrix * forward);
				up = glm::cross(right, forward); 
			}

			void RotateFirstPerson_OY(float angle)
			{
				glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));

				forward = glm::normalize(rotationMatrix * forward);
				right = glm::normalize(rotationMatrix * right);
				up = glm::cross(right, forward); 
			}

			void RotateFirstPerson_OZ(float angle)
			{
				glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));

				right = glm::normalize(rotationMatrix * right);
				up = glm::normalize(rotationMatrix * up);
			}

			void RotateThirdPerson_OX(float angle, glm::vec3 airplaneCenter)
			{
				float distanceToTarget = distance(airplaneCenter, position);

				TranslateForward(distanceToTarget);
				RotateFirstPerson_OX(angle);
				TranslateForward(-distanceToTarget);
			}

			void RotateThirdPerson_OY(float angle, glm::vec3 airplaneCenter)
			{
				float distanceToTarget = distance(airplaneCenter, position);
				
				TranslateForward(distanceToTarget);
				RotateFirstPerson_OY(angle);
				TranslateForward(-distanceToTarget);
			}

			void RotateThirdPerson_OZ(float angle, glm::vec3 airplaneCenter)
			{
				float distanceToTarget = distance(airplaneCenter, position);

				TranslateForward(distanceToTarget);
				RotateFirstPerson_OZ(angle);
				TranslateForward(-distanceToTarget);
			}

			glm::mat4 GetViewMatrix()
			{
				return glm::lookAt(position, position + forward, up);
			}

			glm::vec3 GetEyePosition()
			{
				return position;
			}

			glm::vec3 GetForwardVector()
			{
				return forward;
			}

			glm::vec3 GetRightVector()
			{
				return right;
			}

		public:
			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 right;
			glm::vec3 up;
			float distanceToTarget;
		};
}