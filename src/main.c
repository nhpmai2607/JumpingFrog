#include "gl.h"
#include "util.h"
#include "state.h"
#include "player.h"
#include "level.h"
#include "anim.h"
#include "skybox.h"
#include "particles.h"

/*
------------------------------------
Assignment 3 version 4.0 29/05/2017
------------------------------------

COSC1187 - Interactive 3D Graphics & Animation
Semester A 2017
Lecturer & Coordinator: Geoff Leach

Student Name: Mai Nguyen Ha Phuong
Student ID  : s3558475
*/

Globals globals;

static void cleanup() {
	destroyParticles(&globals.particles);
	destroySkybox(&globals.skybox);
	destroyPlayer(&globals.player);
	destroyLevel(&globals.level);
}

static void updateKeyChar(unsigned char key, bool state)
{
	switch (key)
	{
		case 'w':
			globals.controls.up = state;
			break;
		case 's':
			globals.controls.down = state;
			break;
		case 'a':
			globals.controls.left = state;
			break;
		case 'd':
			globals.controls.right = state;
			break;
		case ' ':
			globals.controls.jump = state;
		default:
			break;
	}
}

static void updateKeyInt(int key, bool state) {
	switch (key) {
		case GLUT_KEY_LEFT:
			globals.controls.turnLeft = state;
			break;
		case GLUT_KEY_RIGHT:
			globals.controls.turnRight = state;
			break;
		default:
			break;
	}
}

static void reshape(int width, int height) {
	glViewport(0,0, width, height);
	globals.camera.width = width;
	globals.camera.height = height;
	applyProjectionMatrix(&globals.camera);
}

void renderOSD()
{
	char buffer[30];
	char *bufp;
	int w, h, count;
	int textPosY = 15;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/* Set up orthographic coordinate system to match the 
	 window, i.e. (0,0)-(w,h) */
	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	/* Frame rate */
	submitColor(YELLOW);
	glRasterPos2i(10, 60);
	snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", globals.frameRate);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	/* Time per frame */
	submitColor(YELLOW);
	glRasterPos2i(10, 40);
	snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / globals.frameRate * 1000.0);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	/* Name */
	submitColor(GREEN);
	count = snprintf(buffer, sizeof buffer, "Frogger");
	glRasterPos2f((w - count * 9)/ 2.0 , h - textPosY);
	textPosY += 18;
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	
	/* Lives left */
	submitColor(GREEN);
	count = snprintf(buffer, sizeof buffer, "Lives left: %d", globals.lives);
	glRasterPos2f((w - count * 9)/ 2.0 , h - textPosY);
	textPosY += 18;
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	/* Score */
	submitColor(GREEN);
	count = snprintf(buffer, sizeof buffer, "Score: %d", globals.score);
	glRasterPos2f((w - count * 9)/ 2.0 , h - textPosY);
	textPosY += 18;
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

	/* Game Over */
	if (globals.lives == 0) {
		submitColor(PURPLE);
		count = snprintf(buffer, sizeof buffer, "Game Over");
		glRasterPos2f((w - count * 9)/ 2.0 , h / 2 + 12);
		for (bufp = buffer; *bufp; bufp++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *bufp);
	}
	/* Pop modelview */
	glPopMatrix();  
	glMatrixMode(GL_PROJECTION);

	/* Pop projection */
	glPopMatrix();  
	glMatrixMode(GL_MODELVIEW);

	/* Pop attributes */
	glPopAttrib();
}

static void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	applyViewMatrix(&globals.camera);

	static float lightPos[] = { 1, 1, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glPushMatrix();
		glLoadIdentity();
		glRotatef(globals.camera.yRot, 1, 0, 0);
		glRotatef(globals.camera.xRot, 0, 1, 0);
		renderSkybox(&globals.skybox, &globals.drawingFlags);
	glPopMatrix();

	renderLevel(&globals.level, &globals.drawingFlags);
	renderPlayer(&globals.player, &globals.drawingFlags);
	if (globals.particles.spawn) {
		renderParticles(&globals.particles, &globals.drawingFlags);
	}

	renderOSD();

	glutSwapBuffers();
	globals.frames++;
}

static void resetGame()
{
	initCamera(&globals.camera);
	initPlayer(&globals.player);
	initLevel(&globals.level, &globals.drawingFlags);
	globals.camera.pos = globals.player.pos;
}

static void checkCrossRiver()
{
	River river = globals.level.river;
	float riverSidePos = river.pos.z - river.logs->size.x;
	if (globals.player.pos.z < riverSidePos) {
		globals.score++;
		resetGame();
	}
}

