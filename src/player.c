#include "player.h"
#include "gl.h"

#include <string.h>

#define ROT_AMOUNT (M_PI / 4.0) // amount that rotation will change each frame the controls are pressed
#define SPEED_AMOUNT 1.0 // amount that speed will change each frame the controls are pressed

/*
 * Update the player's position and velocity with frametime dt
 */
static bool integratePlayer(Player* player, float dt) {
	// update velocity with gravity
	player->vel.y -= player->g * dt;

	// this gives a pretty reasonable integration of acceleration with each step
	player->pos.y += player->vel.y * dt + 0.5f * player->g * dt * dt;
	player->pos.x += player->vel.x * dt;
	player->pos.z += player->vel.z * dt;

	// when we have finished jumping, reset for next jump
	if (player->pos.y <= 0) {
		player->pos.y = 0;
		player->initPos = player->pos;
		return false;
	}
	return true;
}

void initJoints(float * joints) {
	const float initJoints[n_joints] = { 
						10.0, // body 
						0.0, // mouth
						30.0, // shoulder
						-50.0, // elbow
						160.0, // waist
						50.0, // knee
						130.0 }; // ankle

	memcpy(joints, initJoints, sizeof(initJoints));
}

void initPreItps(Interpolator * itps, float velY, float g) {
	float t = 2.0 * velY / g;
	const Interpolator preItps[n_joints] = 
	{
		{ // body
			2,
			{
				{0.0, 10.0}, 
				{t, 30.0}
			},
			-1.0
		},

		{ // mouth
			2,
			{
				{0.0, 0.0}, 
				{t, 0.0}
			},
			-1.0
		},

		{ // shoulder
			2,
			{
				{0.0, 30.0}, 
				{t, 60.0}
			},
			-1.0
		},

		{ // elbow
			2,
			{
				{0.0, -50.0}, 
				{t, -110.0}
			},
			-1.0
		},

		{ // waist
			2,
			{
				{0.0, 160.0}, 
				{t, 160.0}
			},
			-1.0
		},

		{ // knee
			2,
			{
				{0.0, 50.0}, 
				{t, 50.0}
			},
			-1.0
		},

		{ // ankle
			2,
			{
				{0.0, 130.0}, 
				{t, 110.0}
			},
			-1.0
		}
	};
	
	memcpy(itps, preItps, sizeof(preItps));
}

void initJumpItps(Interpolator * itps, float velY, float g) {
	float t = 2.0 * velY / g;
	const Interpolator jumpItps[n_joints] = 
	{
		{ // body
			4,
			{
				{0.0, 30.0},
				{t / 3.0, 50.0},
				{t * 2.0 / 3.0, 50.0},
				{t, 10.0}
			},
			-1.0
		},

		{ // mouth
			2,
			{
				{0.0, 0.0}, 
				{t, 0.0}
			},
			-1.0
		},

		{ // shoulder
			4,
			{
				{0.0, 60.0}, 
				{t / 3.0, 0.0},
				{t * 2.0 / 3.0, 0.0},
				{t, 30.0}
			},
			-1.0
		},

		{ // elbow
			4,
			{
				{0.0, -110.0}, 
				{t / 3.0, -150.0},
				{t * 2.0 / 3.0, -150.0},
				{t, -50.0}
			},
			-1.0
		},

		{ // waist
			4,
			{
				{0.0, 160.0},
				{t / 3.0, 80.0},
				{t * 2.0 / 3.0, 80.0},
				{t, 160.0}
			},
			-1.0
		},

		{ // knee
			4,
			{
				{0.0, 50.0}, 
				{t / 3.0, 90.0},
				{t * 2.0 / 3.0, 90.0},
				{t, 50.0}
			},
			-1.0
		},

		{ // ankle
			4,
			{
				{0.0, 110.0}, 
				{t / 3.0, 50.0},
				{t * 2.0 / 3.0, 50.0},
				{t, 130.0}
			},
			-1.0
		}
	};
	
	memcpy(itps, jumpItps, sizeof(jumpItps));
}

/*
 * Initialise the player
 */
