#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

// ����������˶��ļ������ܵ�ѡ��
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Ĭ��ֵ
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 80.0f;
const float ZOOM_RANGE = 20.0f;
const float ZOOM_SPEED = 2.0f;
const float STADIA = 1000.0f;

// ���������
// ��������ͼ�����Ӧ��ŷ���ǡ������;���������OpenGL
class Camera {
public:
    // ���������
    glm::vec3 Position;
    glm::vec3 Forward;
    glm::vec3 Up;
    glm::vec3 Horizontal;
    glm::vec3 WorldUp;
    // ŷ����
    float Yaw;	// ƫ����
    float Pitch; // ��б��
    // �����ѡ��
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // ���캯��
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH)
        : Forward(glm::vec3(0.0f, 0.0f, -1.0f))
        , MovementSpeed(SPEED)
        , MouseSensitivity(SENSITIVITY)
        , Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // ���캯���ͱ���ֵ
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw,
        float pitch)
        : Forward(glm::vec3(0.0f, 0.0f, -1.0f))
        , MovementSpeed(SPEED)
        , MouseSensitivity(SENSITIVITY)
        , Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // ������ͼ����
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Forward, Up);
    }

    // ����ͶӰ����
    glm::mat4 GetProjMatrix(float aspect)
    {
        return glm::perspective(glm::radians(Zoom), aspect, 0.1f, STADIA);
    }

    // ͨ�����̸ı����λ��
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)				//�����ǰ
            Position += Forward * velocity;
        if (direction == BACKWARD)				//������
            Position -= Forward * velocity;
        if (direction == LEFT)							//�������
            Position -= Horizontal * velocity;
        if (direction == RIGHT)						//�������
            Position += Horizontal * velocity;
        if (direction == UP)								//�������
            Position += WorldUp * velocity;
        if (direction == DOWN)						//����½�
            Position -= WorldUp * velocity;
    }

    // ����ƶ��ı�yaw��pitch����ֵ
    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw -= xoffset;
        Pitch += yoffset;

        // �������Ƕȱ仯�����ӽǷ�ת
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // ʹ��ŷ���Ǹ��� Forward��Up��Horizontal ����
        updateCameraVectors();
    }

    // �����������������
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= ZOOM)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= ZOOM)
            Zoom = ZOOM;
    }

    void FixView(glm::vec3 position, float yaw)
    {
        Position = position;
        Yaw = yaw;
        Pitch = 0.0f;
        updateCameraVectors();
    }

    void ZoomIn()
    {
        if (Zoom >= ZOOM - ZOOM_RANGE)
            Zoom -= ZOOM_SPEED;
    }

    void ZoomOut()
    {
        if (Zoom <= ZOOM + ZOOM_RANGE)
            Zoom += ZOOM_SPEED;
    }

    void ZoomRecover()
    {
        if (Zoom < ZOOM)
            Zoom += ZOOM_SPEED / 2;
        if (Zoom > ZOOM)
            Zoom -= ZOOM_SPEED / 2;
    }

private:
    // ͨ�������ŷ���Ǽ��� Forward��Horizontal��Up ����
    void updateCameraVectors()
    {
        // ���� Forward ����
        glm::vec3 front;
        front.x = -sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Forward = glm::normalize(front);
        // ���� Horizontal �� Up ����
        Horizontal = glm::normalize(glm::cross(Forward, WorldUp));
        Up = glm::normalize(glm::cross(Horizontal, Forward));
    }
};
#endif