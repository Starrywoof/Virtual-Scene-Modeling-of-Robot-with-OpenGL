#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>
#include "robot.h"
#include "fixed_camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <iostream>
#include <math.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "assimp.lib")

// 窗口大小参数设置
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//物品捡起状态标志位
int hand_flag = 0;
int sword_flag = -1;

// 阴影分辨率
const unsigned int SHADOW_WIDTH = 1024 * 10;
const unsigned int SHADOW_HEIGHT = 1024 * 10;

// 是否为线框图模式
bool isPolygonMode = false;

// 世界坐标系向上单位向量Y
glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

// 变形金刚的初始位置
Robot robot(glm::vec3(0.0f, 0.05f, 0.0f));

// 相机观察视角的初始位置
glm::vec3 cameraPos(0.0f, 2.0f, 0.0f);
Camera camera(cameraPos);
FixedCamera fixedCamera(cameraPos);
bool isCameraFixed = false;

// 光照初始位置
glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;

// 深度Map的ID
unsigned int depthMap;
unsigned int depthMapFBO;

// 鼠标位置（中心）
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 根据运行环境动态调整变化速度
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 天空盒
unsigned int cubemapTexture;
unsigned int skyboxVAO, skyboxVBO;

// 天空盒顶点坐标
const float skyboxVertices[] = {

	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

// 天空盒的六个面贴图
const vector<std::string> faces{

	FileSystem::getPath("Resources/skybox4/iceflow_ft.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_bk.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_up.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_dn.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_rt.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_lf.tga")

};
//********************************************************************
//****************************【初始化↓】****************************

GLFWwindow* windowInit();		//窗口初始化
bool init();
void depthMapFBOInit();				// 深度图初始化
void skyboxInit();							// 天空盒初始化
void setDeltaTime();						// 计算帧时间
void updateFixedCamera();			//相机视角
void setLight(Shader& shader);	//光照属性设置

//****************************【初始化↑】****************************
//****************************【模型生成部分↓】*********************
//【地面】
void loadGround(Model& model, Shader& shader)
{
	// 设置视图矩阵
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// 设置模型矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	shader.setMat4("model", modelMatrix);
	// 设置投影矩阵
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

//【剑】
void loadSword(Model& model, Shader& shader) {
	float angel = 0.0f;
	// 设置视图矩阵
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// 设置模型矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.5f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.04f, 0.04f, 0.04f));
	for (int i = 0; i< 6; i++)	//生成6把剑环绕一圈
	{
		shader.setMat4("model", modelMatrix);
		// 设置投影矩阵
		glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
		shader.setMat4("projection", projMatrix);
		//环绕旋转
		modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model.Draw(shader);
	}

}

//【地面零件】
void loadGHand(Model& model, Shader& shader) {
	// 设置视图矩阵
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// 设置模型矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(3.0f, 1.5f, 4.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setMat4("model", modelMatrix);
	// 设置投影矩阵
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

//【天空盒】
void loadSkyBox(Shader& shader)
{
	// 设置视图矩阵，消除相机移动对天空盒的影响
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	// 设置投影矩阵
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

	shader.setMat4("view", viewMatrix);
	shader.setMat4("projection", projMatrix);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// 【跑车】
void loadCar(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
	//调整载入的模型的大小、形状、位置
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getYaw() - robot.getDelayYaw() / 2), WORLD_UP);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), WORLD_UP);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));

	//改变模型矩阵：跑车变形为变形金刚形态的变化
	glm::vec3 Ho2(0.0f, 0.0f, 1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getPitch()), Ho2);

	// 设置配置变换矩阵
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}

