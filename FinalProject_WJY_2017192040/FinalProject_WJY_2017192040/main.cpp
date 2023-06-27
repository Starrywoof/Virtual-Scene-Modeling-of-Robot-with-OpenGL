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

// ���ڴ�С��������
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//��Ʒ����״̬��־λ
int hand_flag = 0;
int sword_flag = -1;

// ��Ӱ�ֱ���
const unsigned int SHADOW_WIDTH = 1024 * 10;
const unsigned int SHADOW_HEIGHT = 1024 * 10;

// �Ƿ�Ϊ�߿�ͼģʽ
bool isPolygonMode = false;

// ��������ϵ���ϵ�λ����Y
glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

// ���ν�յĳ�ʼλ��
Robot robot(glm::vec3(0.0f, 0.05f, 0.0f));

// ����۲��ӽǵĳ�ʼλ��
glm::vec3 cameraPos(0.0f, 2.0f, 0.0f);
Camera camera(cameraPos);
FixedCamera fixedCamera(cameraPos);
bool isCameraFixed = false;

// ���ճ�ʼλ��
glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;

// ���Map��ID
unsigned int depthMap;
unsigned int depthMapFBO;

// ���λ�ã����ģ�
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// �������л�����̬�����仯�ٶ�
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ��պ�
unsigned int cubemapTexture;
unsigned int skyboxVAO, skyboxVBO;

// ��պж�������
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

// ��պе���������ͼ
const vector<std::string> faces{

	FileSystem::getPath("Resources/skybox4/iceflow_ft.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_bk.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_up.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_dn.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_rt.tga"),
	FileSystem::getPath("Resources/skybox4/iceflow_lf.tga")

};
//********************************************************************
//****************************����ʼ������****************************

GLFWwindow* windowInit();		//���ڳ�ʼ��
bool init();
void depthMapFBOInit();				// ���ͼ��ʼ��
void skyboxInit();							// ��պг�ʼ��
void setDeltaTime();						// ����֡ʱ��
void updateFixedCamera();			//����ӽ�
void setLight(Shader& shader);	//������������

//****************************����ʼ������****************************
//****************************��ģ�����ɲ��֡���*********************
//�����桿
void loadGround(Model& model, Shader& shader)
{
	// ������ͼ����
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// ����ģ�;���
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	shader.setMat4("model", modelMatrix);
	// ����ͶӰ����
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

//������
void loadSword(Model& model, Shader& shader) {
	float angel = 0.0f;
	// ������ͼ����
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// ����ģ�;���
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.5f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.04f, 0.04f, 0.04f));
	for (int i = 0; i< 6; i++)	//����6�ѽ�����һȦ
	{
		shader.setMat4("model", modelMatrix);
		// ����ͶӰ����
		glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
		shader.setMat4("projection", projMatrix);
		//������ת
		modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model.Draw(shader);
	}

}

