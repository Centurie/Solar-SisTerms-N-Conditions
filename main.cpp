#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <cassert>

constexpr int screenWidth = 1260;
constexpr int screenHeight = 850;
constexpr float gravity = 1.8f;

namespace ballSettings
{
	int ballRad{50};
	float ballX{ static_cast<float>(screenWidth) / 2 };
	float ballY{ static_cast<float>(screenHeight) / 2 };
	Color ballColor = RED;

	constexpr float cRestitution{ 0.7f };
	int nBounce{ 0 };
	static_assert(cRestitution <= 1, "Coeff of Restitution cannot be greater than 1");
}

float calculateMaxVelocity(float ballPosY)
{
	float heightGained = std::max(0.0f, (screenHeight - ballSettings:: ballRad) - ballPosY);
	return std::sqrt(2.0f * gravity * heightGained)* std::pow(ballSettings::cRestitution, ballSettings::nBounce);
}

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_TOPMOST);
	float velocity{ 0 };

	InitWindow(screenWidth, screenHeight, "RayLib, Everything OPENGL but PACKAGED");

	Vector2 ballPos{ ballSettings::ballX, ballSettings::ballY };

	SetTargetFPS(64);
	HideCursor();
	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_H))
		{
			if (IsCursorHidden()) ShowCursor();
			else HideCursor();
		}
		if (!IsCursorHidden()) {
			ballPos = GetMousePosition(); 
			ballSettings::nBounce = 0;
			velocity = 0;
			ballSettings::ballY = ballPos.y;
			ballSettings::ballX = ballPos.x;
		}
		else velocity += gravity;


		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ballSettings:: ballColor = MAROON;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) ballSettings::ballColor = LIME;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) ballSettings::ballColor = DARKBLUE;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_SIDE)) ballSettings::ballColor = PURPLE;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_EXTRA)) ballSettings::ballColor = YELLOW;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_FORWARD)) ballSettings::ballColor = ORANGE;
		else if (IsMouseButtonPressed(MOUSE_BUTTON_BACK)) ballSettings::ballColor = BEIGE;

		if (ballPos.y >= screenHeight - ballSettings::ballRad)
		{
			ballSettings::nBounce += 1;
			ballPos.y = screenHeight - ballSettings::ballRad;
			velocity = -calculateMaxVelocity(ballSettings::ballY);

		}
		ballPos.y += velocity;

		if (IsKeyDown(KEY_RIGHT)) { ballPos.x += 3.0f; };
		if (IsKeyDown(KEY_LEFT)) { ballPos.x -= 3.0f; };
		if (IsKeyDown(KEY_UP)) { ballPos.y -= 3.0f; };
		if (IsKeyDown(KEY_DOWN)) { ballPos.y += 3.0f; };

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(TextFormat("Pos: [%.2f, %.2f]", ballPos.x, ballPos.y), ballPos.x + 70, ballPos.y - 70, 22, DARKGRAY);
		DrawText("My first raylib code.", 10, screenHeight - 90, 24, RED);
		DrawText("Press \"H\" to show / hide cursor", 10, screenHeight - 60, 24, RED);
		DrawCircleV(ballPos, ballSettings::ballRad, ballSettings::ballColor);
		EndDrawing();
	}

	CloseWindow();	

	return 0;	
}