//【层级建模组建添加】
void loadAdd_things_Pos(Model& model, glm::mat4 modelMatrix, Shader& shader, int flag, float height)
{
	//层级建模组建位置调整default参数
	glm::vec3 testPos(0.0f, 0.0f, 0.0f);
	glm::vec3 Ho2(0.0f, 0.0f, 1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(fixedCamera.getYaw() + robot.getYaw() / 2), WORLD_UP);//旋转

	if (flag == 1)		//在车右边
	{
		testPos[0] += 1.5f;
		testPos[2] += 9.0f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//平移
		modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getWalk()), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//平移

		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, height / 6.9f, 1.0f));	//缩放

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));//旋转

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 2)		//在车左边
	{
		testPos[0] += 7.5f;
		testPos[2] -= 0.5f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//平移
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-robot.getWalk()), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//平移
		
		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, height / 6.9f, 1.0f));	//缩放
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));//旋转

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 3)//在车上面
	{
		float rotate_rate = 2.0f;
		float translate_change_0 = 7.0f / rotate_rate;
		float translate_change_2 = 4.0f / rotate_rate;
		testPos[0] += translate_change_0;
		testPos[2] += translate_change_2;

		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(height / rotate_rate, 0.2f, height / rotate_rate));	//缩放

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 4)//在车下面
	{
		float rotate_rate = 2.0f;
		float translate_change_0 = 7.0f / rotate_rate;
		float translate_change_2 = 4.0f / rotate_rate;

		testPos[0] += translate_change_0;
		testPos[2] += translate_change_2;

		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(height / rotate_rate, 0.2f, height / rotate_rate));	//缩放

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 5)//头部
	{
		testPos[0] -= 0.25f;
		//testPos[2] += ;
		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(-3.7f*robot.getHead()), 0.0f));	//平移

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 6) //左头部-向左旋转拼接
	{
		testPos[0] += 1.0f;
		testPos[2] -= 2.0f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//平移
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-robot.getGrotate()), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//平移

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), WORLD_UP);//旋转
		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(5.0f*robot.getHead()), 0.0f));	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));//缩放

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 7) //右头部-向右旋转拼接
	{
		testPos[0] += 1.0f;
		testPos[2] += 0.7f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//平移
		modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getGrotate()), glm::vec3(1.0f, 0.0f, 0.0f));//旋转
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//平移
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), WORLD_UP);//旋转
		modelMatrix = glm::translate(modelMatrix, testPos);	//平移
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(5.0f*robot.getHead()), 0.0f));	//平移
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));//缩放

		shader.setMat4("model", modelMatrix);// 设置配置变换矩阵
		model.Draw(shader);
	}

	if (flag == 8) //扳手零部件
	{
		glm::mat4 modelMatrix_default = glm::mat4(1.0f);
		if (robot.Position.y >= 1.05f)
		{
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, (5.5f*(robot.getHeight() / 7.0f)), 0.0f));	//平移
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

			shader.setMat4("model", modelMatrix);// 设置配置变换矩阵

			model.Draw(shader);
		}
	}

}