static void checkInRiver()
{
	River river = globals.level.river;
	float riverTopSide = river.pos.z - river.logs->size.x;
	float riverBottomSide = river.pos.z + river.laneHeight - river.logs->size.x;
	if (globals.player.pos.y == 0 && !globals.player.onLog &&
			globals.player.pos.z > riverTopSide && globals.player.pos.z < riverBottomSide) {
		globals.lives--;
		resetGame();
	}
}

static void attachFrogOnLog(Entity log)
{
	Player * frog = &globals.player;
	static Vec3f posOnLog = { 0.0, 0.0, 0.0 };
	static bool isJump = false;
	if (!frog->onLog) {
		posOnLog = (Vec3f) {log.pos.x - frog->pos.x, log.pos.y - frog->pos.y, log.pos.z - frog->pos.z};
		frog->onLog = true;
		isJump = frog->jump;
	}

	// skip the jump which moves to the log
	if (isJump) { 
		if (frog->jump) {
			return;
		} else {
			isJump = frog->jump;
		}
	}

	if (!frog->jump) {
		frog->pos.x = log.pos.x - posOnLog.x;
		frog->pos.y = log.pos.y - posOnLog.y;
		frog->pos.z = log.pos.z - posOnLog.z;
		frog->initPos = frog->pos;
	} else {
		frog->onLog = false;
	}
}

static bool checkEnemiesCollision()
{
	bool isCollided = false;
	float distance;
	float enemyRadius = globals.level.road.enemies->size.x * 1.41421356; // sqrt(2) = 1.41421356;
	float overlap = (globals.player.size + enemyRadius) * (globals.player.size + enemyRadius);

	Vec3f frog = globals.player.pos;
	Vec3f enemy;

	Entity * enemies = globals.level.road.enemies;

	for (size_t i = 0; i < globals.level.road.numLanes; ++i) {
		enemy = enemies->pos;
		distance = (frog.x - enemy.x) * (frog.x - enemy.x) + 
					(frog.y - enemy.y) * (frog.y - enemy.y) +
					(frog.z - enemy.z) * (frog.z - enemy.z);
		if (distance < overlap) {
			isCollided = true;
			break;
			
		}
		enemies++;
	}
	return isCollided;
}

static bool checkLogsCollision()
{
	Vec3f frog = globals.player.pos;
	Vec3f log;

	bool isCollided = false;

	Entity * logs = globals.level.river.logs;
	float logLength = logs->size.z;
	float logHeight = logs->size.y;
	for (size_t i = 0; i < globals.level.river.numLanes; ++i) {
		log = logs->pos;
		Vec3f minPoint = {log.x - logLength / 2.0, 0.0, log.z - logHeight};
		Vec3f maxPoint = {log.x + logLength / 2.0, 0.0, log.z + logHeight};

		if (frog.x >= minPoint.x && frog.x <= maxPoint.x
				&& frog.y <= logHeight
				&& frog.z >= minPoint.z && frog.z <= maxPoint.z) {
			attachFrogOnLog(*logs);
			isCollided = true;
			break;
		}
		logs++;
		}
	return isCollided;
}

static void checkOutBoundary()
{
	Vec3f * frog = &globals.player.pos;
	float posBoundary = globals.level.width / 2;
	float negBoundary = -globals.level.width / 2;

	if (frog->x < negBoundary) {
		frog->x = negBoundary;
	} else if (frog->x > posBoundary) {
		frog->x = posBoundary;
	}

	if (frog->z < negBoundary) {
		frog->z = negBoundary;
	} else if (frog->z > posBoundary) {
		frog->z = posBoundary;
	}
}


static void update()
{
	static int tLast = -1;
	
	if (tLast < 0)
		tLast = glutGet(GLUT_ELAPSED_TIME);

	int t = glutGet(GLUT_ELAPSED_TIME);
	int dtMs = t - tLast;
	float dt = (float)dtMs / 1000.0f;
	tLast = t;

	bool enemyCollided = false;
	bool logCollided = false;

	if (globals.lives == 0) {
		globals.halt = true;
	}

	if (!globals.halt) {
		updatePlayer(&globals.player, dt, &globals.controls, t / 1000.0f);
		updateLevel(&globals.level, dt);

		enemyCollided = checkEnemiesCollision();
		updateParticles(&globals.particles, enemyCollided, globals.player.pos, dt);
		if (enemyCollided) {
			globals.lives--;
			resetGame();
		}
			
		logCollided = checkLogsCollision();
		if (!logCollided) { // when the log that frog is attached on disappears, onLog -> false
			globals.player.onLog = false;
			if (!globals.player.jump) {
				globals.player.pos.y = 0.0; // frog falls into river when the log disappears
			}
		}
		checkInRiver();

		checkCrossRiver();
		checkOutBoundary();
		globals.camera.pos = globals.player.pos;
	};

	/* Frame rate */
	dt = t / 1000.0f - globals.lastFrameRateT;
	if (dt > globals.frameRateInterval) { // after frameRateInterval, calculate frameRate again
		globals.frameRate = globals.frames / dt;
		globals.lastFrameRateT = t / 1000.0f;
		globals.frames = 0;
	}

	glutPostRedisplay();
}

