﻿#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"
#include "Button.h"
#include "Character.h"
#include "Enemy.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
const std::string LAYER[BACKGROUND_LAYER] = {
	"res/img/background/layer01.png",
	"res/img/background/layer02.png",
	"res/img/background/layer03.png",
	"res/img/background/layer04.png",
	"res/img/background/layer05.png",
	"res/img/background/layer06.png",
	"res/img/background/layer07.png",
	"res/img/background/layer08.png",
	"res/img/background/layer09.png",
};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Color textColor = { 0, 0, 0 };
TTF_Font* gFont = NULL;
Mix_Music* gMusic = NULL;
Mix_Music* gMenuMusic = NULL;
Mix_Chunk* gClick = NULL;
Mix_Chunk* gJump = NULL;
Mix_Chunk* gLose = NULL;

SDL_Rect gPlayButton[BUTTON_TOTAL];
SDL_Rect gHelpButton[BUTTON_TOTAL];
SDL_Rect gExitButton[BUTTON_TOTAL];
SDL_Rect gBackButton[BUTTON_TOTAL];
SDL_Rect gPauseButton[BUTTON_TOTAL];
SDL_Rect gContinueButton[BUTTON_TOTAL];
SDL_Rect gPlayAgainButton[BUTTON_TOTAL];
SDL_Rect gCharacterClips[RUNNING_FRAMES];
SDL_Rect gEnemyClips[FLYING_FRAMES];

LTexture gMenuTexture;
LTexture gInstructionTexture;
LTexture gBackgroundTexture[BACKGROUND_LAYER];
LTexture gCharacterTexture;
LTexture gGroundTexture;
LTexture gPlayButtonTexture;
LTexture gHelpButtonTexture;
LTexture gExitButtonTexture;
LTexture gBackButtonTexture;
LTexture gPauseButtonTexture;
LTexture gContinueButtonTexture;
LTexture gLoseTexture;
LTexture gText1Texture;
LTexture gScoreTexture;
LTexture gText2Texture;
LTexture gHighScoreTexture;

Button PlayButton(PLAY_BUTON_POSX, PLAY_BUTTON_POSY);
Button HelpButton(HELP_BUTTON_POSX, HELP_BUTTON_POSY);
Button ExitButton(EXIT_BUTTON_POSX, EXIT_BUTTON_POSY);
Button BackButton(BACK_BUTTON_POSX, BACK_BUTTON_POSY);
Button PauseButton(PAUSE_BUTTON_POSX, PAUSE_BUTTON_POSY);
Button ContinueButton(CONTINUE_BUTTON_POSX, CONTINUE_BUTTON_POSY);

Character character;

