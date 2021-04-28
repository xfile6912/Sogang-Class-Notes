#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

int rightbuttonpressed = 0;//마우스 오른쪽 버튼이 눌린상태인지에 대한 flag//눌려잇으면1
float linecolorR = 0.5f;
float linecolorG = 0.5f;
float linecolorB = 0.5f;
float linestartx = -1.0f, linestarty = 0.0f;
float lineendx = 0.0f, lineendy = 0.0f;
int middle = 250;
int leftbuttonpressed = 0;
//float r = 0.0f, g = 0.0f, b = 0.0f; // Backgroud color = Black
float r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon

void display(void) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(linecolorR, linecolorG, linecolorB);
	glBegin(GL_LINES);
	glVertex2f(linestartx, linestarty); glVertex2f(lineendx, lineendy);
	glEnd();
	glFinish();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		r = 1.0f; g = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'g':
		g = 1.0f; r = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'b':
		b = 1.0f; r = g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	case 'f'://SHIFT+F를 눌렀을 때 LINE의 시작좌표와 끝좌표가 출력됨.
	case 'F'://SHIFT+f를 눌렀을때 line의 시작좌표와 끝좌표가 출력됨.
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
		{
			int startx = (int)(linestartx * middle + middle);
			int starty = (int)(-1 * linestarty * middle + middle);
			int endx = (int)(lineendx * middle + middle);
			int endy = (int)(-1 * lineendy * middle + middle);
			fprintf(stdout, "$$$ The startpoint of the line is (x, y)=(%d, %d) and endpoint of the line is (x, y)=(%d, %d).\n", startx, starty, endx, endy);
		}
		break;
	}

}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		r -= 0.1f;
		if (r < 0.0f) r = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		r += 0.1f;
		if (r > 1.0f) r = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		g -= 0.1f;
		if (g < 0.0f) g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		g += 0.1f;
		if (g > 1.0f) g = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	}

}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		leftbuttonpressed = 1;
		fprintf(stdout, "*** The left mouse button was pressed at (%d, %d).\n", x, y);
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			linestartx = (float)(x - middle) / middle;
			linestarty = 0 - (float)(y - middle) / middle;
			glutPostRedisplay();
		}
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
	{
		leftbuttonpressed = 0;
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		rightbuttonpressed = 1;
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		rightbuttonpressed = 0;
}

void mousemove(int x, int y) {
	if (rightbuttonpressed)
		fprintf(stdout, "$$$ The right mouse button is now at (%d, %d).\n", x, y);
	if (glutGetModifiers() == GLUT_ACTIVE_ALT && leftbuttonpressed)//ALT와 마우스왼쪽버튼을 누른채로 X좌표 250을 기준으로 오른쪽에서 마우스를 움직이면 line이 점점 하얘짐
	{//250을 기준으로 왼쪽에서 마우스를 움직이면 점점 어두워짐
		if (middle < x)
		{
			linecolorR += 0.01f;
			linecolorG += 0.01f;
			linecolorB += 0.01f;
			if (linecolorR > 1.0f)
				linecolorR = 1.0f;
			if (linecolorG > 1.0f)
				linecolorG = 1.0f;
			if (linecolorB > 1.0f)
				linecolorB = 1.0f;
		}
		else if (x < middle)
		{
			linecolorR -= 0.01f;
			linecolorG -= 0.01f;
			linecolorB -= 0.01f;
			if (linecolorR < 0.0f)
				linecolorR = 0.0f;
			if (linecolorG < 0.0f)
				linecolorG = 0.0f;
			if (linecolorB < 0.0f)
				linecolorB = 0.0f;
		}
		glutPostRedisplay();
	}
}

void reshape(int width, int height) {
	fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
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

#define N_MESSAGE_LINES 4
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	//   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}