static void keyDown(unsigned char key, int x, int y)
{
	UNUSED(x);
	UNUSED(y);

	switch (key)
	{
		case 27:
		case 'q':
			cleanup();
			exit(EXIT_SUCCESS);
			break;
		case 'h':
			globals.halt = !globals.halt;
			if (globals.halt)
				printf("Stopping time\n");
			else
				printf("Resuming time\n");
			break;
		case 'l':
			globals.drawingFlags.lighting = !globals.drawingFlags.lighting;
			printf("Toggling lighting\n");
			break;
		case 't':
			globals.drawingFlags.textures = !globals.drawingFlags.textures;
			printf("Toggling textures\n");
			break;
		case 'n':
			globals.drawingFlags.normals = !globals.drawingFlags.normals;
			printf("Toggling normals\n");
			break;
		case 'o':
			globals.drawingFlags.axes = !globals.drawingFlags.axes;
			printf("Toggling axes\n");
			break;
		case 'p':
			globals.drawingFlags.wireframe = !globals.drawingFlags.wireframe;
			if (globals.drawingFlags.wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				printf("Using wireframe rendering\n");
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				printf("Using filled rendering\n");
			}
			break;
		case '+':
		case '=':
			globals.drawingFlags.segments = clamp(globals.drawingFlags.segments * 2, 8, 1024);
			generateLevelGeometry(&globals.level, globals.drawingFlags.segments);
			printf("Tesselation: %zu\n", globals.drawingFlags.segments);
			break;
		case '-':
			globals.drawingFlags.segments = clamp(globals.drawingFlags.segments / 2, 8, 1024);
			generateLevelGeometry(&globals.level, globals.drawingFlags.segments);
			printf("Tesselation: %zu\n", globals.drawingFlags.segments);
			break;
		default:
			updateKeyChar(key, true); // player controls are updated here and processed on each frame
			break;
	}

	glutPostRedisplay();
}

static void keyUp(unsigned char key, int x, int y)
{
	UNUSED(x);
	UNUSED(y);

	updateKeyChar(key, false);
}

static void specialKeyDown(int key, int x, int y)
{
	UNUSED(x);
	UNUSED(y);

	switch(key)
	{
	default:
		updateKeyInt(key, true);
		break;
	}

	glutPostRedisplay();
}

static void specialKeyUp(int key, int x, int y)
{
	UNUSED(x);
	UNUSED(y);

	updateKeyInt(key, false);
}

static void mouseMotion(int x, int y) {
	int dX = x - globals.camera.lastX;
	int dY = y - globals.camera.lastY;

	if (globals.controls.lmb) {
		globals.camera.xRot += dX * 0.1;
		globals.camera.yRot += dY * 0.1;
		globals.camera.yRot = clamp(globals.camera.yRot, 0, 90);
	}
	
	if (globals.controls.rmb) {
		globals.camera.zoom += dY * 0.01;
		globals.camera.zoom = max(globals.camera.zoom, 0.5);
	}

	globals.camera.lastX = x;
	globals.camera.lastY = y;

	glutPostRedisplay();
}

static void mouseButton(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		globals.camera.lastX = x;
		globals.camera.lastY = y;
	}

	if (button == GLUT_LEFT_BUTTON) {
		globals.controls.lmb = state == GLUT_DOWN;
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		globals.controls.rmb = state == GLUT_DOWN;
	}

	glutPostRedisplay();
}

static void init() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	globals.drawingFlags.segments = 8;
	globals.drawingFlags.wireframe = false;
	globals.drawingFlags.textures = true;
	globals.drawingFlags.lighting = true;

	resetGame();
	initSkybox(&globals.skybox);
	globals.camera.width = 800;
	globals.camera.height = 600;
	
	globals.score = 0;
	globals.lives = 5;
	
	globals.halt = false;
	globals.frames = 0;
	globals.frameRate = 0.0;
	globals.frameRateInterval = 0.2;
	globals.lastFrameRateT = 0.0;

	initParticles(&globals.particles, &globals.drawingFlags);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Assignment 3_s3558475");

	glutDisplayFunc(render);
	glutIdleFunc(update);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialKeyDown);
	glutSpecialUpFunc(specialKeyUp);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseButton);
	glutReshapeFunc(reshape);

	init();

	glutMainLoop();

	return EXIT_SUCCESS;
}