int main(int argc, char* argv[])
{
	if (!Init())
	{
		std::cout << "Failed to initialize!";
	}
	else
	{
		if (!LoadMedia())
		{
			std::cout << "Failed to load media!";
		}
		else
		{
			bool Quit_Menu = false;
			bool Play_Again = false;

			Mix_PlayMusic(gMenuMusic, IS_REPEATITIVE);
			while (!Quit_Menu)
			{
				SDL_Event e_mouse;
				while (SDL_PollEvent(&e_mouse) != 0)
				{
					if (e_mouse.type == SDL_QUIT)
					{
						Quit_Menu = true;
					}

					bool Quit_Game = false;
					HandlePlayButton(&e_mouse, PlayButton, Quit_Menu, Play_Again, gClick);
						
					HandleHelpButton(&e_mouse, gBackButton,
									 HelpButton, BackButton, 
									 gInstructionTexture, gBackButtonTexture,
									 gRenderer, Quit_Game, gClick);

					HandleExitButton(&e_mouse, ExitButton, Quit_Menu, gClick);

					if (Quit_Game == true)
					{
						return 0;
					}
				}

				gMenuTexture.Render(0, 0, gRenderer);

				SDL_Rect* currentClip_Play = &gPlayButton[PlayButton.currentSprite];
				PlayButton.Render(currentClip_Play, gRenderer, gPlayButtonTexture);

				SDL_Rect* currentClip_Help = &gHelpButton[HelpButton.currentSprite];
				HelpButton.Render(currentClip_Help, gRenderer, gHelpButtonTexture);

				SDL_Rect* currentClip_Exit = &gExitButton[ExitButton.currentSprite];
				ExitButton.Render(currentClip_Exit, gRenderer, gExitButtonTexture);

				SDL_RenderPresent(gRenderer);
			}

			while (Play_Again)
			{
				srand(time(NULL));
				int time = 0;
				int score = 0;
				int acceleration = 0;
				int frame_Character = 0;
				int frame_Enemy = 0;
				std::string highscore = GetHighScoreFromFile("high_score.txt");
				
				SDL_Event e;
				Enemy enemy1(ON_GROUND_ENEMY);
				Enemy enemy2(ON_GROUND_ENEMY);
				Enemy enemy3(IN_AIR_ENEMY);
				
				Mix_PlayMusic(gMusic, IS_REPEATITIVE);
				GenerateEnemy(enemy1, enemy2, enemy3, gEnemyClips, gRenderer);

				int OffsetSpeed_Ground = BASE_OFFSET_SPEED;
				std::vector <double> OffsetSpeed_Bkgr(BACKGROUND_LAYER, BASE_OFFSET_SPEED);				

				bool Quit = false;
				bool Game_State = true;
				while (!Quit)
				{
					if (Game_State)
					{
						UpdateGameTimeAndScore(time, acceleration, score);

						while (SDL_PollEvent(&e) != 0)
						{
							if (e.type == SDL_QUIT)
							{
								Quit = true;
								Play_Again = false;
							}

							HandlePauseButton(&e, gRenderer, gContinueButton,
								PauseButton, ContinueButton,
								gContinueButtonTexture, Game_State, gClick);

							character.HandleEvent(e, gJump);
						}
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
						SDL_RenderClear(gRenderer);

						RenderScrollingBackground(OffsetSpeed_Bkgr, gBackgroundTexture, gRenderer);
						RenderScrollingGround(OffsetSpeed_Ground, acceleration, gGroundTexture, gRenderer);


						character.Move();
						SDL_Rect* currentClip_Character = NULL;
						if (character.OnGround())
						{
							currentClip_Character = &gCharacterClips[frame_Character / SLOW_FRAME_CHAR];
							character.Render(currentClip_Character, gRenderer, gCharacterTexture);
						}
						else
						{
							currentClip_Character = &gCharacterClips[0];
							character.Render(currentClip_Character, gRenderer, gCharacterTexture);
						}


						enemy1.Move(acceleration);
						enemy1.Render(gRenderer);
			
						enemy2.Move(acceleration);
						enemy2.Render(gRenderer);

						SDL_Rect* currentClip_Enemy = &gEnemyClips[frame_Enemy / SLOW_FRAME_ENEMY];
						enemy3.Move(acceleration);
						enemy3.Render(gRenderer, currentClip_Enemy);


						SDL_Rect* currentClip_Pause = &gPauseButton[PauseButton.currentSprite];
						PauseButton.Render(currentClip_Pause, gRenderer, gPauseButtonTexture);


						DrawPlayerScore(gText1Texture, gScoreTexture, textColor, gRenderer, gFont, score);
						DrawPlayerHighScore(gText2Texture, gHighScoreTexture, textColor, gRenderer, gFont, highscore);

						if (CheckEnemyColission(character,
							enemy1, enemy2, enemy3,
							currentClip_Character, currentClip_Enemy))
						{
							Mix_PauseMusic();
							Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
							UpdateHighScore("highscore.txt", score, highscore);
							Quit = true;
						}


						SDL_RenderPresent(gRenderer);

						ControlCharFrame(frame_Character);
						ControlEnemyFrame(frame_Enemy);
					}
				}

				DrawEndGameSelection(gLoseTexture, &e, gRenderer, Play_Again);
				if (!Play_Again)
				{
					enemy1.~Enemy();
					enemy2.~Enemy();
					enemy3.~Enemy();
				}
			}
		}
	}
	Close();

	return 0;
}