void initPlayer(Player* player) {
	player->pos = (Vec3f) { 0, 0, 4 };
	player->initPos = player->pos;

	player->xRot = M_PI / 4.0;
	player->yRot = M_PI;
	player->speed = 2.0;
	player->size = 0.05;
	player->g = 9.8;
	player->jump = false;
	player->onLog = false;
	player->prepare = false;
	player->ribbit = false;

	player->mesh = createCube();
	player->material = (Material) { { 0.2, 0.2, 0.2, 0 }, { 0, 1, 0, 0 }, { 1, 1, 1, 0 }, 50 };

	initJoints(player->joints);
	
	initPreItps(player->preItps, player->initVel.y, player->g);
	initJumpItps(player->jumpItps, player->initVel.y, player->g);
	player->ribbitItp = (Interpolator)
						{ // mouth
							3,
							{
								{0.0, 0.0}, 
								{0.1, 20.0},
								{0.2, 0.0}
							},
							-1.0
						};
}

/*
 * Cleanup any memory used by the player
 */
void destroyPlayer(Player* player) {
	destroyMesh(player->mesh);
}

bool playerAnimation(float elapsedTime, Interpolator * interpolators, float * joints) {
	bool isStop = false;
	for (int i = 0; i < n_joints; i++) {
		if (i == mouth) continue;
		isStop = animate(elapsedTime, interpolators[i], &joints[i]);
	}
	return isStop;
}

bool ribbitAnimation(float elapsedTime, Interpolator itp, float * joint) {
	bool isStop = false;
	isStop = animate(elapsedTime, itp, joint);
	return isStop;
}

/*
 * Update the player's state for frametime dt.
 * If we aren't jumping, update speed and rotation from controls, otherwise animate the next step of our jump
 */
void updatePlayer(Player* player, float dt, Controls* controls, float elapsedTime) {
	static bool isSetStartJump = false;
	static bool isJump = false;
	if (!player->jump) {
		// process controls
		if (controls->up && player->speed < 3.0)
			player->speed += SPEED_AMOUNT * dt;
		if (controls->down)
			player->speed -= SPEED_AMOUNT * dt;
		if (controls->left && player->xRot < 1.56)
			player->xRot += ROT_AMOUNT * dt;
		if (controls->right)
			player->xRot -= ROT_AMOUNT * dt;
		if (controls->turnLeft)
			player->yRot += ROT_AMOUNT * dt;
		if (controls->turnRight)
			player->yRot -= ROT_AMOUNT * dt;
		

		// dont let the player jump backwards or into the floor
		player->speed = max(0, player->speed);
		player->xRot = clamp(player->xRot, 0, M_PI);

		// set our initial velocity from speed and rotation
		player->initVel.x = cosf(player->xRot) * sinf(player->yRot);
		player->initVel.y = sinf(player->xRot);
		player->initVel.z = cosf(player->yRot) * cosf(player->xRot);
		player->initVel = mulVec3f(player->initVel, player->speed);
		player->vel = player->initVel;

		if (controls->jump) {
			player->jump = true;
			player->prepare = true;
			initPreItps(player->preItps, player->initVel.y, player->g);
			initJumpItps(player->jumpItps, player->initVel.y, player->g);
			for (int i = 0; i < n_joints; i++) {
				player->preItps[i].startTime = elapsedTime;
			}
		}

	}
	else {
		if (player->prepare) {
			player->prepare = playerAnimation(elapsedTime, player->preItps, player->joints);
		} 
		else {
			if (!isSetStartJump) {
				for (int i = 0; i < n_joints; i++) {
					player->jumpItps[i].startTime = elapsedTime;
					isJump = true;
				}
			}
			isSetStartJump = playerAnimation(elapsedTime, player->jumpItps, player->joints);
			if (!isSetStartJump) {
				initJoints(player->joints);
				player->jump = false;
			}
			if (isJump) {
				isJump = integratePlayer(player, dt);
			}
		}
	}
	if (!player->ribbit) {
		float randomInterval = getTRand(0.0, 1.0);
		if (randomInterval >= 0.99) {
			player->ribbit = true;
			player->ribbitItp.startTime = elapsedTime;
		}
	}
	if (player->ribbit) {
		player->ribbit = ribbitAnimation(elapsedTime, player->ribbitItp, &player->joints[mouth]);
		if (!player->ribbit) {
			player->joints[mouth] = 0.0;
		}
	}
}