//�����������
void loadGHand(Model& model, Shader& shader) {
	// ������ͼ����
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// ����ģ�;���
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(3.0f, 1.5f, 4.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setMat4("model", modelMatrix);
	// ����ͶӰ����
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

//����պС�
void loadSkyBox(Shader& shader)
{
	// ������ͼ������������ƶ�����պе�Ӱ��
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	// ����ͶӰ����
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);

	shader.setMat4("view", viewMatrix);
	shader.setMat4("projection", projMatrix);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// ���ܳ���
void loadCar(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
	//���������ģ�͵Ĵ�С����״��λ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getYaw() - robot.getDelayYaw() / 2), WORLD_UP);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), WORLD_UP);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));

	//�ı�ģ�;����ܳ�����Ϊ���ν����̬�ı仯
	glm::vec3 Ho2(0.0f, 0.0f, 1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getPitch()), Ho2);

	// �������ñ任����
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}

//���㼶��ģ�齨��ӡ�
void loadAdd_things_Pos(Model& model, glm::mat4 modelMatrix, Shader& shader, int flag, float height)
{
	//�㼶��ģ�齨λ�õ���default����
	glm::vec3 testPos(0.0f, 0.0f, 0.0f);
	glm::vec3 Ho2(0.0f, 0.0f, 1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(fixedCamera.getYaw() + robot.getYaw() / 2), WORLD_UP);//��ת

	if (flag == 1)		//�ڳ��ұ�
	{
		testPos[0] += 1.5f;
		testPos[2] += 9.0f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//ƽ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getWalk()), glm::vec3(1.0f, 0.0f, 0.0f));//��ת
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//ƽ��

		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, height / 6.9f, 1.0f));	//����

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));//��ת

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 2)		//�ڳ����
	{
		testPos[0] += 7.5f;
		testPos[2] -= 0.5f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//ƽ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-robot.getWalk()), glm::vec3(1.0f, 0.0f, 0.0f));//��ת
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//ƽ��
		
		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, height / 6.9f, 1.0f));	//����
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));//��ת

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 3)//�ڳ�����
	{
		float rotate_rate = 2.0f;
		float translate_change_0 = 7.0f / rotate_rate;
		float translate_change_2 = 4.0f / rotate_rate;
		testPos[0] += translate_change_0;
		testPos[2] += translate_change_2;

		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(height / rotate_rate, 0.2f, height / rotate_rate));	//����

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 4)//�ڳ�����
	{
		float rotate_rate = 2.0f;
		float translate_change_0 = 7.0f / rotate_rate;
		float translate_change_2 = 4.0f / rotate_rate;

		testPos[0] += translate_change_0;
		testPos[2] += translate_change_2;

		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(height / rotate_rate, 0.2f, height / rotate_rate));	//����

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 5)//ͷ��
	{
		testPos[0] -= 0.25f;
		//testPos[2] += ;
		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(-3.7f*robot.getHead()), 0.0f));	//ƽ��

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 6) //��ͷ��-������תƴ��
	{
		testPos[0] += 1.0f;
		testPos[2] -= 2.0f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//ƽ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-robot.getGrotate()), glm::vec3(1.0f, 0.0f, 0.0f));//��ת
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//ƽ��

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), WORLD_UP);//��ת
		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(5.0f*robot.getHead()), 0.0f));	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));//����

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 7) //��ͷ��-������תƴ��
	{
		testPos[0] += 1.0f;
		testPos[2] += 0.7f;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));	//ƽ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getGrotate()), glm::vec3(1.0f, 0.0f, 0.0f));//��ת
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -7.0f, 0.0f));	//ƽ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), WORLD_UP);//��ת
		modelMatrix = glm::translate(modelMatrix, testPos);	//ƽ��
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, glm::radians(5.0f*robot.getHead()), 0.0f));	//ƽ��
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));//����

		shader.setMat4("model", modelMatrix);// �������ñ任����
		model.Draw(shader);
	}

	if (flag == 8) //�����㲿��
	{
		glm::mat4 modelMatrix_default = glm::mat4(1.0f);
		if (robot.Position.y >= 1.05f)
		{
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, (5.5f*(robot.getHeight() / 7.0f)), 0.0f));	//ƽ��
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

			shader.setMat4("model", modelMatrix);// �������ñ任����

			model.Draw(shader);
		}
	}

}