bool Init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO) < 0)
	{
		LogError("Can not initialize SDL.", SDL_ERROR);
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			std::cout << "Warning: Linear texture filtering not enabled!";
		}

		gWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								   SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			LogError("Can not create window", SDL_ERROR);
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				LogError("Can not create renderer", SDL_ERROR);
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					LogError("Can not initialize SDL_image", IMG_ERROR);
					success = false;
				}

				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " <<  Mix_GetError();
					success = false;
				}

				if (TTF_Init() == -1)
				{
					std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " <<  TTF_GetError();
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia()
{
	bool success = true;

	gMusic = Mix_LoadMUS("res/sound/background_audio.wav");
	if (gMusic == NULL)
	{
		LogError("Failed to load background music", MIX_ERROR);
		success = false;
	}

	gMenuMusic = Mix_LoadMUS("res/sound/menu_audio.wav");
	if (gMenuMusic == NULL)
	{
		LogError("Failed to load menu music", MIX_ERROR);
		success = false;
	}

	gClick = Mix_LoadWAV("res/sound/mouse_click.wav");
	if (gClick == NULL)
	{
		LogError("Failed to load mouse click sound", MIX_ERROR);
		success = false;
	}

	gJump = Mix_LoadWAV("res/sound/jump_sound.wav");
	if (gJump == NULL)
	{
		LogError("Failed to load jumping sound", MIX_ERROR);
		success = false;
	}

	gLose = Mix_LoadWAV("res/sound/lose_sound.wav");
	if (gLose == NULL)
	{
		LogError("Failed to load lose sound", MIX_ERROR);
		success = false;
	}

	else
	{
		gFont = TTF_OpenFont("font/pixel_font.ttf", 28);
		if (gFont == NULL)
		{
			LogError("Failed to load font", MIX_ERROR);
			success = false;
		}
		else
		{
			if (!gText1Texture.LoadFromRenderedText("Your score: ", gFont, textColor, gRenderer))
			{
				std::cout << "Failed to render text1 texture" << std::endl;
				success = false;
			}

			if (!gText2Texture.LoadFromRenderedText("High score: ", gFont, textColor, gRenderer))
			{
				std::cout << "Failed to render text2 texture" << std::endl;
				success = false;
			}
			
			if (!gMenuTexture.LoadFromFile("res/img/background/menu.png", gRenderer))
			{
				std::cout << "Failed to load menu image" << std::endl;
				success = false;
			}

			if (!gInstructionTexture.LoadFromFile("res/img/background/instruction.png", gRenderer))
			{
				std::cout << "Failed to load instruction image" << std::endl;
				success = false;
			}

			if (!gPlayButtonTexture.LoadFromFile("res/img/button/big_button/play_button.png", gRenderer))
			{
				std::cout << "Failed to load play_button image" << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gPlayButton[i].x = 150 * i;
					gPlayButton[i].y = 0;
					gPlayButton[i].w = 150;
					gPlayButton[i].h = 98;
				}
			}

			if (!gHelpButtonTexture.LoadFromFile("res/img/button/big_button/help_button.png", gRenderer))
			{
				std::cout << "Failed to load help_button image" << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gHelpButton[i].x = 150 * i;
					gHelpButton[i].y = 0;
					gHelpButton[i].w = 150;
					gHelpButton[i].h = 98;
				}
			}

			if (!gBackButtonTexture.LoadFromFile("res/img/button/big_button/back_button.png", gRenderer))
			{
				std::cout << "Failed to load back_button image" << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gBackButton[i].x = 100 * i;
					gBackButton[i].y = 0;
					gBackButton[i].w = 100;
					gBackButton[i].h = 78;
				}
			}

			if (!gExitButtonTexture.LoadFromFile("res/img/button/big_button/exit_button.png", gRenderer))
			{
				std::cout << "Failed to load exit_button image" << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gExitButton[i].x = 150 * i;
					gExitButton[i].y = 0;
					gExitButton[i].w = 150;
					gExitButton[i].h = 98;
				}
			}

			if (!gPauseButtonTexture.LoadFromFile("res/img/button/big_button/pause_button.png", gRenderer))
			{
				std::cout << "Failed to load pause_button image " << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gPauseButton[i].x = 22 * i;
					gPauseButton[i].y = 0;
					gPauseButton[i].w = 22;
					gPauseButton[i].h = 34;
				}
			}

			if (!gContinueButtonTexture.LoadFromFile("res/img/button/big_button/continue_button.png", gRenderer))
			{
				std::cout << "Failed to load continue_button image " << std::endl;
				success = false;
			}
			else
			{
				for (int i = 0; i < BUTTON_TOTAL; ++i)
				{
					gContinueButton[i].x = 22 * i;
					gContinueButton[i].y = 0;
					gContinueButton[i].w = 22;
					gContinueButton[i].h = 34;
				}
			}

			for (int i = 0; i < BACKGROUND_LAYER; ++i)
			{
				if (!gBackgroundTexture[i].LoadFromFile(LAYER[i].c_str(), gRenderer))
				{
					std::cout << "Failed to load background image" << std::endl;
					success = false;
				}
			}

			if (!gGroundTexture.LoadFromFile("res/img/background/ground.png", gRenderer))
			{
				std::cout << "Failed to load ground image" << std::endl;
				success = false;
			}

			if (!gCharacterTexture.LoadFromFile("res/img/character/char.png", gRenderer))
			{
				std::cout << "Failed to load character_run image." << std::endl;
				success = false;
			}
			else
			{
				gCharacterClips[0].x = 57 * 0;
				gCharacterClips[0].y = 0;
				gCharacterClips[0].w = 57;
				gCharacterClips[0].h = 57;

				gCharacterClips[1].x = 57 * 1;
				gCharacterClips[1].y = 0;
				gCharacterClips[1].w = 57;
				gCharacterClips[1].h = 57;

				gCharacterClips[2].x = 57 * 2;
				gCharacterClips[2].y = 0;
				gCharacterClips[2].w = 57;
				gCharacterClips[2].h = 57;

				gCharacterClips[3].x = 57 * 3;
				gCharacterClips[3].y = 0;
				gCharacterClips[3].w = 57;
				gCharacterClips[3].h = 57;

				gCharacterClips[4].x = 57 * 4;
				gCharacterClips[4].y = 0;
				gCharacterClips[4].w = 57;
				gCharacterClips[4].h = 57;

				gCharacterClips[5].x = 57 * 5;
				gCharacterClips[5].y = 0;
				gCharacterClips[5].w = 57;
				gCharacterClips[5].h = 57;
			}

			if (!gLoseTexture.LoadFromFile("res/img/background/lose.png", gRenderer))
			{
				std::cout << "Failed to load lose image." << std::endl;
				success = false;
			}
		}
	}
	return success;
}

void Close()
{
	gMenuTexture.Free();
	gInstructionTexture.Free();
	gCharacterTexture.Free();
	gGroundTexture.Free();
	gPlayButtonTexture.Free();
	gHelpButtonTexture.Free();
	gExitButtonTexture.Free();
	gBackButtonTexture.Free();
	gPauseButtonTexture.Free();
	gContinueButtonTexture.Free();
	gLoseTexture.Free();
	gText1Texture.Free();
	gScoreTexture.Free();
	gText2Texture.Free();
	gHighScoreTexture.Free();

	for (int i = 0; i < BACKGROUND_LAYER; ++i)
	{
		gBackgroundTexture[i].Free();
	}

	Mix_FreeMusic(gMusic);
	Mix_FreeMusic(gMenuMusic);
	Mix_FreeChunk(gClick);
	Mix_FreeChunk(gLose);
	Mix_FreeChunk(gJump);
	gMusic = NULL;
	gMenuMusic = NULL;
	gClick = NULL;
	gLose = NULL;
	gJump = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
}