void renderEye(Mesh * cube, DrawingFlags * flags) {
	Material gray = { { 0.2, 0.2, 0.2, 0 }, { 0.5, 0.5, 0.5, 0 }, { 1, 1, 1, 0 }, 50 };
	Material black = { { 0.0, 0.0, 0.0, 0 }, { 0.0, 0.0, 0.0, 0 }, { 1, 1, 1, 0 }, 50 };

	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glPushMatrix();
		glScalef(0.15, 0.15, 0.15);
		applyMaterial(&gray);
		submitColor(GRAY);
		renderMesh(cube, flags);

		applyMaterial(&black);
		submitColor(BLACK);
		glTranslatef(0, 0, 0.5);
		glScalef(0.7, 0.7, 0.7);
		renderMesh(cube, flags);
	glPopMatrix();

	glPopAttrib();
}

void renderHead(Material green, Mesh * cube, DrawingFlags * flags, float mouth) {
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glPushMatrix();
		applyMaterial(&green);
		submitColor(GREEN);
		glPushMatrix();
			glScalef(0.4, 0.35, 0.2);
			renderMesh(cube, flags);
		glPopMatrix();
		
		//eyes;
		glPushMatrix();
			glTranslatef(0.0, 0.15, 0.17);
			//right
			glPushMatrix();
				glTranslatef(-0.2, 0, 0);
				renderEye(cube, flags);
			glPopMatrix();
			//left
			glPushMatrix();
				glTranslatef(0.2, 0, 0);
				renderEye(cube, flags);
			glPopMatrix();
		glPopMatrix();

		//mouth
		submitColor(RED);
		
		glPushMatrix();
			glScalef(0.03, 0.03, 0.03);
			renderMesh(cube, flags);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.0, -0.15, 0.2);
			glPushMatrix();
				glRotatef(mouth, -1, 0, 0);
				glTranslatef(0.0, 0.09, 0.2);
				applyMaterial(&green);
				submitColor(GREEN);
				glScalef(0.4, 0.09, 0.2);
				renderMesh(cube, flags);
			glPopMatrix();

			glPushMatrix();
				glRotatef(mouth, 1, 0, 0);
				glTranslatef(0.0, -0.09, 0.2);
				glScalef(0.4, 0.09, 0.2);
				renderMesh(cube, flags);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	glPopAttrib();
}

void renderFoot(Material green, Mesh * cube, DrawingFlags * flags, float length) {
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	applyMaterial(&green);
	submitColor(GREEN);

	glPushMatrix();
		glScalef(0.15, 0.15, length);
		renderMesh(cube, flags);
	glPopMatrix();
	
	//toes
	glPushMatrix();
		glTranslatef(0, -0.1, length + 0.1);
		// middle
		glPushMatrix();
			glScalef(0.05, 0.05, 0.2);
			renderMesh(cube, flags);
		glPopMatrix();

		// left
		glPushMatrix();
			glTranslatef(0.1, 0.0, 0.0);
			glRotatef(20, 0, 1, 0);
			glScalef(0.05, 0.05, 0.2);
			renderMesh(cube, flags);
		glPopMatrix();

		// right
		glPushMatrix();
			glTranslatef(-0.1, 0.0, 0.0);
			glRotatef(20, 0, -1, 0);
			glScalef(0.05, 0.05, 0.2);
			renderMesh(cube, flags);
		glPopMatrix();
	glPopMatrix();

	glPopAttrib();
}

void renderFrontLeg(Material green, Mesh * cube, DrawingFlags * flags, float elbow) {
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	applyMaterial(&green);
	submitColor(GREEN);

	glPushMatrix();
		glPushMatrix();
			glScalef(0.15, 0.15, 0.3);
			renderMesh(cube, flags);
		glPopMatrix();

		glTranslatef(0.0, 0.0, -0.3);
		glRotatef(elbow, -1, 0, 0);

		glTranslatef(0.0, 0.0, 0.3);
		renderFoot(green, cube, flags, 0.3);
	glPopMatrix();

	glPopAttrib();
}

void renderRearLeg(Material green, Mesh * cube, DrawingFlags * flags,
					float knee, float ankle) {
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	applyMaterial(&green);
	submitColor(GREEN);

	glPushMatrix();
		glPushMatrix();
			glScalef(0.15, 0.15, 0.4);
			renderMesh(cube, flags);
		glPopMatrix();

		glTranslatef(0.0, 0.0, -0.4);
		glRotatef(knee, -1, 0, 0);

		glTranslatef(0.0, 0.0, 0.3);
		glPushMatrix();
			glScalef(0.15, 0.15, 0.3);
			renderMesh(cube, flags);
		glPopMatrix();

		glTranslatef(0.0, 0.0, 0.3);
		glRotatef(ankle, -1, 0, 0);

		glTranslatef(0.0, 0.0, 0.5);
		renderFoot(green, cube, flags, 0.5);
	glPopMatrix();

	glPopAttrib();
}