//�����ν�ջ����ˡ�
void loadRobot(Model& carModel, Model& LegModel, Model& CircleModel, Model& HeadModel, Model& HeadGunModel, Model &HandModel, Model SwordModel, Shader& shader)
{
	glm::vec3 HandPosition(3.0f, 1.5f, 4.0f);
	glm::vec3 SwordPosition(3.0f, 1.5f, 4.0f);
	// ������ͼ����
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// ����ͶӰ����
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	// ���ò㼶��ģģ�;���
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

	//��ȡrobot��ʵʱλ��
	//�����齨���˶���ͬ����Ҫ�ж����Ĳ���
	modelMatrix = glm::translate(modelMatrix, robot.getMidValPosition());		
	modelMatrix_no_y = glm::translate(modelMatrix_no_y, robot.getMidValPosition_no_y());
	modelMatrix_no_y_left = glm::translate(modelMatrix_no_y_left, robot.getMidValPosition_no_y());
	modelMatrix_no_y_up = glm::translate(modelMatrix_no_y_up, robot.getMidValPosition_fixed_y(19.0f));
	modelMatrix_no_y_down = glm::translate(modelMatrix_no_y_down, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_head = glm::translate(modelMatrix_no_y_head, robot.getMidValPosition_fixed_y(16.0f));
	modelMatrix_no_y_leftarm = glm::translate(modelMatrix_no_y_leftarm, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_rightarm = glm::translate(modelMatrix_no_y_rightarm, robot.getMidValPosition_fixed_y(0.0f));
	modelMatrix_no_y_hands= glm::translate(modelMatrix_no_y_hands, robot.getMidValPosition_fixed_y(0.0f));

	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y = glm::rotate(modelMatrix_no_y, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_left = glm::rotate(modelMatrix_no_y_left, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_up = glm::rotate(modelMatrix_no_y_up, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_down = glm::rotate(modelMatrix_no_y_down, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_head = glm::rotate(modelMatrix_no_y_head, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_leftarm = glm::rotate(modelMatrix_no_y_leftarm, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_rightarm = glm::rotate(modelMatrix_no_y_rightarm, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת
	modelMatrix_no_y_hands= glm::rotate(modelMatrix_no_y_hands, glm::radians(robot.getDelayYaw() / 2), WORLD_UP);//��ת

	loadCar(carModel, modelMatrix, shader); // ��Ⱦ�ܳ�

	//��ʼ����ʱ�����ء�˫��ģ�͡�����ħ����ģ�͡�
	if (robot.Position.y > 0.05f)
	{
		loadAdd_things_Pos(LegModel, modelMatrix_no_y, shader, 1, robot.getHeight());
		loadAdd_things_Pos(LegModel, modelMatrix_no_y_left, shader, 2, robot.getHeight());

		loadAdd_things_Pos(CircleModel, modelMatrix_no_y_up, shader, 3, robot.getHeight());
		loadAdd_things_Pos(CircleModel, modelMatrix_no_y_down, shader, 4, robot.getHeight());
	}

	//���ο����ʱ�����ء�ͷ��ģ�͡�����ͷ������ģ�͡�
	if (robot.Position.y > 5.0f)
	{
		loadAdd_things_Pos(HeadModel, modelMatrix_no_y_head, shader, 5, robot.getHeight());
		loadAdd_things_Pos(HeadGunModel, modelMatrix_no_y_leftarm, shader, 6, robot.getHeight());
		loadAdd_things_Pos(HeadGunModel, modelMatrix_no_y_rightarm, shader, 7, robot.getHeight());
	}

	//�ж��Ƿ����ʰȡ�������
	if (pow(robot.Position.x - 3.0f, 2) + pow(robot.Position.z - 4.0f, 2) <= 6.0f)
	{
		hand_flag = 1;
	}

	if (pow(robot.Position.x - 0.0f, 2) + pow(robot.Position.z - 0.0f, 2) <= 8.5f && sword_flag == 1)
	{
		exit(0);
	}


	//װ�������������ء��������ģ�͡�
	if(hand_flag)
		loadAdd_things_Pos(HandModel, modelMatrix_no_y_hands, shader, 8, robot.getHeight());

}
//*************��ģ�����ɲ��֡���**********************************

//*************�������ơ����̿��ơ���պм��ء���*************
void framebuffer_size_callback(GLFWwindow* window, int width, int height);				//���ڴ�С
void mouse_callback(GLFWwindow* window, double xpos, double ypos);						//����ƶ�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);					//������
void handleKeyInput(GLFWwindow* window);																	//���̼���
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);	//�����¼�
unsigned int loadSkybox(vector<std::string> faces);															//��պ�����
//*************�������ơ����̿��ơ���պм��ء���*************

//*************��main��������*************************************
int main()
{
	// *********************
	//����ʼ�����ڡ�
	GLFWwindow* window = windowInit();
	bool isInit = init();
	if (window == NULL || !isInit) {
		return -1;
	}
	depthMapFBOInit();	// ���ͼ��ʼ��
	skyboxInit();				// ��պг�ʼ��
	
	// *********************
	// ��������ɫ����
	// shader��Ϊģ����ӹ��պ���Ӱ
	Shader shader("shader/light_and_shadow.vs", "shader/light_and_shadow.fs");
	// depthShader���ɹ��߽Ƕ����������Ϣ
	Shader depthShader("shader/shadow_mapping_depth.vs", "shader/shadow_mapping_depth.fs");
	// skyboxShader����պ�
	Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");

	// *********************
	// ��ģ�͵��롿
	// ���ܳ�ģ�͡�
	Model carModel(FileSystem::getPath("Resources/models/objModel/Lamborghini/Lamborghini.obj"));
	// ������ģ�͡�
	Model raceTrackModel(FileSystem::getPath("Resources/models/objModel/race-track/race-track.obj"));
	// ����е��ģ�͡�
	Model LegModel(FileSystem::getPath("Resources/models/objModel/Leg/Sci Fi Cross Bridge.obj"));
	// ��ħ����ģ�͡�
	Model CircleModel(FileSystem::getPath("Resources/models/objModel/Circle/18373_Wiccan_Star_in_a_Circle_V1.obj"));
	// ��ͷģ�͡�
	Model HeadModel(FileSystem::getPath("Resources/models/objModel/Drone166/Drone166.obj"));
	// ��ͷ����ǹģ�͡�
	Model HeadGunModel(FileSystem::getPath("Resources/models/objModel/RobotArm/obj.obj"));
	// ����ģ�͡�
	Model SwordModel(FileSystem::getPath("Resources/models/objModel/equalizer/equalizer.obj"));
	// ���������ģ�͡�
	Model HandModel(FileSystem::getPath("Resources/models/objModel/OneHand/OneHand.obj"));

	// *********************
	// ���������á�
	shader.use();
	shader.setInt("diffuseTexture", 0);
	// ʹ��"GL_TEXTURE15"���Ӧ���������
	shader.setInt("shadowMap", 15);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// *********************
	// ������ѭ�����֡�
	while (!glfwWindowShouldClose(window)) {
		
		setDeltaTime();// ����֡ʱ�䣬��֤�ٶȾ���
		handleKeyInput(window);// �����������

		// ��Ⱦ����
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// *********************
		// ������ӰͶӰ����
		// ����ͶӰ����
		glm::mat4 lightProjection = glm::ortho(
			-200.0f, 200.0f,
			-200.0f, 200.0f,
			-200.0f, 200.0f);
		// ������ڷ�Χʼ������ͶӰ
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), WORLD_UP);
		lightSpaceMatrix = lightProjection * lightView;

		// �ӹ�Դ�Ƕ���Ⱦ��������
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// �ı�Viewport��С���������Ⱦ
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		// ʹ�����shader��Ⱦ�����������ӰЧ��
		glClear(GL_DEPTH_BUFFER_BIT);

		//�����ء�ģ�͡�ͶӰ��
		loadGround(raceTrackModel, depthShader);	//������ģ�͡�ͶӰ
		loadRobot(carModel, LegModel, CircleModel, HeadModel, HeadGunModel, HandModel, SwordModel, depthShader);//�����ν��ģ��ͶӰ��
		if(sword_flag == 1)
			loadSword(SwordModel, depthShader);			//����ģ�͡�ͶӰ
		if(hand_flag==0)
			loadGHand(HandModel, depthShader);			//���������ģ�͡�ͶӰ

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// ����Viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		// ���ù�������
		setLight(shader);
		robot.UpdateDelayYaw();
		robot.UpdateDelayPosition();
		// ���������ͼ
		if (isCameraFixed) {
			updateFixedCamera();
		}

		// �����ν�ղ㼶ģ�͡�����
		loadRobot(carModel, LegModel, CircleModel, HeadModel, HeadGunModel, HandModel, SwordModel, shader);
		//����ģ�͡�����
		if (sword_flag == 1)
			loadSword(SwordModel, shader);
		//���������ģ�͡�����
		if (hand_flag == 0)
			loadGHand(HandModel, shader);
		// ������ģ�͡�����
		loadGround(raceTrackModel, shader);

		// ������Ȳ���ʹ����պ�λ��Ϊ����Զ
		//����պС�����
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		loadSkyBox(skyboxShader);
		// ��ԭ��Ȳ���
		glDepthFunc(GL_LESS);

		// ���ڻ������¼�����
		glfwSwapBuffers(window);
		glfwPollEvents();	// �¼���ѯ
	}
	glfwTerminate();		// �ر�glfw
	return 0;
}

//���ڳ�ʼ��
GLFWwindow* windowInit()
{
	// ���ڳ�ʼ������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������
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

	// ʹ��GLFW�����û������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

bool init()
{
	// ����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return false;
	}
	// ����ȫ��openGL״̬
	glEnable(GL_DEPTH_TEST);
	return true;
}

// depthMap��ʼ��
void depthMapFBOInit()
{
	glGenFramebuffers(1, &depthMapFBO);
	// �����������
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// �����������Ϊ֡����
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ��պг�ʼ��
void skyboxInit()
{
	// ��պе�VAO����
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// ��պ���ͼ����
	cubemapTexture = loadSkybox(faces);
}

// ����֡ʱ�䳤��
void setDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

// ���λ�ø���
void updateFixedCamera()
{
	camera.ZoomRecover();
	// ������ڳ�����������ϵ�仯
	float angle = glm::radians(-robot.getMidValYaw());
	glm::mat4 rotateMatrix(
		cos(angle), 0.0, sin(angle), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(angle), 0.0, cos(angle), 0.0,
		0.0, 0.0, 0.0, 1.0);
	glm::vec3 rotatedPosition = glm::vec3(rotateMatrix * glm::vec4(fixedCamera.getPosition(), 1.0));
	camera.FixView(rotatedPosition + robot.getMidValPosition(), fixedCamera.getYaw() + robot.getMidValYaw());
}

// ���ù�������
void setLight(Shader& shader)
{
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightDirection", lightDirection);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

// ���̼���
void handleKeyInput(GLFWwindow* window)
{
	// ��������esc�˳�
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//�������������ͼ�����ƶ�
	if (!isCameraFixed) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)	//ǰ
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)		//��
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)		//��
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	//��
			camera.ProcessKeyboard(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)	//��
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)	//��
			camera.ProcessKeyboard(DOWN, deltaTime);
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			fixedCamera.ProcessKeyboard(CAMERA_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			fixedCamera.ProcessKeyboard(CAMERA_RIGHT, deltaTime);
	}

	// �����ν��ģ�͡��ƶ���ǰ������ˡ�
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {			//ǰ��
		robot.ProcessKeyboard(ROBOT_FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)		//��ת
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)	//��ת
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {		//����
		robot.ProcessKeyboard(ROBOT_BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)		//��ת
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)	//��ת
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomIn();
	}

	//�����ν��ģ�͡����Ρ�������
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//�����ν��ģ�͡����Ρ��½���
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_DOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//�����ν��ģ�͡�������ת�����ϡ�
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_GUP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);
		if (isCameraFixed)
			camera.ZoomOut();
	}

	//�����ν��ģ�͡�������ת�����¡�
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		robot.ProcessKeyboard(ROBOT_GDOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			robot.ProcessKeyboard(ROBOT_RIGHT, deltaTime);

		if (isCameraFixed)
			camera.ZoomOut();
	}

	//������ģ�͡��ʵ������ء�
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		sword_flag *= -1;
		Sleep(10);
	}

	// �ص���������
	glfwSetKeyCallback(window, key_callback);
}

// �ص���������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		isCameraFixed = !isCameraFixed;
		string info = isCameraFixed ? "�����Ϊ�̶�" : "�����Ϊ����";
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
		string info = isPolygonMode ? "���Ϊ�߿�ͼ��Ⱦ" : "���Ϊ������Ⱦ";
		std::cout << "[POLYGON_MODE]" << info << std::endl;
	}
}

// ����ƶ���������������Ƕ�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!isCameraFixed) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // ����任Ϊ��Ӧ����ϵ
		lastX = xpos;
		lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// �����ֺ�������������佹
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// �ı䴰�ڴ�С����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// ȷ������ƥ����´��ڳߴ�
	glViewport(0, 0, width, height);
}

// ��������ͼ����Ϊ��պ�cubemap������
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
