#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
//airplane, house, car, sword, hat
#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
int lastswordtheta=0;
int sword_theta = 0;
float lastflowerangle=0;
float lasthouseangle=0;
float lastplaneangle = 0;
int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f;
int leftbuttonpressed = 0;
int rightbuttonpressed = 0;
int housex = +100;
int flowerx = -100;
int airplanex = 300;
float housescale = 1;
float flowerscale = 1;
float airplanescale = 1;
float carscale = 1;
float swordscale = 1;
///집에 차를 부딪히면 carscale=1;
//화분에 차를 부딪히면 carscale+=0.02;   max=1.30;
//비행기에 차를 부딪히면 carscale-=0.02;   min  =0.7;
//
int carflag = 0;//1이면 left, 2이면 right;
float carx = 0;
float cary = -200;

int hat_timer_flag = 1;//0이면 hattimer 시작
int hat_flag = 0;//hat 동작 flag
int hat_timer = 0;//hat timer

int sword_timer_flag = 1;//0이면 swordtimer 시작
int sword_flag = 0;//sword 동작 flag
int sword_timer = 0;//sword timer

int flower_timer_flag = 1;
int flower_flag = 0;
int flower_timer = 0;

int house_timer_flag = 1;
int house_flag = 0;
int house_timer = 0;

int airplane_timer_flag = 1;//0이면 hattimer 시작
int airplane_flag = 0;//hat 동작 flag
int airplane_timer = 0;//hat timer

// 2D 물체 정의 부분은 objects.h 파일로 분리
// 새로운 물체 추가 시 prepare_scene() 함수에서 해당 물체에 대한 prepare_***() 함수를 수행함.
// (필수는 아니나 올바른 코딩을 위하여) cleanup() 함수에서 해당 resource를 free 시킴.
#include "objects.h"

