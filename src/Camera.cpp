//
// Created by Connor on 11/14/2019.
//

#include "Camera.h"

#define CAMERA_SPEED_MULTIPLIER 2.5f
#define DEFAULT_SENSITIVITY 0.1f
#define DEFAULT_JUMP_SPEED 1.0f
#define DEFAULT_ZOOM 45.0f
#define DEFAULT_STICK_SENSITIVITY 0.00007f
#define GROUND_Y_VALUE 0.0f

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float64 yaw, float64 pitch)
: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(DEFAULT_SENSITIVITY), Zoom(DEFAULT_ZOOM)
{
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::UpdateViewMatrix(float32 deltaTime, float32 movementSpeed, bool groundedMovement)
{
  changePositioning(deltaTime, movementSpeed, groundedMovement);
  return lookAt();
}

glm::mat4 Camera::lookAt()
{
  glm::vec3 target = Position + Front;

  // Calculate cameraDirection
  glm::vec3 zaxis = glm::normalize(Position - target);
  // Get positive right axis vector
  glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(Up), zaxis));
  // Calculate camera up vector
  glm::vec3 yaxis = glm::cross(zaxis, xaxis);

  // In glm we access elements as mat[col][row] due to column-major layout
  glm::mat4 translation = glm::mat4(
          1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          -Position.x, -Position.y, -Position.z, 1.0f);

  glm::mat4 rotation = glm::mat4(
          xaxis.x, yaxis.x, zaxis.x, 0.0f,
          xaxis.y, yaxis.y, zaxis.y, 0.0f,
          xaxis.z, yaxis.z, zaxis.z, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f);

  // Return lookAt matrix as combination of translation and rotation matrix
  return rotation * translation; // Remember to read from right to left (first translation then rotation)
}

void Camera::changePositioning(float32 deltaTime, float32 movementSpeed, bool groundedMovement)
{
  if(groundedMovement) { deltaPosition.y = 0.0f; }

  if (jumping && groundedMovement)
  {
    jumpVal += DEFAULT_JUMP_SPEED * deltaTime * 6;
    float32 verticalOffset = sin(jumpVal) / 1.3f;
    if (verticalOffset < GROUND_Y_VALUE)
    {
      Position.y = GROUND_Y_VALUE;
      jumpVal = 0.0f;
      jumping = false;
    } else
    {
      Position.y = verticalOffset;
    }
  }

  // multiplying a vec3(0,0,0) by small fractions may lead to NAN values
  if (deltaPosition.x != 0.0f || deltaPosition.y != 0.0f || deltaPosition.z != 0.0f)
  {
    // normalizing the deltaPosition helps:
    // - accommodate for slower movement when looking up or down
    //      due to the front.xz values creating a < 1 magnitude vector
    float32 velocity = movementSpeed * CAMERA_SPEED_MULTIPLIER * deltaTime;
    Position += glm::normalize(deltaPosition) * velocity;
  }
  deltaPosition = glm::vec3(0.0f);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessInput(CameraMovement direction)
{
  switch (direction)
  {
    case FORWARD:
      deltaPosition += Front;
      break;
    case BACKWARD:
      deltaPosition -= Front;
      break;
    case LEFT:
      deltaPosition -= Right;
      break;
    case RIGHT:
      deltaPosition += Right;
      break;
    case JUMP:
      jumping = true;
      break;
  }
}

void Camera::ProcessLeftAnalog(int16 stickX, int16 stickY)
{
  if (stickY > 0) ProcessInput(FORWARD);
  else if (stickY < 0) ProcessInput(BACKWARD);
  if (stickX > 0) ProcessInput(RIGHT);
  else if (stickX < 0) ProcessInput(LEFT);
}


// TODO: use MouseCoord?
// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float64 xOffset, float64 yOffset, bool invertedY)
{
  xOffset *= MouseSensitivity;
  yOffset *= MouseSensitivity;

  Yaw += xOffset;
  Pitch = invertedY ? Pitch - yOffset : Pitch + yOffset;

  // Constrain pitch value
  if (Pitch > 89.0f) { Pitch = 89.0f; };
  if (Pitch < -89.0f) { Pitch = -89.0f; };

  // Update Front, Right and Up Vectors using the updated Eular angles
  updateCameraVectors();
}

void Camera::ProcessRightAnalog(int16 stickX, int16 stickY)
{
  Yaw += (float32)stickX * DEFAULT_STICK_SENSITIVITY;;
  Pitch += (float32)stickY * DEFAULT_STICK_SENSITIVITY;

  // Constrain pitch value
  if (Pitch > 89.0f) { Pitch = 89.0f; };
  if (Pitch < -89.0f) { Pitch = -89.0f; };

  // Update Front, Right and Up Vectors using the updated Eular angles
  updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float32 yoffset)
{
  Zoom -= yoffset;
  if (Zoom < 1.0f) Zoom = 1.0f;
  if (Zoom > 45.0f) Zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
  // Calculate the new Front vector
  glm::vec3 front;
  float32 pitch = (float32)Pitch;
  float32 yaw = (float32)Yaw;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  Front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  Up = glm::normalize(glm::cross(Right, Front));
}