/*
 * Render a car
 */
void renderFrog(Player * player, DrawingFlags* flags) {
	Mesh * cube = player->mesh;
	Material green = player->material;

	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glPushMatrix();
		glTranslatef(0.0, 1.0, 0.5); // to be on the ground
		// frog's torso
		glRotatef(player->joints[body], -1, 0, 0);
		glPushMatrix();
			applyMaterial(&green);
			submitColor(GREEN);
			glTranslatef(0.0, 0.0, -0.35);
			glScalef(0.4, 0.4, 0.8);
			renderMesh(cube, flags);
		glPopMatrix();

		// frog's head
		glPushMatrix();
			glTranslatef(0.0, 0.0 , 0.55);
			renderHead(green, cube, flags, player->joints[mouth]);
		glPopMatrix();

		// frog's front left leg
		glPushMatrix();
			glTranslatef(0.4, -0.35, 0.4);
			glRotatef(player->joints[shoulder], -1, 0, 0);

			glTranslatef(0.13, -0.15, -0.25);
			applyMaterial(&green);
			submitColor(GREEN);

			glPushMatrix();
				glRotatef(10, 0, 1, 0);
				glRotatef(10, 0, 0, 1);
				renderFrontLeg(green, cube, flags, player->joints[elbow]);
			glPopMatrix();
		glPopMatrix();

		// frog's front right leg
		glPushMatrix();
			glTranslatef(-0.27, -0.35, 0.4);
			glRotatef(player->joints[shoulder], -1, 0, 0);

			glTranslatef(-0.13, -0.15, -0.25);
			applyMaterial(&green);
			submitColor(GREEN);

			glPushMatrix();
				glTranslatef(-0.13, 0.0, 0.0);
				glRotatef(10, 0, -1, 0);
				glRotatef(10, 0, 0, -1);
				renderFrontLeg(green, cube, flags, player->joints[elbow]);
			glPopMatrix();
		glPopMatrix();

		// frog's rear left leg
		glPushMatrix();
			glTranslatef(0.4, 0.0, -1.15);
			glRotatef(player->joints[waist], -1, 0, 0);

			glTranslatef(0.15, 0.0, -0.4);
			applyMaterial(&green);
			submitColor(GREEN);

			glPushMatrix();
				glRotatef(30, 0, -1, 0);
				glRotatef(10, 0, 0, -1);
				renderRearLeg(green, cube, flags, player->joints[knee], player->joints[ankle]);
			glPopMatrix();
		glPopMatrix();

		// frog's rear right leg
		glPushMatrix();
			glTranslatef(-0.4, 0.0, -1.15);
			glRotatef(player->joints[waist], -1, 0, 0);

			glTranslatef(-0.15, 0.0, -0.4);
			applyMaterial(&green);
			submitColor(GREEN);

			glPushMatrix();
				glRotatef(30, 0, 1, 0);
				glRotatef(10, 0, 0, 1);
				renderRearLeg(green, cube, flags, player->joints[knee], player->joints[ankle]);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	glPopAttrib();
}

/*
 * Draw the player's mesh, as well as a parabola showing our jump arc and a visualization of our current velocity
 */
void renderPlayer(Player* player, DrawingFlags* flags) {
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	// draw the parabola from the starting point of the jump
	glPushMatrix();
	glTranslatef(player->initPos.x, player->initPos.y, player->initPos.z);
	drawParabola(BLUE, player->initVel, player->g, flags);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(player->pos.x, player->pos.y, player->pos.z);

	// draw the player mesh at our current position
	glPushMatrix();
	glRotatef(RADDEG(player->yRot), 0, 1, 0);
	glScalef(player->size, player->size, player->size);
	applyMaterial(&player->material);
	glColor3f(0.1, 0.5, 0.9);
	renderFrog(player, flags);
	glPopMatrix();
	
	// draw the visualization of the player's velocity at our current position
	glBegin(GL_LINES);
	drawLine(PURPLE, (Vec3f) { 0, 0, 0 }, mulVec3f(player->vel, 0.1)); 
	glEnd();
	glPopMatrix();

	glPopAttrib();
}