//【变形金刚机器人】
void loadRobot(Model& carModel, Model& LegModel, Model& CircleModel, Model& HeadModel, Model& HeadGunModel, Model &HandModel, Model SwordModel, Shader& shader)
{
	glm::vec3 HandPosition(3.0f, 1.5f, 4.0f);
	glm::vec3 SwordPosition(3.0f, 1.5f, 4.0f);
	// 设置视图矩阵
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// 设置投影矩阵
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	// 设置层级建模模型矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	glm::mat4 modelMatrix_no_y = glm::mat4(1.0f);
	glm::mat4 modelMatrix_no_y_left = glm::mat4(1.0f);

	glm::mat4 modelMatrix_no_y_up = glm::mat4(1.0f);
	glm::mat4 modelMatrix_no_y_down = glm::mat4(1.0f);

	glm::mat4 modelMatrix_no_y_head = glm::mat4(1.0f);
	glm::mat4 modelMatrix_no_y_leftarm = glm::mat4(1.0f);
	glm::mat4 modelMatrix_no_y_rightarm = glm::mat4(1.0f);

	glm::mat4 modelMatrix_no_y_hands = glm::mat4(1.0f);
	glm::mat4 modelMatrix_no_y_sword = glm::mat4(1.0f);

	//获取robot的实时位置
	//各个组建的运动不同，需要有独立的操作
	modelMatrix = glm::translate(modelMatrix, robot.getMidValPosition());		
	modelMatrix_no_y = glm::translate(modelMatrix_no_y, robot.getMidValPosition_no_y());
	modelMatrix_no_y_left = glm::translate(modelMatrix_no_y_left, robot.getMidValPosition_no_y());
	modelMatrix_no_y_up = glm::translate(modelMatrix_no_y_up, robot.getMidValPosition_fixed_y(19.0f));
	modelMatrix_no_y_down = glm::translate(modelMatrix_no_y_down, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_head = glm::translate(modelMatrix_no_y_head, robot.getMidValPosition_fixed_y(16.0f));
	modelMatrix_no_y_leftarm = glm::translate(modelMatrix_no_y_leftarm, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_rightarm = glm::translate(modelMatrix_no_y_rightarm, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_hands= glm::translate(modelMatrix_no_y_hands, robot.getMidValPosition_fixed_y(0.0f));

	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y = glm::rotate(modelMatrix_no_y, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_left = glm::rotate(modelMatrix_no_y_left, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_up = glm::rotate(modelMatrix_no_y_up, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_down = glm::rotate(modelMatrix_no_y_down, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_head = glm::rotate(modelMatrix_no_y_head, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_leftarm = glm::rotate(modelMatrix_no_y_leftarm, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_rightarm = glm::rotate(modelMatrix_no_y_rightarm, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转
	modelMatrix_no_y_hands= glm::rotate(modelMatrix_no_y_hands, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//旋转

	loadCar(carModel, modelMatrix, shader); // 渲染跑车

	//开始变形时，加载【双腿模型】，【魔法阵模型】
	if (robot.Position.y > 0.05f)
	{
		loadAdd_things_Pos(LegModel, modelMatrix_no_y, shader, 1, robot.getHeight());
		loadAdd_things_Pos(LegModel, modelMatrix_no_y_left, shader, 2, robot.getHeight());

		loadAdd_things_Pos(CircleModel, modelMatrix_no_y_up, shader, 3, robot.getHeight());
		loadAdd_things_Pos(CircleModel, modelMatrix_no_y_down, shader, 4, robot.getHeight());
	}

	//变形快完成时，加载【头部模型】，【头部武器模型】
	if (robot.Position.y > 5.0f)
	{
		loadAdd_things_Pos(HeadModel, modelMatrix_no_y_head, shader, 5, robot.getHeight());
		loadAdd_things_Pos(HeadGunModel, modelMatrix_no_y_leftarm, shader, 6, robot.getHeight());
		loadAdd_things_Pos(HeadGunModel, modelMatrix_no_y_rightarm, shader, 7, robot.getHeight());
	}

	//判断是否可以拾取地面零件
	if (pow(robot.Position.x - 3.0f, 2) + pow(robot.Position.z - 4.0f, 2) <= 6.0f)
	{
		hand_flag = 1;
	}

	if (pow(robot.Position.x - 0.0f, 2) + pow(robot.Position.z - 0.0f, 2) <= 8.5f && sword_flag == 1)
	{
		exit(0);
	}


	//装配地面零件，加载【地面零件模型】
	if(hand_flag)
		loadAdd_things_Pos(HandModel, modelMatrix_no_y_hands, shader, 8, robot.getHeight());

}
//*************【模型生成部分↑】**********************************

//*************【鼠标控制、键盘控制、天空盒加载↓】*************
void framebuffer_size_callback(GLFWwindow* window, int width, int height);				//窗口大小
void mouse_callback(GLFWwindow* window, double xpos, double ypos);						//鼠标移动
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);					//鼠标滚轮
void handleKeyInput(GLFWwindow* window);																	//键盘监听
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);	//键盘事件
unsigned int loadSkybox(vector<std::string> faces);															//天空盒生成
//*************【鼠标控制、键盘控制、天空盒加载↑】*************

//*************【main主函数】*************************************
int main()
{
	// *********************
	//【初始化窗口】
	GLFWwindow* window = windowInit();
	bool isInit = init();
	if (window == NULL || !isInit) {
		return -1;
	}
	depthMapFBOInit();	// 深度图初始化
	skyboxInit();				// 天空盒初始化
	
	// *********************
	// 【编译着色器】
	// shader：为模型添加光照和阴影
	Shader shader("shader/light_and_shadow.vs", "shader/light_and_shadow.fs");
	// depthShader：由光线角度生成深度信息
	Shader depthShader("shader/shadow_mapping_depth.vs", "shader/shadow_mapping_depth.fs");
	// skyboxShader：天空盒
	Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");

	// *********************
	// 【模型导入】
	// 【跑车模型】
	Model carModel(FileSystem::getPath("Resources/models/objModel/Lamborghini/Lamborghini.obj"));
	// 【地面模型】
	Model raceTrackModel(FileSystem::getPath("Resources/models/objModel/race-track/race-track.obj"));
	// 【机械腿模型】
	Model LegModel(FileSystem::getPath("Resources/models/objModel/Leg/Sci Fi Cross Bridge.obj"));
	// 【魔法阵模型】
	Model CircleModel(FileSystem::getPath("Resources/models/objModel/Circle/18373_Wiccan_Star_in_a_Circle_V1.obj"));
	// 【头模型】
	Model HeadModel(FileSystem::getPath("Resources/models/objModel/Drone166/Drone166.obj"));
	// 【头部机枪模型】
	Model HeadGunModel(FileSystem::getPath("Resources/models/objModel/RobotArm/obj.obj"));
	// 【剑模型】
	Model SwordModel(FileSystem::getPath("Resources/models/objModel/equalizer/equalizer.obj"));
	// 【地面零件模型】
	Model HandModel(FileSystem::getPath("Resources/models/objModel/OneHand/OneHand.obj"));

	// *********************
	// 【纹理配置】
	shader.use();
	shader.setInt("diffuseTexture", 0);
	// 使用"GL_TEXTURE15"与对应后光照属性
	shader.setInt("shadowMap", 15);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// *********************
	// 【程序循环部分】
	while (!glfwWindowShouldClose(window)) {
		
		setDeltaTime();// 计算帧时间，保证速度均匀
		handleKeyInput(window);// 键盘输入监听

		// 渲染背景
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// *********************
		// 场景阴影投影设置
		// 正交投影矩阵
		glm::mat4 lightProjection = glm::ortho(
			-200.0f, 200.0f,
			-200.0f, 200.0f,
			-200.0f, 200.0f);
		// 相机所在范围始终生成投影
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), WORLD_UP);
		lightSpaceMatrix = lightProjection * lightView;

		// 从光源角度渲染整个场景
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// 改变Viewport大小进行深度渲染
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		// 使用深度shader渲染场景，获得阴影效果
		glClear(GL_DEPTH_BUFFER_BIT);

		//【加载】模型【投影】
		loadGround(raceTrackModel, depthShader);	//【地面模型】投影
		loadRobot(carModel, LegModel, CircleModel, HeadModel, HeadGunModel, HandModel, SwordModel, depthShader);//【变形金刚模型投影】
		if(sword_flag == 1)
			loadSword(SwordModel, depthShader);			//【剑模型】投影
		if(hand_flag==0)
			loadGHand(HandModel, depthShader);			//【地面零件模型】投影

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 重置Viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		// 设置光照属性
		setLight(shader);
		robot.UpdateDelayYaw();
		robot.UpdateDelayPosition();
		// 更新相机视图
		if (isCameraFixed) {
			updateFixedCamera();
		}

		// 【变形金刚层级模型】加载
		loadRobot(carModel, LegModel, CircleModel, HeadModel, HeadGunModel, HandModel, SwordModel, shader);
		//【剑模型】加载
		if (sword_flag == 1)
			loadSword(SwordModel, shader);
		//【地面零件模型】加载
		if (hand_flag == 0)
			loadGHand(HandModel, shader);
		// 【地面模型】加载
		loadGround(raceTrackModel, shader);

		// 调整深度测试使得天空盒位置为无穷远
		//【天空盒】加载
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		loadSkyBox(skyboxShader);
		// 复原深度测试
		glDepthFunc(GL_LESS);

		// 窗口缓冲区事件处理
		glfwSwapBuffers(window);
		glfwPollEvents();	// 事件轮询
	}
	glfwTerminate();		// 关闭glfw
	return 0;
}

//窗口初始化
GLFWwindow* windowInit()
{
	// 窗口初始化配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"2017192040_WuJunYi_FinalProject", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// 使用GLFW控制用户的鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

bool init()
{
	// 加载OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return false;
	}
	// 配置全局openGL状态
	glEnable(GL_DEPTH_TEST);
	return true;
}

// depthMap初始化
void depthMapFBOInit()
{
	glGenFramebuffers(1, &depthMapFBO);
	// 创建深度纹理
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// 将深度纹理作为帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 天空盒初始化
void skyboxInit()
{
	// 天空盒的VAO数据
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// 天空盒贴图加载
	cubemapTexture = loadSkybox(faces);
}

// 计算帧时间长度
void setDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

// 相机位置更新
void updateFixedCamera()
{
	camera.ZoomRecover();
	// 相机关于车与世界坐标系变化
	float angle = glm::radians(-robot.getMidValYaw());
	glm::mat4 rotateMatrix(
		cos(angle), 0.0, sin(angle), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(angle), 0.0, cos(angle), 0.0,
		0.0, 0.0, 0.0, 1.0);
	glm::vec3 rotatedPosition = glm::vec3(rotateMatrix * glm::vec4(fixedCamera.getPosition(), 1.0));
	camera.FixView(rotatedPosition + robot.getMidValPosition(), fixedCamera.getYaw() + robot.getMidValYaw());
}

// 设置光照属性
void setLight(Shader& shader)
{
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightDirection", lightDirection);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

// 键盘监听
void handleKeyInput(GLFWwindow* window)
{
	// 【按键】esc退出
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//【按键】相机视图自由移动
	if (!isCameraFixed) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)	//前
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)		//后
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)		//左
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	//右
			camera.ProcessKeyboard(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)	//上
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)	//下
			camera.ProcessKeyboard(DOWN, deltaTime);
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			fixedCamera.ProcessKeyboard(CAMERA_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			fixedCamera.ProcessKeyboard(CAMERA_RIGHT, deltaTime);
	}

	// 【变形金刚模型】移动【前进与后退】
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {			//前进
		robot.ProcessKeyboard(ROBOT_FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)		//左转
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)	//右转
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {		//后退
		robot.ProcessKeyboard(ROBOT_BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)		//左转
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)	//右转
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomIn();
	}

	//【变形金刚模型】变形【上升】
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//【变形金刚模型】变形【下降】
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_DOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//【变形金刚模型】武器旋转【向上】
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_GUP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//【变形金刚模型】武器旋转【向下】
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_GDOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);

		if (isCameraFixed)
			camera.ZoomOut();
	}

	//【调试模型】彩蛋【开关】
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		sword_flag *= -1;
		Sleep(10);
	}

	// 回调监听按键
	glfwSetKeyCallback(window, key_callback);
}

// 回调监听函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		isCameraFixed = !isCameraFixed;
		string info = isCameraFixed ? "相机变为固定" : "相机变为自由";
		std::cout << "[CAMERA]" << info << std::endl;
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		isPolygonMode = !isPolygonMode;
		if (isPolygonMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		string info = isPolygonMode ? "变更为线框图渲染" : "变更为正常渲染";
		std::cout << "[POLYGON_MODE]" << info << std::endl;
	}
}

// 鼠标移动函数，控制相机角度
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!isCameraFixed) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // 坐标变换为相应坐标系
		lastX = xpos;
		lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// 鼠标滚轮函数，控制相机变焦
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// 改变窗口大小函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// 确保窗口匹配的新窗口尺寸
	glViewport(0, 0, width, height);
}

// 将六张贴图加载为天空盒cubemap纹理函数
unsigned int loadSkybox(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
