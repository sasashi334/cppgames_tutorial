#pragma once

#include "SDL.h"

struct Vector2
{
	float x;
	float y;
};

class Game
{

public:
	Game();//コンストラクタ
	//ゲームの初期化
	bool Initialize();
	//ゲームオーバーまでループ
	void RunLoop();
	//ゲームをシャットダウンする
	void Shutdown();

private:
	//ゲームループのためのヘルパー関数
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	//SDLが作るウィンドウ
	SDL_Window* mWindow;

	SDL_Renderer* mRenderer;
	//ゲームの続行を指示する
	bool mIsRunning;

	//パドルの移動
	int mPaddleDir;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	//パドルの位置
	Vector2 mPaddlePos;

	// ボールの位置
	Vector2 mBallPos;
	//ボールの速度
	Vector2 mBallVel;

};