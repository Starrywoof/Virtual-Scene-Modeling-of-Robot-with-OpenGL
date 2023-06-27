#ifndef ROBOT_H
#define ROBOT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>

// 定义变形金刚的多种动作
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

    // 存储旧Yaw参数，实现左右移动
    queue<float> HistoryYaw;
    int DelayFrameNum = 20;
    float DelayYaw;

    // 实现变形金刚的移动
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

	//初始化
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
	//位置
    glm::vec3 getPosition()
    {
        return Position;
    }
	//旋转
	float getGrotate()
	{
		return Grotate;
	}
	//零件旋转动作参数
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
	//移动动作参数-前进和后退
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
	//头部运动参数
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
	//方向移动参数
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
	//位置确定参数
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

    // 视图矩阵
    glm::mat4 GetViewMatrix(glm::vec3 cameraPosition)
    {
        return glm::lookAt(cameraPosition, Position, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // 【键盘输入】【前后左右上下动作计算】
	void ProcessKeyboard(Direction direction, float deltaTime)
	{
		//前
		if (direction == ROBOT_FORWARD) {
			if (Position.y == 7.0f || Position.y == 0.05f)
			{
				Position += Front * MovementSpeed * deltaTime;
				if(Position.y == 7.00f)
					walk = 30*cos(goCount(deltaTime));
			}
		}
		//后	
		if (direction == ROBOT_BACKWARD) {
			if (Position.y == 7.0f || Position.y == 0.05f)
			{
				Position -= Front * MovementSpeed * deltaTime;
				if (Position.y == 7.00f)
					walk = 30 * cos(backCount(deltaTime));
			}
		}
		//左+右	
		if (direction == ROBOT_LEFT)
		{
			Yaw += TurningSpeed * deltaTime;
		}
		if (direction == ROBOT_RIGHT)
		{
			Yaw -= TurningSpeed * deltaTime;
		}
		//上
		if (direction == ROBOT_UP && (Position + Height*UpingSpeed* deltaTime).y > 6.90f) {//柱子高约6.78
			Position.y=7.00f;
			Pitch = (90.0f / 7.0f)*Position.y;
			goHead();
			updateHeight();
		}
		//上
		if (direction == ROBOT_UP && (Position + Height*UpingSpeed* deltaTime).y <= 6.90f){//柱子高约6.78
			Position.y += UpingSpeed* deltaTime;
			Pitch = (90.0f / 7.0f)*Position.y;
			updateHeight();
			backHead();
		}
		//下
		if (direction == ROBOT_DOWN && (Position - Height*UpingSpeed* deltaTime).y < 0.05f) {
			Position.y=0.05f;
			Pitch = (90.0f / 7.0f)*Position.y;
			walk = 0.0f;
			Grotate = 0.0f;
			updateHeight();
		}
		//下
		if (direction == ROBOT_DOWN && (Position - Height*UpingSpeed* deltaTime).y>=0.05f) {
			Position.y -= UpingSpeed* deltaTime;
			Pitch = (90.0f / 7.0f)*Position.y;
			updateHeight();
			backHead();
		}
		//零件上+下
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

    // 更新DalayYaw
    void UpdateDelayYaw()
    {
        HistoryYaw.push(Yaw);
        while (HistoryYaw.size() > DelayFrameNum) {
            HistoryYaw.pop();
        }
        DelayYaw = HistoryYaw.front();
    }

    // 更新DalayYaw
    void UpdateDelayPosition()
    {
        HistoryPosition.push(Position);
        while (HistoryPosition.size() > DelayFrameNum) {
            HistoryPosition.pop();
        }
        DelayPosition = HistoryPosition.front();
    }

private:
    // 计算新的 Front 向量
    void updateFront()
    {
        glm::vec3 front;
        front.x = -sin(glm::radians(getMidValYaw()));			//前进
		front.y = 0.0f;
        front.z = -cos(glm::radians(getMidValYaw()));		//后退
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