unsigned int timestamp = 0;
void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	if (hat_timer_flag == 0)
		hat_timer = hat_timer + 1;
	else if (hat_timer_flag == 1)
		hat_timer = 0;

	if (flower_timer_flag == 0)
		flower_timer = flower_timer + 1;
	else if (flower_timer_flag == 1)
		flower_timer = 0;

	if (house_timer_flag == 0)
		house_timer = house_timer + 1;
	else if (flower_timer_flag == 1)
		house_timer = 0;

	if (airplane_timer_flag == 0)
		airplane_timer = airplane_timer + 1;
	else if (flower_timer_flag == 1)
		airplane_timer = 0;

	if (sword_timer_flag == 0)
		sword_timer = sword_timer + 1;
	else if (sword_timer_flag == 1)
		sword_timer = 0;

	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_ground();

	int sceneframe = timestamp;


	float y, x;//for translate
	int house_clock;
	if (timestamp < 850)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		////////////////////////////

		//자동차의 움직임
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-sceneframe + 515, 0.7 * sinf(-sceneframe + 515) - 200, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();

		//////////////////////////


	}
	else if (timestamp < 1200)
	{
		sceneframe = timestamp - 850;
		float swordclock = (1200 - timestamp) * (float)500 / 350;//비행기가 지나간 이후에 swordclock 시작

		glClearColor(50 / 255.0f, 64 / 255.0f, 27 / 255.0f, 1.0f);
		x = 4 * swordclock;
		y = 15 * sqrtf(swordclock);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 300, y - 180, 0.0f));//비행기가 지나간 이후에 sword 움직임.
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(1.0f / (2 * sqrtf(swordclock)) * 15), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		x = 4 * swordclock;
		y = 15 * sqrtf(swordclock);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 100, y - 180, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(1.0f / (2 * sqrtf(swordclock)) * 15), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		x = 4 * swordclock;
		y = 40 * sqrtf(swordclock);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 100, y - 180, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(1.0f / (2 * sqrtf(swordclock)) * 40), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		x = 4 * swordclock;
		y = 50 * sqrtf(swordclock);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 300, y - 180, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(1.0f / (2 * sqrtf(swordclock)) * 50), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();



		x = -335.0f;//차는 그만 가만히 있음.
		y = 0.7 * sinf(-335) - 200;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();


		x = -500 + 4 * sceneframe;//비행기 지나감
		y = sinf(x) + 200 + 0.0005 * powf(x, 2);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(0.001 * x), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1 - (float)sceneframe / 350, 1 - (float)sceneframe / 350, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		x = -500 + 4 * sceneframe;//비행기 지나감
		y = sinf(x) + 200 - 0.0005 * powf(x, 2);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(-0.001 * x), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1 - (float)sceneframe / 350, 1 - (float)sceneframe / 350, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		x = -500 + 4 * sceneframe;//비행기 지나감
		y = 30 * sinf(0.05 * sceneframe - 500) - 100;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90 * TO_RADIAN + atanf(30 * cosf(0.05 * sceneframe - 500) * 0.05), glm::vec3(0.0f, 0.0f, 0.1f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1 - (float)sceneframe / 350, 1 - (float)sceneframe / 350, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();
		//////////////////////////


	}
	else if (timestamp < 1350)//집이 칼에 꽂힌 이후 날라감.
	{
		sceneframe = timestamp - 1200;

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-335, 0.7 * sinf(-335) - 200, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();

		x = sceneframe;
		if (x < 45)
		{
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 300, -180, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cosf(2 * x * TO_RADIAN), cosf(2 * x * TO_RADIAN), 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN * 8, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-x - 300, -180, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cosf(2 * x * TO_RADIAN), cosf(2 * x * TO_RADIAN), 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN * 8, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300, -x - 180, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cosf(2 * x * TO_RADIAN), cosf(2 * x * TO_RADIAN), 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN * 8, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300, x - 180, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cosf(2 * x * TO_RADIAN), cosf(2 * x * TO_RADIAN), 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN * 8, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_house();
		}
		///////////////////////////////////////////////

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100, -180.0f +sceneframe*4, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100 - 4 * sceneframe, -180.0f + 100 * atanf(3 * sceneframe * TO_RADIAN), 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300, 0, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (-2 * sceneframe) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, -180.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();


	}
	else if (timestamp < 1518)
	{
		glClearColor(166 / 255.0f, 208 / 255.0f, 228 / 255.0f, 1.0f);//다시 파란하늘
		sceneframe = timestamp - 1350;
		x = 2 * sceneframe - 335;
		y = 0.7 * sinf(x) - 200;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
		carflag = 0;
	}
	else
	{

		sceneframe = timestamp - 1518;
		if (carflag == 1)//차가 왼쪽으로 움직임
		{
			if (carx > -400)
				carx = carx - 2;
		}
		else if (carflag == 2)//차가 오른쪽으로 움직임
		{
			if (carx < 400)
				carx = carx + 2;
		}
		cary = 0.7 * sinf(carx) - 200;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(carx, cary, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(carscale, carscale, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
		if (leftbuttonpressed)
			hat_timer = hat_timer - 1;
		if (hat_flag == 0)///모자 날라감
		{
			hat_timer_flag = 0;
			x = 4 * hat_timer - 500;
			y = 0.001 * powf(x, 2);
			if (x > 0)
			{
				hat_flag = 1;
				hat_timer = 0;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, atanf(0.002f * x), glm::vec3(0.0f, 0.0f, 0.1f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * cosf(x / 70)) + 0.5, abs(0.4 * cosf(x / 70)) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();


				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 30, -y - 30, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, atanf(-0.002f * x), glm::vec3(0.0f, 0.0f, 0.1f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * sin(x / 70)) + 0.5, abs(0.4 * sin(x / 70)) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();
			}
		}
		if (hat_flag == 1)
		{
			x = 2 * hat_timer;
			y = 0;
			if (x > 360)
			{
				hat_flag = 2;
				hat_timer = 0;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, y + 100, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));;
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, y - 100, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * cosf((x * TO_RADIAN))) + 0.5, abs(0.4 * cosf((x * TO_RADIAN))) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30, y + 100 - 30, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, x * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));;
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, y - 100, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * sin((x * TO_RADIAN))) + 0.5, abs(0.4 * sin((x * TO_RADIAN))) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();
			}
		}
		if (hat_flag == 2)
		{
			x = 4 * hat_timer;
			y = -0.001 * powf(x, 2);
			if (x > 500)
			{
				hat_flag = 0;
				hat_timer = 0;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, atanf(-0.002f * x), glm::vec3(0.0f, 0.0f, 0.1f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * cosf(x / 70)) + 0.5, abs(0.4 * cosf(x / 70)) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();


				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 30, -y - 30, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, atanf(0.002f * x), glm::vec3(0.0f, 0.0f, 0.1f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(abs(0.4 * sinf(x / 70)) + 0.5, abs(0.4 * sin(x / 70)) + 0.5, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();
			}
		}

		if (flower_flag == 0)///화분날라감
		{
			flower_timer_flag = 0;
			x = flowerx;
			y = -200;
			if (x == carx)//화분이 오른쪽으로 날라감.
			{
				flower_flag = 1;
				lastflowerangle = flower_timer % 360 * TO_RADIAN * 4;
				flower_timer = 0;
				carscale += 0.1;
				if (carscale > 1.5)
					carscale = 1.5;//화분에 부딪히면 차크기 커짐
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(flowerscale, flowerscale, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, flower_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_flowerpot();
			}
		}
		if (flower_flag == 1)///화분날라감
		{
			x = 2 * flower_timer;
			if (x > 90)
			{
				flower_flag = 0;
				flower_timer = 0;
				flowerx = (((timestamp + 50 + flowerx) % 401) - 200) * 2;
				flowerscale = abs(sinf(flowerx)) + 0.5;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(flowerx, -200, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(flowerscale + 2 * flowerscale * sinf(x * TO_RADIAN), flowerscale * cos(x * TO_RADIAN), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, lastflowerangle+flower_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_flowerpot();

			}
		}
		if (house_flag == 0)///집날라감
		{
			house_timer_flag = 0;
			x = housex;
			y = -200;
			if (x == carx )//집.
			{
				house_flag = 1;
				lasthouseangle = house_timer % 360 * TO_RADIAN * 4;
				house_timer = 0;
				carscale = 1;//집에부딪히면 차크기 1
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(housescale, housescale, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, house_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
		}
		if (house_flag == 1)//집날라감
		{
			x = 2 * house_timer;
			y = 0.01 * powf(x, 2);
			if (x > 90)
			{
				house_flag = 0;
				house_timer = 0;
				housex = (((timestamp + 50 + housex) % 401) - 200) * 2;
				housescale = abs(sinf(housex)) + 0.5;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + housex, y - 200, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(housescale * cosf(x * TO_RADIAN), housescale * cosf(x * TO_RADIAN), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix,lasthouseangle+ house_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-x + housex, y - 200, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(housescale * cosf(x * TO_RADIAN), housescale * cosf(x * TO_RADIAN), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, lasthouseangle+house_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + housex, -y - 200, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(housescale * cosf(x * TO_RADIAN), housescale * cosf(x * TO_RADIAN), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, lasthouseangle+house_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-x + housex, -y - 200, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(housescale * cosf(x * TO_RADIAN), housescale * cosf(x * TO_RADIAN), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, lasthouseangle+house_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();

			}
		}
		

		if (airplane_flag == 0)///비행기날라감
		{
			airplane_timer_flag = 0;
			x = airplanex;
			y = -200;
			if (x == carx && carflag == 2)//비행기이오른쪽으로날라감.
			{
				lastplaneangle = airplane_timer % 360 * TO_RADIAN * 4;
				airplane_flag = 1;
				airplane_timer = 0;
				carscale -= 0.1;
				if (carscale < 0.5)
					carscale = 0.5;
			}
			else if (x == carx && carflag == 1)//비행기이왼쪽으로날라감.
			{
				lastplaneangle = airplane_timer % 360 * TO_RADIAN * 4;
				airplane_flag = 2;
				airplane_timer = 0;
				carscale -= 0.1;//차 크기 작아짐 비행기에 부딪히면
				if (carscale < 0.5)
					carscale = 0.5;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplanescale, airplanescale, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, airplane_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_airplane();
			}
		}
		if (airplane_flag == 1)///비행기날라감
		{
			x = 4 * airplane_timer;
			y = 0.01 * powf(x, 2) - 200;
			if (x > 500)
			{
				airplane_flag = 0;
				airplane_timer = 0;
				airplanex = (((timestamp + 250 + airplanex) % 401) - 200) * 2;
				airplanescale = abs(sinf(airplanex)) + 0.5;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + airplanex, y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplanescale, airplanescale, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, lastplaneangle+airplane_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_airplane();

			}
		}
		if (airplane_flag == 2)///비행기날라감
		{
			x = -4 * airplane_timer;
			y = 0.01 * powf(x, 2) - 200;
			if (x < -500)
			{
				airplane_flag = 0;
				airplane_timer = 0;
				airplanex = (((timestamp + 350 + airplanex) % 401) - 200) * 2;
				airplanescale = abs(sinf(airplanex)) + 0.5;
			}
			else
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + airplanex, y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplanescale, airplanescale, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix,lastplaneangle+ airplane_timer % 360 * TO_RADIAN * 4, glm::vec3(0.0f, 0.0f, 0.1f));//(0~359)
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_airplane();

			}
		}


		/////////////////////////////////////////////////sword
		if (rightbuttonpressed == 0)
		{
			if (sword_flag == 0)///칼 날라감
			{
				sword_timer_flag = 0;
				sword_theta = sword_timer % 360;//0~360
				lastswordtheta = sword_theta;
				for (int i = 0; i < 12; i++)
				{
					x = 16 * powf(sinf(sword_theta * TO_RADIAN), 3);
					y = 13 * cosf(sword_theta * TO_RADIAN) - 5 * cosf(2 * sword_theta * TO_RADIAN) - 2 * cosf(3 * sword_theta * TO_RADIAN) - cosf(4 * sword_theta * TO_RADIAN);
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(5 * x + carx, 5 * y - 75, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(swordscale, swordscale, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_sword();
					sword_theta += 30;
				}
			}
		}
		if (rightbuttonpressed == 1)
		{
			if (sword_flag == 0)///칼 날라감
			{
				sword_timer_flag = 0;
				sword_theta = lastswordtheta -sword_timer % 360;//0~360
				for (int i = 0; i < 12; i++)
				{
					x = 16 * powf(sinf(sword_theta * TO_RADIAN), 3);
					y = 13 * cosf(sword_theta * TO_RADIAN) - 5 * cosf(2 * sword_theta * TO_RADIAN) - 2 * cosf(3 * sword_theta * TO_RADIAN) - cosf(4 * sword_theta * TO_RADIAN);
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(5 * x + carx, 5 * y - 75, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(swordscale, swordscale, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환해주는행렬
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_sword();
					sword_theta -= 30;
				}
			}
		}


	}

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(330, 170.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5, 1.5, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cloud();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(120, 200.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cloud();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-70, 170.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5, 1.5, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cloud();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-270, 200.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cloud();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(350, -220.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_flowerpot();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150, -220.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_flowerpot();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50, -220.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_flowerpot();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-250, -220.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;//변환행렬
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_flowerpot();


	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'd':
	case 'D':
		carflag = 2;
		break;
	case 'a':
	case 'A':
		carflag = 1;
		break;
	}
}


void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		rightbuttonpressed = 1;
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		rightbuttonpressed = 0;
}

void motion(int x, int y) {
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(166 / 255.0f, 208 / 255.0f, 228 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_hat();
	prepare_sword();
	prepare_flowerpot();
	prepare_ground();
	prepare_cloud();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
int main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformationMotion_GLSL_3.0.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC'"
		"    - Mouse used: L-click and move"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


