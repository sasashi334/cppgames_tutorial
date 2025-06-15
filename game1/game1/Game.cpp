#include "Game.h"
#include <random>

const int thickness = 15;
const int width = 1024;
const int height = 700;
const float paddleH = 100.0f;

//コンストラクタ
Game::Game() : mWindow(nullptr), mRenderer(nullptr), mIsRunning(true), mTicksCount(0),mPaddleDir(0)
{

}

bool Game::Initialize()
{
	//SDLの初期化
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if(sdlResult != 0)
	{
		SDL_Log("SDLを初期化できません: %s", SDL_GetError());
		return false;
	}

	//SDLウィンドウを作成
	mWindow = SDL_CreateWindow(
		"Game Programming in C++",//ウィンドウのタイトル
		100,//ウィンドウ左上のx座標
		100,//ウィンドウ左上のy座標
		width,//ウィンドウの幅wigth
		height,//ウィンドウの高さheight
		0//フラグ(設定しない場合は0)
	);

	if (!mWindow)
	{
		SDL_Log("ウィンドウの作成に失敗しました: %s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(
		mWindow, // 作成するレンダラーの描画対象となるウィンドウ
		-1,		 // 通常は -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> uniform_real(50.0f, 500.0f);
	mBallVel.x = uniform_real(gen);
	mBallVel.y = uniform_real(gen);
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_UP])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	//deltaTimeは前のフレームとの時間差を秒に変換した値
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	//最大値を0.5秒に
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	//時刻を更新(次のフレームのために)
	mTicksCount = SDL_GetTicks();

	//パドルの位置
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// 画面外に出ないように
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (height - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = height - paddleH / 2.0f - thickness;
		}
	}
	// ボール位置更新
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;


	float diff = mPaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// Our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}
	// Did the ball collide with the right wall?
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}

	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (height - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}

}

void Game::GenerateOutput()
{
	//青セット
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A不透明度
	);

	//バックバッファをクリア
	SDL_RenderClear(mRenderer);
	//上壁を描画
	//白設定
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_Rect wall
	{
		0,			// 左上 x
		0,			// 左上 y
		1024,		// 幅
		thickness	// 高さ
	};
	SDL_RenderFillRect(mRenderer, &wall);

	//下壁を描画
	wall.y = height - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// 右壁を描画
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	// ボール描画
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);


	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	// フロントバッファとバックバッファの交換
	SDL_RenderPresent(mRenderer);
}



void Game::Shutdown()
{
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}