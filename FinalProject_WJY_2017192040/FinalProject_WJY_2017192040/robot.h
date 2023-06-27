#ifndef ROBOT_H
#define ROBOT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>

// ������ν�յĶ��ֶ���
enum Direction {
    ROBOT_FORWARD,
    ROBOT_BACKWARD,
    ROBOT_LEFT,
    ROBOT_RIGHT,
	ROBOT_UP,
	ROBOT_DOWN,
	ROBOT_GUP,
	ROBOT_GDOWN
};

class Robot {
public:
    glm::vec3 Position;
    glm::vec3 Front;
	glm::vec3 Height;
    float Yaw;
	float Pitch;
	float upup;

    // �洢��Yaw������ʵ�������ƶ�
    queue<float> HistoryYaw;
    int DelayFrameNum = 20;
    float DelayYaw;

    // ʵ�ֱ��ν�յ��ƶ�
    queue<glm::vec3> HistoryPosition;
    glm::vec3 DelayPosition;

    float MovementSpeed;
    float TurningSpeed;
	float UpingSpeed;
	float walk;
	float count;
	float headpo;
	float Gcount;
	float Grotate;

	//��ʼ��
    Robot(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
        : MovementSpeed(30.0f)
        , TurningSpeed(90.0f)
        , Yaw(0.0f)
		, Pitch(0.0f)
        , DelayYaw(0.0f)
		,UpingSpeed(10.0f)
		,walk(0.0f)
		,count(0.0f)
		,headpo(0.0f)
		,upup(0.0f)
		,Gcount(0.0f)
		,Grotate(0.0f)
    {
        Position = position;
        updateFront();
		updateHeight();
    }
	//λ��
    glm::vec3 getPosition()
    {
        return Position;
    }
	//��ת
	float getGrotate()
	{
		return Grotate;
	}
	//�����ת��������
	float Gup(float deltaTime)
	{
		if (Gcount < 180.0f)
		{
			Gcount += 6.0f* deltaTime;
		}
		return Gcount;
	}

	float Gdown(float deltaTime)
	{
		if (Gcount > -90.0f)
		{
			Gcount -= 6.0f* deltaTime;
		}
		
		return Gcount;
	}
	//�ƶ���������-ǰ���ͺ���
	float goCount(float deltaTime)
	{
		if(count > 360.0f)
		{
			count = 0.0f;
		}
		count += 10.0f* deltaTime;
		return count;
	}

	float backCount(float deltaTime)
	{
		if (count < -360.0f)
		{
			count = 0.0f;
		}
		count -= 10.0f* deltaTime;
		return count;
	}
	//ͷ���˶�����
	float goHead()
	{
		if (headpo > 90.0f)
		{
			headpo = 90.0f;
		}
		headpo += 10.0f;
		return headpo;
	}

	float backHead()
	{
		if (headpo < -90.0f)
		{
			headpo = -90.0f;
		}
		headpo -= 10.0f;
		return headpo;
	}

	float getHead()
	{
		return headpo;
	}


	float getWalk()
	{
		return walk;
	}
	//�����ƶ�����
    float getYaw()
    {
        return Yaw;
    }

	float getPitch()
	{
		return Pitch;
	}

    float getDelayYaw()
    {
        return DelayYaw;
    }
	//λ��ȷ������
    float getYawDif()
    {
        return Yaw - DelayYaw;
    }

    float getMidValYaw()
    {
        return (DelayYaw + Yaw) / 2;
    }

	float getHeight() 
	{
		return Position.y;
	}

    glm::vec3 getMidValPosition()
    {
        return (DelayPosition + Position) / 2.0f;
    }    

	glm::vec3 getMidValPosition_no_y() {
		glm::vec3 Position_no_y = Position;
		glm::vec3 DelayPosition_no_y = DelayPosition;
		Position_no_y[1] = 0.0f;
		DelayPosition_no_y[1] = 0.0f;
		return (DelayPosition_no_y + Position_no_y) / 2.0f;
	}

	glm::vec3 getMidValPosition_fixed_y(float level) {
		glm::vec3 Position_no_y = Position;
		glm::vec3 DelayPosition_no_y = DelayPosition;
		Position_no_y[1] = level;
		DelayPosition_no_y[1] = level;
		return (DelayPosition_no_y + Position_no_y) / 2.0f;
	}

    // ��ͼ����
    glm::mat4 GetViewMatrix(glm::vec3 cameraPosition)
    {
        return glm::lookAt(cameraPosition, Position, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // ���������롿��ǰ���������¶������㡿
	void ProcessKeyboard(Direction direction, float deltaTime)
	{
		//ǰ
		if (direction == ROBOT_FORWARD) {
			if (Position.y == 7.0f || Position.y == 0.05f)
			{
				Position += Front * MovementSpeed * deltaTime;
				if(Position.y == 7.00f)
					walk = 30*cos(goCount(deltaTime));
			}
		}
		//��	
		if (direction == ROBOT_BACKWARD) {
			if (Position.y == 7.0f || Position.y == 0.05f)
			{
				Position -= Front * MovementSpeed * deltaTime;
				if (Position.y == 7.00f)
					walk = 30 * cos(backCount(deltaTime));
			}
		}
		//��+��	
		if (direction == ROBOT_LEFT)
		{
			Yaw += TurningSpeed * deltaTime;
		}
		if (direction == ROBOT_RIGHT)
		{
			Yaw -= TurningSpeed * deltaTime;
		}
		//��
		if (direction == ROBOT_UP && (Position + Height*UpingSpeed* deltaTime).y > 6.90f) {//���Ӹ�Լ6.78
			Position.y=7.00f;
			Pitch = (90.0f / 7.0f)*Position.y;
			goHead();
			updateHeight();
		}
		//��
		if (direction == ROBOT_UP && (Position + Height*UpingSpeed* deltaTime).y <= 6.90f){//���Ӹ�Լ6.78
			Position.y += UpingSpeed* deltaTime;
			Pitch = (90.0f / 7.0f)*Position.y;
			updateHeight();
			backHead();
		}
		//��
		if (direction == ROBOT_DOWN && (Position - Height*UpingSpeed* deltaTime).y < 0.05f) {
			Position.y=0.05f;
			Pitch = (90.0f / 7.0f)*Position.y;
			walk = 0.0f;
			Grotate = 0.0f;
			updateHeight();
		}
		//��
		if (direction == ROBOT_DOWN && (Position - Height*UpingSpeed* deltaTime).y>=0.05f) {
			Position.y -= UpingSpeed* deltaTime;
			Pitch = (90.0f / 7.0f)*Position.y;
			updateHeight();
			backHead();
		}
		//�����+��
		if (direction == ROBOT_GUP) {
			if (Position.y == 7.0f)
			{
				Grotate = 30 * cos(Gup(deltaTime));
			}
		}
		if (direction == ROBOT_GDOWN) {
			if (Position.y == 7.0f)
			{
				Grotate = 30 * cos(Gdown(deltaTime));
			}
		}

		updateFront();
    }

    // ����DalayYaw
    void UpdateDelayYaw()
    {
        HistoryYaw.push(Yaw);
        while (HistoryYaw.size() > DelayFrameNum) {
            HistoryYaw.pop();
        }
        DelayYaw = HistoryYaw.front();
    }

    // ����DalayYaw
    void UpdateDelayPosition()
    {
        HistoryPosition.push(Position);
        while (HistoryPosition.size() > DelayFrameNum) {
            HistoryPosition.pop();
        }
        DelayPosition = HistoryPosition.front();
    }

private:
    // �����µ� Front ����
    void updateFront()
    {
        glm::vec3 front;
        front.x = -sin(glm::radians(getMidValYaw()));			//ǰ��
		front.y = 0.0f;
        front.z = -cos(glm::radians(getMidValYaw()));		//����
        Front = glm::normalize(front);
    }

	void updateHeight() {
		glm::vec3 height;
		height.x = 0.0f;
		height.y = Position.y;
		height.z = 0.0f;
		Height = glm::normalize(height);
	}
};

#endif