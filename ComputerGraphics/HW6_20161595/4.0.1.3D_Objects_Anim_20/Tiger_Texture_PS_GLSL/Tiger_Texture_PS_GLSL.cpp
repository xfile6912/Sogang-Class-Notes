#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS, h_ShaderProgram_gouraud; // handles to shader programs

// for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
GLint loc_global_ambient_colorg;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_light_Parameters loc_lightg[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
loc_Material_Parameters loc_materialg;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_ModelViewProjectionMatrix_gouraud, loc_ModelViewMatrix_gouraud, loc_ModelViewMatrixInvTrans_gouraud;
GLint loc_texture, loc_flag_texture_mapping;
GLint loc_textureg, loc_flag_texture_mappingg;
GLint loc_screen_effect, loc_screen_frequency, loc_blind_frequency;
GLint loc_blind_effect;
int rightbutton = 0;
int leftbutton = 0;
// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;
glm::mat4 tankModelViewMatrix;
glm::mat4 tankcameraModelViewMatrix;
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

float frequency = 1.0f;
float blindfrequency = 1.0f;



// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

float light1[4];
float light2[4];
float light3[4];
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;

int flag_camera=0;///////만약 0이면 u방향으로, 1이면 v방향으로, 2면 n방향으로
int flagg = 0;
int flagr = 0;
int flagt = 0;
int flage = 0;
int flagw = 0;
int frequencyflag = 0;//0이면 upcount, 1이면 down count;
int blindfrequencyflag = 0;
// for tiger animation
int timestamp_scene = 0; // the global clock in the scene
int flag_tiger_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0;
float rotation_angle_tiger = 0.0f;

#include "my_geom_objects.h"
int tankflag=0;
int tanktimer = 0;
int spider1 = 0;
int spidertimer1 = 0;
int spider2 = 0;
int spidertimer2 = 0;
int wolftimer = 0;
int wolfflag = 3;
int humantimer = 0;
int humantimer2 = 0;
int humanflag = 0;
float wolfx = 240.0f;
float wolfy = 0.0f;
float wolfz = -240.0f;
float objectx = 350.0f;
float objecty = 0.0f;
float objectz = 240.f;
int count = 1;

glm::mat4 vectorM;
float lastangle=0.0f;
glm::vec3 nowv;
glm::vec3 lastv = glm::vec3(0.0f, 0.0f, 1.0f);
// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };
int redflag=0;
int greenflag = 0;
int blueflag = 0;

void changecolor(int flag)
{
	if (flag == 1)
	{
		if (redflag == 0)
		{
			red = red + 0.05;
			if (red >= 1.0f)
				redflag = 1;
		}
		else
		{
			red = red - 0.05;
			if (red <= 0.0f)
				redflag = 0;
		}
		if (greenflag == 0)
		{
			green = green + 0.03;
			if (green >= 1.0f)
				greenflag = 1;
		}
		else
		{
			green = green - 0.03;
			if (green <= 0.0f)
				greenflag = 0;
		}
		if (blueflag == 0)
		{
			blue += 0.07;
			if (blue >= 1.0f)
				blueflag = 1;
		}
		else
		{
			blue -= 0.07;
			if (blue <= 0.0f)
				blueflag = 0;
		}

		light1[0] = red;light1[1]=0.0f, light1[2]=0.0f, light1[3]=1.0f;
		light2[0] = 0.0f; light2[1] = green, light2[2] = 0.0f, light2[3] = 1.0f;
		light3[0] = 0.0f; light3[1] = 0.0f, light3[2] = blue, light3[3] = 1.0f;
		glUniform4fv(loc_light[0].diffuse_color, 1, light1);
		glUniform4fv(loc_light[1].diffuse_color, 1, light2);
		glUniform4fv(loc_light[2].diffuse_color, 1, light3);
	}
	else
	{
		glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
		glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
		glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	}
}
float tankx;
float spider1x;
float spider2x=240.0f;
float spider2y=0.0f;
float spider2z=0.0f;
void displayspider()
{
	set_material_tiger();//애니메이션
	glUniform1i(loc_texture, TEXTURE_ID_SPIDER);
	if (spider1 == 1)
	{

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(spider1x + 40 * sinf( spidertimer1 * 2 * TO_RADIAN), 10 + 10 * cosf(5 * spidertimer1 * 2 * TO_RADIAN), -240 + spidertimer1 * 2));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(spidertimer1/450.0f+0.3, spidertimer1 / 450.0f+0.3, spidertimer1 / 450.0f+0.3));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, -20.0f, 20.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		if (-240 + spidertimer1 * 2 > 490)
			spider1 = 0;

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_spider();
	}
	set_material_tank();////////애니메이션 넣기
	glUniform1i(loc_texture, TEXTURE_ID_MINE);
	if (humanflag == 0)
	{
		float z;
		if (spider2z < objectz)
			z = spider2z + spidertimer2;
		else if (spider2z > objectz)
			z = spider2z - spidertimer2;
		float temp = (spider2x - objectx) / (spider2z - objectz) * (z - spider2z) + spider2x;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(temp, 0.0f, z));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (z == objectz)
		{
			humanflag = 1;
			wolfflag = 0;
			wolftimer = 0;
		}
	}
	if (humanflag == 1)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(objectx, objecty, objectz));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (humanflag != 2)
	{
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, -20.0f, 20.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_spider();
	}
}


void displaytank()
{
	set_material_tank();////////애니메이션 넣기
	glUniform1i(loc_texture, TEXTURE_ID_TANK);

	//ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	if (tankflag == 0)//위로 올라감
	{
		tankx = 180.0f - tanktimer;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tankx == -180.0f)
		{
			tanktimer = 0;
			tankflag = 1;
		}
	}
	if (tankflag == 1)//회전
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, tanktimer * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tanktimer == 90)
		{
			tanktimer = 0;
			tankflag = 2;
		}
	}
	if (tankflag == 2)//뒤로 후진
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0 - tanktimer));
		if (tanktimer==30)
		{
			tanktimer = 0;
			tankflag = 3;
			spider1 = 1;
			spidertimer1 = 0;
			spider1x = tankx;
		}
	}
	if (tankflag == 3)///앞으로 전진
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0-30 + tanktimer));
		if (tanktimer == 30)
		{
			tanktimer = 0;
			tankflag = 4;
			if (tankx == 180)
				tankflag = 7;
		}
	}
	if (tankflag == 4)//회전
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, tanktimer * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tanktimer == 90)
		{
			tanktimer = 0;
			tankflag = 5;
		}
	}
	if (tankflag == 5)//아래로 내려옴
	{

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx+tanktimer, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tanktimer == 120)
		{
			tankx = tankx + tanktimer;
			tanktimer = 0;
			tankflag = 6;
		}
	}
	if (tankflag == 6)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -tanktimer * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tanktimer == 90)
		{
			tanktimer = 0;
			tankflag = 2;
		}
	}
	if (tankflag == 7)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(tankx, 0.0f, -325.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -tanktimer * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (tanktimer == 90)
		{
			tanktimer = 0;
			tankflag = 0;
		}
	}
	tankModelViewMatrix = ModelViewMatrix;
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, -15.0f, 0.0));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));


	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_tank();
}
void displaygouraud()
{
	////////////////////////거미 시작
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 0.0f, 100.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	if (flagg == 1 && light[0].light_on)
	{
		glUseProgram(h_ShaderProgram_gouraud);
		glUniform1i(loc_textureg, TEXTURE_ID_SPIDER);

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_gouraud, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_gouraud, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_gouraud, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else
	{
		glUseProgram(h_ShaderProgram_TXPS);
		set_material_tiger();//애니메이션
		glUniform1i(loc_texture, TEXTURE_ID_SPIDER);
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}

	draw_spider();

	glUseProgram(0);
}
void displayironmanandbike()
{
	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-150.0f, 0.0f, -150.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 30.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glUniform1f(loc_blind_frequency, blindfrequency);
	if (frequencyflag == 0)
	{
		blindfrequency = blindfrequency + 0.1;
		if (blindfrequency >= 1)
			blindfrequencyflag = 1;
	}
	else
	{
		blindfrequency = blindfrequency - 0.1;
		if (blindfrequency <= 0)
			blindfrequencyflag = 0;
	}
	glUniform1i(loc_blind_effect, flagw);
	if (flagw == 1)
	{
		changecolor(1);
	}
	draw_bike();
	changecolor(0);
	glUniform1i(loc_blind_effect, 0);

	////////////////////////////////
	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	if (flagt == 1)
		glUniform1i(loc_texture, TEXTURE_ID_MINE);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-230.0f, 0.0, -110.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 135.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	glUniform1f(loc_screen_frequency, frequency);
	if (frequencyflag == 0)
	{
		frequency = frequency + 0.2;
		if (frequency >= 20)
			frequencyflag = 1;
	}
	else
	{
		frequency = frequency - 0.2;
		if (frequency <= 1)
			frequencyflag = 0;
	}
	glUniform1i(loc_screen_effect, flage);
	draw_ironman();
	glUniform1i(loc_screen_effect, 0);
}
int wolfnextflag = 1;
void displaywolf()
{
	set_material_wolf();//애니메이션
	glUniform1i(loc_texture, TEXTURE_ID_WOLF);
	if (wolfflag == 0)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wolfx, wolfy, wolfz));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (lastangle+wolftimer) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		vectorM= glm::rotate(glm::mat4(1.0f), (wolftimer) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		nowv = glm::vec3(objectx, objecty, objectz) - glm::vec3(wolfx, wolfy, wolfz);
		nowv = glm::normalize(nowv);
		glm::vec3 temp = glm::mat3(vectorM) * lastv;
		float dotproduct = glm::dot(temp, nowv);
		if (dotproduct>0.9999f)
		{
			lastv = nowv;
			lastangle = lastangle + wolftimer;
			wolfflag = 1;
			wolftimer = 0;
		}


	}
	if (wolfflag == 1)
	{
		float z;
		if (wolfz < objectz)
			z = wolfz+wolftimer;
		else if (wolfz > objectz)
			z = wolfz-wolftimer;
		float temp = (wolfx - objectx) / (wolfz - objectz) * (z - wolfz) + wolfx;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(temp, 0.0f, z));
		ModelViewMatrix = glm::rotate(ModelViewMatrix,lastangle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (z == (int)(wolfz + objectz) / 2)
			wolfflag = 2;
	}
	if (wolfflag == 2)
	{
		float z;
		if (wolfz < objectz)
			z = wolfz+wolftimer;
		else if (wolfz > objectz)
			z = wolfz-wolftimer;
		float temp = (wolfx - objectx) / (wolfz - objectz) * (z - wolfz) + wolfx;
		float halfz = (wolfz + objectz) / 2;
		halfz = (halfz + objectz) / 2;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(temp, -(z - objectz) * (z - (wolfz + objectz) / 2) / 200, z));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, lastangle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (z == objectz)
		{
			wolfflag = 3;
			humanflag = 2;
			//humantimer = 0;
			humantimer2 = 0;
			spidertimer2 = 0;
			wolftimer = 0;
			wolfx = objectx;
			wolfy = objecty;
			wolfz = objectz;
			objectx = (float)(rand()%490);
			objecty = 0;
			objectz = (float)(((rand()+ 700 + timestamp_scene) % 250) + 200);
			count +=1;
			if (count == 9)
				count = 8;
			if (wolfnextflag == 1)
			{
				objectz = -objectz;
				wolfnextflag = 0;
			}
			else
			{
				wolfnextflag = 1;
			}
		}
	}
	if (wolfflag == 3)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(wolfx, wolfy, wolfz));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, lastangle * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_wolf();
}
float lasta;
void displayhuman()
{

	set_material_ben();//애니메이션
	glUniform1i(loc_texture, TEXTURE_ID_BEN);
	int i;

	for (i = 0; i < count; i++)
	{
		humantimer = humantimer % 359;
		ModelViewMatrix = ViewMatrix;
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -(humantimer + i * 45) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (humanflag == 2)
		{
			ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0, 100.0, 0.0));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, humantimer2 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0, -100.0, 0.0));
			if (humantimer2 == 360)
			{
				//humanflag = 0;
				humanflag = 1;
				wolfflag = 0;
				wolftimer = 0;
				spidertimer2 = 0;
			}
		}
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-100.0, 0.0, 0.0));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, -100.0f, -100.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		lasta= (-humantimer);
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_ben();
	}
}
void display(void) {
	glm::vec4 position_EC;
	glm::vec3 direction_EC;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(h_ShaderProgram_TXPS);
	//모델링좌표계에 있는 광원 눈좌표계로 옮기기
	position_EC = tankModelViewMatrix * glm::vec4(light[3].position[0], light[3].position[1], light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(tankModelViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1], light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	//세상좌표계에 있는 것들 눈좌표계로 옮기기
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0],
		light[1].spot_direction[1], light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
	position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light[0].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[0].spot_direction[0],
		light[0].spot_direction[1], light[0].spot_direction[2]);
	glUniform3fv(loc_light[0].spot_direction, 1, &direction_EC[0]);
	glUseProgram(0);

	glUseProgram(h_ShaderProgram_gouraud);
	//모델링좌표계에 있는 광원 눈좌표계로 옮기기
	position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_lightg[0].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[0].spot_direction[0],light[0].spot_direction[1], light[0].spot_direction[2]);
	glUniform3fv(loc_lightg[0].spot_direction, 1, &direction_EC[0]);
	glUseProgram(0);

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);

	glUseProgram(h_ShaderProgram_TXPS);
  	set_material_floor();
	glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, 500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();

	glUseProgram(h_ShaderProgram_TXPS);

	displayhuman();
	displaywolf();
	displayironmanandbike();
	displaytank();
	displayspider();
	glUseProgram(0);

	displaygouraud();

	glutSwapBuffers();
}

void timer_scene(int value) {
	
	timestamp_scene = (timestamp_scene + 1) % INT_MAX;
	cur_frame_tiger = timestamp_scene/2 % N_TIGER_FRAMES;
	//if(humanflag==2)
		cur_frame_ben = timestamp_scene/2 % N_BEN_FRAMES;
	cur_frame_wolf= timestamp_scene/3 % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	tanktimer = (tanktimer + 1) % INT_MAX;
	spidertimer1 = (spidertimer1 + 1) % INT_MAX;
	spidertimer2 = (spidertimer2 + 1) % INT_MAX;
	humantimer = (humantimer + 1) % INT_MAX;
	humantimer2 = (humantimer2 + 1) % INT_MAX;
	wolftimer = (wolftimer + 1) % INT_MAX;
	glutPostRedisplay();
	if (flag_tiger_animation)
		glutTimerFunc(10, timer_scene, 0);
}
int prevx;
int half;
void mousemove(int x, int y)
{
	glm::mat3 rotationM;
	if (leftbutton)
	{
		if (flag_camera == 0)
		{
			if (x > half)
			{
				camera.pos[0] += 2 * camera.uaxis[0];
				camera.pos[1] += 2 * camera.uaxis[1];
				camera.pos[2] += 2 * camera.uaxis[2];
			}
			else if (x < half)
			{
				camera.pos[0] -= 2 * camera.uaxis[0];
				camera.pos[1] -= 2 * camera.uaxis[1];
				camera.pos[2] -= 2 * camera.uaxis[2];
			}
		}
		else if (flag_camera == 1)
		{
			if (x > half)
			{
				camera.pos[0] += 2 * camera.vaxis[0];
				camera.pos[1] += 2 * camera.vaxis[1];
				camera.pos[2] += 2 * camera.vaxis[2];
			}
			else if (x < half)
			{
				camera.pos[0] -= 2 * camera.vaxis[0];
				camera.pos[1] -= 2 * camera.vaxis[1];
				camera.pos[2] -= 2 * camera.vaxis[2];
			}
		}
		else if (flag_camera == 2)
		{
			if (x > half)
			{
				camera.pos[0] += 2 * camera.naxis[0];
				camera.pos[1] += 2 * camera.naxis[1];
				camera.pos[2] += 2 * camera.naxis[2];
			}
			else if (x < half)
			{
				camera.pos[0] -= 2 * camera.naxis[0];
				camera.pos[1] -= 2 * camera.naxis[1];
				camera.pos[2] -= 2 * camera.naxis[2];
			}
		}
	}
	if (rightbutton)
	{
		if (flag_camera == 0)
		{
			if (x > half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), 0.2f * TO_RADIAN, glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2])));
			}
			else if (x < half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), -0.2f * TO_RADIAN, glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2])));
			}
		}
		else if (flag_camera == 1)
		{
			if (x > half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), -0.2f * TO_RADIAN, glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2])));
			}
			else if (x < half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), 0.2f * TO_RADIAN, glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2])));
			}
		}
		else if (flag_camera == 2)
		{
			if (x > half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), 0.2f * TO_RADIAN, glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2])));
			}
			else if (x < half)
			{
				rotationM = glm::mat3(glm::rotate(glm::mat4(1.0f), -0.2f * TO_RADIAN, glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2])));
			}
		}
		glm::vec3 temp = glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2]);
		temp = rotationM * temp;
		camera.uaxis[0] = temp.x;
		camera.uaxis[1] = temp.y;
		camera.uaxis[2] = temp.z;

		temp = glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2]);
		temp = rotationM * temp;
		camera.vaxis[0] = temp.x;
		camera.vaxis[1] = temp.y;
		camera.vaxis[2] = temp.z;

		temp = glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2]);
		temp = rotationM * temp;
		camera.naxis[0] = temp.x;
		camera.naxis[1] = temp.y;
		camera.naxis[2] = temp.z;

	}
	ViewMatrix = glm::mat4(camera.uaxis[0], camera.vaxis[0], camera.naxis[0], 0.0f,
		camera.uaxis[1], camera.vaxis[1], camera.naxis[1], 0.0f,
		camera.uaxis[2], camera.vaxis[2], camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-camera.pos[0], -camera.pos[1], -camera.pos[2]));
}
void mousepress(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		leftbutton = 1;
		prevx = x;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftbutton = 0;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rightbutton = 1;
		prevx = x;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		rightbutton = 0;
	}
}
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;

	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if ((key >= '0') && (key <= '3')) {
		int id= (int)(key - '0');
		light[id].light_on = 1 - light[id].light_on;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_light[id].light_on, light[id].light_on);
		glUseProgram(0);
		glUseProgram(h_ShaderProgram_gouraud);
		glUniform1i(loc_lightg[id].light_on, light[id].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'w':
		flagw = 1 - flagw;
		glutPostRedisplay();
		break;
	case 'e':
		flage = 1 - flage;
		glutPostRedisplay();
		break;
	case 't':
		flagt = 1 - flagt;
		glutPostRedisplay();
		break;
	case 'q':
		if (flagg == 0)
		{
			flagg = 1;
		}
		else if (flagg == 1)
		{
			flagg = 0;
		}
		glutPostRedisplay();
		break;
	case 'u':///////만약 0이면 u방향으로, 1이면 v방향으로, 2면 n방향으로
		flag_camera = 0;
		glutPostRedisplay();
		break;
	case 'v':
		flag_camera = 1;
		glutPostRedisplay();
		break;
	case 'n':
		flag_camera = 2;
		glutPostRedisplay();
		break;
	case 'r':
		if (flagr == 0)
			flagr = 1;
		else if (flagr == 1)
			flagr=0;
		if (flagr == 1)
		{

			glm::vec4 temp = glm::vec4(0, 0, 0, 1);
			tankcameraModelViewMatrix = glm::inverse(ViewMatrix) * tankModelViewMatrix;
			tankcameraModelViewMatrix = glm::translate(tankcameraModelViewMatrix, glm::vec3(0.0f, 70.0f, 5.0f));
			tankcameraModelViewMatrix = glm::rotate(tankcameraModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			temp = tankcameraModelViewMatrix * temp;
			glm::mat3 change = glm::mat3(tankcameraModelViewMatrix);
			camera.pos[0] = temp.x; camera.pos[1] = temp.y; camera.pos[2] = temp.z;
			glm::vec3 tempvector = glm::vec3(1.0f, 0.0f, 0.0f);
			tempvector = change * tempvector;
			camera.uaxis[0] = tempvector.x; camera.uaxis[1] = tempvector.y; camera.uaxis[2] = tempvector.z;
			tempvector = glm::vec3(0.0f, 1.0f, 0.0f);
			tempvector = change * tempvector;
			camera.vaxis[0] = tempvector.x; camera.vaxis[1] = tempvector.y; camera.vaxis[2] = tempvector.z;
			tempvector = glm::vec3(0.0f, 0.0f, 1.0f);
			tempvector = change * tempvector;
			camera.naxis[0] = tempvector.x; camera.naxis[1] = tempvector.y; camera.naxis[2] = tempvector.z;
			//ViewMatrix = glm::inverse(tankcameraModelViewMatrix);
			ViewMatrix = glm::mat4(camera.uaxis[0], camera.vaxis[0], camera.naxis[0], 0.0f,
				camera.uaxis[1], camera.vaxis[1], camera.naxis[1], 0.0f,
				camera.uaxis[2], camera.vaxis[2], camera.naxis[2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			camera.fovy = 70.0f;
			camera.near_c = 100.0f;
			camera.far_c = 20000.0f;
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-camera.pos[0], -camera.pos[1], -camera.pos[2]));
			ProjectionMatrix = glm::perspective(camera.fovy * TO_RADIAN, camera.aspect_ratio, camera.near_c, camera.far_c);
		}
		else
		{
			camera.uaxis[0] = 1.0f / sqrtf(2.0f); camera.uaxis[1] = 0.0f; camera.uaxis[2] = -1.0f / sqrtf(2.0f);
			camera.vaxis[0] = -1.0f / sqrt(6.0f); camera.vaxis[1] = 2.0f / sqrt(6.0f); camera.vaxis[2] = -1.0f / sqrt(6.0f);
			camera.naxis[0] = 1.0f / sqrt(3.0f); camera.naxis[1] = 1.0f / sqrt(3.0f); camera.naxis[2] = 1.0f / sqrt(3.0f);
			camera.pos[0] = 500.0f; camera.pos[1] = 500.0f; camera.pos[2] = 500.0f;
			ViewMatrix = glm::mat4(camera.uaxis[0], camera.vaxis[0], camera.naxis[0], 0.0f,
				camera.uaxis[1], camera.vaxis[1], camera.naxis[1], 0.0f,
				camera.uaxis[2], camera.vaxis[2], camera.naxis[2], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			camera.fovy = 70.0f;
			camera.near_c = 100.0f;
			camera.far_c = 20000.0f;
			ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-camera.pos[0], -camera.pos[1], -camera.pos[2]));
			ProjectionMatrix = glm::perspective(camera.fovy * TO_RADIAN, camera.aspect_ratio, camera.near_c, camera.far_c);
		}
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
	}
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);
	half = width / 2;
	camera.aspect_ratio = (float) width / height;
	ProjectionMatrix = glm::perspective(camera.fovy*TO_RADIAN, camera.aspect_ratio, camera.near_c, camera.far_c);
	glutPostRedisplay();
}
void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:
		if (camera.fovy < 90)
			camera.fovy = camera.fovy + 1;
		break;
	case GLUT_KEY_UP:
		if (camera.fovy > 15)
			camera.fovy = camera.fovy - 1;
		break;
	}
	ProjectionMatrix = glm::perspective(camera.fovy * TO_RADIAN, camera.aspect_ratio, camera.near_c, camera.far_c);
	glutPostRedisplay();
}
void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO); 
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &rectangle_VAO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutSpecialFunc(special);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_gouraud[3] = {
	{ GL_VERTEX_SHADER, "Shaders/gouraud.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/gouraud.frag" },
	{ GL_NONE, NULL }
	};
	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");
	////////////////////////////////////////
	h_ShaderProgram_gouraud = LoadShaders(shader_info_gouraud);
	loc_ModelViewProjectionMatrix_gouraud = glGetUniformLocation(h_ShaderProgram_gouraud, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_gouraud = glGetUniformLocation(h_ShaderProgram_gouraud, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_gouraud = glGetUniformLocation(h_ShaderProgram_gouraud, "u_ModelViewMatrixInvTrans");


	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_effect");
	loc_screen_frequency = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_frequency");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_blind_effect");


	loc_global_ambient_colorg = glGetUniformLocation(h_ShaderProgram_gouraud, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_lightg[i].light_on = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].position", i);
		loc_lightg[i].position = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_lightg[i].ambient_color = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_lightg[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_lightg[i].specular_color = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_lightg[i].spot_direction = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_lightg[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_lightg[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_gouraud, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_lightg[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_gouraud, string);
	}

	loc_materialg.ambient_color = glGetUniformLocation(h_ShaderProgram_gouraud, "u_material.ambient_color");
	loc_materialg.diffuse_color = glGetUniformLocation(h_ShaderProgram_gouraud, "u_material.diffuse_color");
	loc_materialg.specular_color = glGetUniformLocation(h_ShaderProgram_gouraud, "u_material.specular_color");
	loc_materialg.emissive_color = glGetUniformLocation(h_ShaderProgram_gouraud, "u_material.emissive_color");
	loc_materialg.specular_exponent = glGetUniformLocation(h_ShaderProgram_gouraud, "u_material.specular_exponent");

	loc_textureg = glGetUniformLocation(h_ShaderProgram_gouraud, "u_base_texture");

	loc_flag_texture_mappingg = glGetUniformLocation(h_ShaderProgram_gouraud, "u_flag_texture_mapping");

}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

		glUseProgram(h_ShaderProgram_TXPS);


	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]
	glUniform1i(loc_screen_effect, 0);
	glUniform1f(loc_screen_frequency, 1.0f);

	glUniform1i(loc_blind_effect, 0);

	glUseProgram(0);
	


	glUseProgram(h_ShaderProgram_gouraud);
	glUniform4f(loc_global_ambient_colorg, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_lightg[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_lightg[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_lightg[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_lightg[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_lightg[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_lightg[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_lightg[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_lightg[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_lightg[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_lightg[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_lightg[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_materialg.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_materialg.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_materialg.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_materialg.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_materialg.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_flags(void) {
	flag_tiger_animation = 1;
	flag_polygon_fill = 1;
	flag_texture_mapping = 1;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
	glUseProgram(h_ShaderProgram_gouraud);
	glUniform1i(loc_flag_texture_mappingg, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_MULTISAMPLE);


  	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	camera.uaxis[0] = 1.0f/sqrtf(2.0f);camera.uaxis[1] = 0.0f;camera.uaxis[2] = -1.0f / sqrtf(2.0f);
	camera.vaxis[0] = -1.0f/sqrt(6.0f);camera.vaxis[1] = 2.0f/ sqrt(6.0f);camera.vaxis[2] = -1.0f/ sqrt(6.0f);
	camera.naxis[0] = 1.0f/sqrt(3.0f);camera.naxis[1] = 1.0f / sqrt(3.0f);camera.naxis[2] = 1.0f / sqrt(3.0f);
	camera.pos[0] = 500.0f; camera.pos[1] = 500.0f; camera.pos[2] = 500.0f;
	ViewMatrix = glm::mat4(camera.uaxis[0], camera.vaxis[0], camera.naxis[0], 0.0f,
		camera.uaxis[1], camera.vaxis[1], camera.naxis[1], 0.0f,
		camera.uaxis[2], camera.vaxis[2], camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	camera.fovy = 70.0f;
	camera.near_c = 100.0f;
	camera.far_c = 20000.0f;
	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-camera.pos[0], -camera.pos[1], -camera.pos[2]));
	//ViewMatrix = glm::lookAt(4.0f/6.0f * glm::vec3(500.0f, 600.0f, 500.0f),glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	initialize_lights_and_material();	
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	
	//0번 광원
	light[0].light_on = 1;
	light[0].position[0] = -400.0f; light[0].position[1] = 400.0f; 	// point light position in WC
	light[0].position[2] = -400.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 57/255.0f; light[0].ambient_color[1] = 57 / 255.0f;
	light[0].ambient_color[2] = 57 / 255.0f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 210 / 255.0f; light[0].diffuse_color[1] = 210 / 255.0f;
	light[0].diffuse_color[2] = 210 / 255.0f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 240/255.0f; light[0].specular_color[1] = 240 / 255.0f;;
	light[0].specular_color[2] = 240 / 255.0f;; light[0].specular_color[3] = 1.0f;


	//1번 광원
	light[1].light_on = 1;
	light[1].position[0] = 0.0f; light[1].position[1] = 150.0f; // spot light position in WC
	light[1].position[2] = -300.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 55 / 255.0f; light[1].ambient_color[1] = 69 / 255.0f;
	light[1].ambient_color[2] = 78 / 255.0f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 151 / 255.0f; light[1].diffuse_color[1] = 241/ 255.0f;
	light[1].diffuse_color[2] = 250 / 255.0f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.8; light[1].specular_color[1] = 0.8f;
	light[1].specular_color[2] = 0.8f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = -1.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 1.0f;
	light[1].spot_cutoff_angle = 30.0f;
	light[1].spot_exponent = 10.0f;

	//2번 광원
	light[2].light_on = 0;
	light[2].position[0] = 0.0f; light[2].position[1] = 0.0f;
	light[2].position[2] = 10.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.140f; light[2].ambient_color[1] = 0.140f;
	light[2].ambient_color[2] = 0.140f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 139/255.0f; light[2].diffuse_color[1] = 191 / 255.0f;
	light[2].diffuse_color[2] = 231 / 255.0f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 139/255.0f; light[2].specular_color[1] = 191 / 255.0f;
	light[2].specular_color[2] = 231 / 255.0f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = 0.0f;
	light[2].spot_direction[2] = -1.0f;
	light[2].spot_cutoff_angle = 15.0f;
	light[2].spot_exponent = 70.0f;

	//3번 광원
	light[3].light_on = 0;
	light[3].position[0] = 0.0f; light[3].position[1] = 10.0f;
	light[3].position[2] = 0.0f; light[3].position[3] = 1.0f;

	light[3].ambient_color[0] = 0.0f; light[3].ambient_color[1] = 0.0f;
	light[3].ambient_color[2] = 0.0f; light[3].ambient_color[3] = 1.0f;

	light[3].diffuse_color[0] = 233 / 255.0f; light[3].diffuse_color[1] = 201 / 255.0f;
	light[3].diffuse_color[2] = 1 / 255.0f; light[3].diffuse_color[3] = 1.0f;

	light[3].specular_color[0] = 0.9f; light[3].specular_color[1] = 0.9f;
	light[3].specular_color[2] = 0.9f; light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = 0.0f;
	light[3].spot_direction[2] = 1.0f;
	light[3].spot_cutoff_angle = 20.0f;
	light[3].spot_exponent = 5.0f;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light[0].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	/////////////////////////////////////////////
	glUniform1i(loc_light[1].light_on, light[1].light_on);
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]); 
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]); 
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	////////////////////////////
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	glUniform4fv(loc_light[2].position, 1, light[2].position);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	glUniform3fv(loc_light[2].spot_direction, 1, light[2].spot_direction);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);
	////////////////////////////
	glUniform1i(loc_light[3].light_on, light[3].light_on);
	position_EC = ViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1],light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);
	glUseProgram(0);

	glUseProgram(h_ShaderProgram_gouraud);
	glUniform1i(loc_lightg[0].light_on, light[0].light_on);
	position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_lightg[0].position, 1, &position_EC[0]);
	glUniform4fv(loc_lightg[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_lightg[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_lightg[0].specular_color, 1, light[0].specular_color);

	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_tiger();
	prepare_ben();
	prepare_wolf();
	prepare_spider();
	prepare_dragon();
	prepare_optimus();
	prepare_cow();
	prepare_bus();
	prepare_bike();
	prepare_godzilla();
	prepare_ironman();
	prepare_tank();
	set_up_scene_lights();
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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects";
	char messages[N_MESSAGE_LINES][256] = { ""  };

	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}