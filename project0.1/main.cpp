#include<SDL.h>
#include<string>
#include<SDL_image.h>
#include<Windows.h>
#include<iostream>
#include<ctime>
#include<SDL_mixer.h>
#include<SDL_ttf.h>

#undef main

using namespace std;

Mix_Chunk* chunk = NULL;
Mix_Music* music = NULL;

TTF_Font* font = NULL;

SDL_Renderer* screen;
SDL_Window* window;
SDL_Event events;
SDL_Rect rect;

const int SCREEN_HEIGHT = 507;
const int SCREEN_WIDTH = 900;
const int SPEED = 7;
const int COLOR_KEY = 255;
const int WIDTH_IMG_PLAYER = 300;
const int HEIGHT_IMG_PLAYER = 64;
const int FRAME_NUMBER = 5;
const int WIDTH_BIRD = 310;
const int HEIGHT_BIRD = 64;

int y_star = SCREEN_HEIGHT - HEIGHT_IMG_PLAYER - 30;
int y_pos = SCREEN_HEIGHT - HEIGHT_IMG_PLAYER - 30;
int x_pos = 150;
int y_val = 0;
bool check_jump = false;
bool check_down = false;
bool check_star_cactus = true;
bool checkChunk = false;

const int cactus_number = 4;
const int range_max = 400;
const int range_min = 200;


bool InitData() // khởi tạo các thông số cho môi trường SDL
{

	bool success = true;
	int ret = SDL_Init(SDL_INIT_VIDEO);
	if (ret < 0)
		return false;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // thiết lập chế độ tỉ lệ về chất lượng

	// tạo của sổ window cho game
	window = SDL_CreateWindow("code game 2d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
		success = false;
	else
	{
		screen = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (screen == NULL)
			success = false;
		else
		{
			SDL_SetRenderDrawColor(screen, 0xff, 0xff, 0xff, 0xff);
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) && imgFlags))
				success = false;
		}
	}

	if (TTF_Init() < 0)
	{
		SDL_Log("%s", TTF_GetError());
		success = false;
	}

	font = TTF_OpenFont("font//dlxfont_.ttf", 15);
	if (font == NULL)
		success = false;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		success = false;
	}


	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	rect.x = 0;
	rect.y = 0;



	return success;
}


bool load_img(SDL_Renderer* screen, string path, SDL_Rect* rect_img, SDL_Rect* rect)
{
	SDL_Surface* load_surface = NULL;
	SDL_Texture* texture = NULL;

	load_surface = IMG_Load(path.c_str());

	if (load_surface == NULL)
		return false;


	texture = SDL_CreateTextureFromSurface(screen, load_surface);

	if (texture == NULL)
		return false;


	SDL_RenderCopy(screen, texture, rect_img, rect);

	SDL_FreeSurface(load_surface);
	load_surface = NULL;

	SDL_DestroyTexture(texture);
	texture = NULL;

	return true;
}


SDL_Rect loadBackGroung(int& x_pos)
{
	if (x_pos >= SCREEN_WIDTH)
		x_pos = 0;

	else
		x_pos += SPEED;

	SDL_Rect rect;
	rect.x = x_pos;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;

	return rect;
}


class Cactus
{
public:
	Cactus()
	{
		castus_y_pos = SCREEN_HEIGHT - 80;
		castus_x_pos = 0;
		rect_img.x = 0;
		rect_img.y = 0;
		rect_img.w = 91;
		rect_img.h = 138;
	}

	int castus_x_pos;
	int castus_y_pos;
	SDL_Rect rect;
	SDL_Rect rect_img;
	void castus_move()
	{
		castus_x_pos -= SPEED;
		rect.x = castus_x_pos;
		rect.y = castus_y_pos;
		rect.w = rect_img.w;
		rect.h = 50;
	}

	void load_img_castus()
	{
		load_img(screen, "img//cactus.bmp", &rect_img, &rect);
	}

};

Cactus cactus[cactus_number];

bool checkCollision(Cactus cactus[])
{
	bool check_collision = true;
	for (size_t i = 0; i < cactus_number; i++)
	{
		if (x_pos <= cactus[i].castus_x_pos + 91 && x_pos + WIDTH_IMG_PLAYER / FRAME_NUMBER - 50 >= cactus[i].castus_x_pos && y_pos >= cactus[i].castus_y_pos - 45)
		{
			check_collision = false;
			break;
		}
	}


	return check_collision;
}


void move_player()
{

	if (y_pos == y_star)
	{
		if (events.type == SDL_KEYDOWN)
		{
			if (events.key.keysym.sym == SDLK_UP)
			{
				y_val = -17;
				check_jump = true;
				checkChunk = true;
			}
		}
	}
	if (y_pos <= y_star - 30 - 200)
	{
		y_val = 15;
	}

	if (y_pos > y_star)
	{
		y_val = 0;
		y_pos = y_star;
		check_jump = false;
		check_down = false;
	}

	if (check_jump)
		y_pos += y_val;


}

bool loadPlayerAnimation(SDL_Renderer* screen, int& a, Cactus cactus[])
{

	SDL_Rect frame[FRAME_NUMBER + 1];
	const int width_player = WIDTH_IMG_PLAYER / FRAME_NUMBER;

	for (size_t i = 0; i < FRAME_NUMBER; i++)
	{
		frame[i].x = i * width_player;
		frame[i].y = 0;
		frame[i].w = width_player;
		frame[i].h = HEIGHT_IMG_PLAYER;
	}

	frame[FRAME_NUMBER].w = WIDTH_IMG_PLAYER / 10;
	frame[FRAME_NUMBER].h = HEIGHT_IMG_PLAYER;
	frame[FRAME_NUMBER].x = x_pos;
	frame[FRAME_NUMBER].y = y_pos;


	if (a >= FRAME_NUMBER - 1)
		a = 0;

	else
		a++;

	bool check = false;

	move_player();

	if (check_jump)
		check = load_img(screen, "img//player.bmp", &frame[1], &frame[FRAME_NUMBER]);
	else

		check = load_img(screen, "img//player.bmp", &frame[a], &frame[FRAME_NUMBER]);


	return check;

}



void handingCactus(Cactus cactus[])
{
	for (size_t i = 0; i < cactus_number; i++)
	{
		if (cactus[i].castus_x_pos <= 0)
		{
			while (true)
			{
				int temp = abs(rand() % (2 * SCREEN_WIDTH + 1)) + SCREEN_WIDTH;

				bool check = true;
				for (size_t j = 0; j < cactus_number; j++)
				{
					if (i != j)
					{
						if (abs(temp - cactus[j].castus_x_pos) <= 200)
						{
							check = false;
							break;
						}
					}
				}
				if (check)
				{
					cactus[i].castus_x_pos = temp;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < cactus_number; i++)
	{
		cactus[i].castus_move();
	}
	for (size_t i = 0; i < cactus_number; i++)
	{
		cactus[i].load_img_castus();
	}

}

void reset()
{
	check_jump = false;

 y_star = SCREEN_HEIGHT - HEIGHT_IMG_PLAYER - 30;
 y_pos = SCREEN_HEIGHT - HEIGHT_IMG_PLAYER - 30;
 x_pos = 150;
 y_val = 0;
bool check_star_cactus = true;
bool checkChunk = false;


	for (size_t i = 0; i < cactus_number; i++)
	{
		cactus[i].castus_x_pos = 0;
	}
}

bool loadSound()
{
	chunk = Mix_LoadWAV("sound//jump.mp3");
	if (chunk == NULL)
	{
		return false;
	}

	music = Mix_LoadMUS("sound//mario.mp3");
	if (music == NULL)
	{
		return false;
	}

	return true;
}

void loadText(std::string str_val, SDL_Rect rectText,const int color)
{
	SDL_Color color1 = { 243, 156, 18 };
	SDL_Color color2 = { 255,0,0 };

	SDL_Surface* surface = NULL;

	if(color==1)
		 surface = TTF_RenderText_Solid(font, str_val.c_str(), color1);
	if(color==2)
		 surface = TTF_RenderText_Solid(font, str_val.c_str(), color2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(screen, surface);
	SDL_FreeSurface(surface);

	SDL_Rect temp;
	TTF_SizeText(font, str_val.c_str(), &temp.w, &temp.h);


	temp.x = 0;
	temp.y = 0;

	rectText.w = temp.w;
	rectText.h = temp.h;

	SDL_RenderCopy(screen, texture, &temp, &rectText);
}

bool checkFocus(const int& x, const int& y, const SDL_Rect& rect)
{
	if (x >= rect.x && x <= rect.x + 200 && y >= rect.y && y <= rect.y + 30)
		return true;

	return false;
}

int loadMenuGame(const int type)
{

	const int status = 2;

	string textMenu1[status];
	string textMenu2[status];

	textMenu1[1] = "Play game";
	textMenu1[0	] = "Exit";

	textMenu2[1] = "Play again";
	textMenu2[0] = "Exit";

	SDL_Rect rectTextMenu[status];

	rectTextMenu[0].x = 100;
	rectTextMenu[0].y = 300;

	rectTextMenu[1].x = 100;
	rectTextMenu[1].y = 400;

	bool selected[status] = { 0,0 };
	int xm = 0;
	int ym = 0;

	SDL_Event m_event;
	while (true)
	{
		if (load_img(screen, "img//menu.bmp", NULL, NULL) == false)
			return 0;
		if (type == 1)
		{
			loadText(textMenu1[0], rectTextMenu[0], 2);
			loadText(textMenu1[1], rectTextMenu[1], 2);
		}

		else if (type == 2)
		{
			loadText(textMenu2[0], rectTextMenu[0], 2);
			loadText(textMenu2[1], rectTextMenu[1], 2);
		}
		while (SDL_PollEvent(&m_event))
		{
			switch (m_event.type)
			{
			case SDL_QUIT:
				return 1;
			case SDL_MOUSEMOTION:
			{
				xm = m_event.motion.x;
				ym = m_event.motion.y;

				for (size_t i = 0; i < status; i++)
				{
					if (checkFocus(xm, ym, rectTextMenu[i]) == true)
					{
						if (selected[i] == false)
						{
							selected[i] = 1;
						}
						if (selected[i] == true)
						{
							selected[i] = 0;
						}
					}
				}
					
			}
				break;
			case SDL_MOUSEBUTTONDOWN:
			{
				xm = m_event.button.x;
				ym = m_event.button.y;

				for (size_t i = 0; i < status; i++)
				{
					if (checkFocus(xm, ym, rectTextMenu[i]) == true)
					{
						return i;	
					}
				}
			}
				break;
				case SDL_KEYDOWN:
					if (m_event.key.keysym.sym == SDLK_ESCAPE)
					{
						return 0;
					}
			default:
				break;
			}
		}
		SDL_RenderPresent(screen);
	}
	return 0; 
}	

void timeGame()
{

	std::string str_time = "Time: ";
	Uint32 time_val = SDL_GetTicks() / 1000;
	std::string str_val = std::to_string(time_val);
	str_time += str_val;

	SDL_Rect desRect;
	desRect.x = SCREEN_WIDTH - 500;
	desRect.y = 30;

	loadText(str_time, desRect,1);
}

int main()
{
	srand(time(NULL));

	if (InitData() == false)
		return 0;

	loadSound();
	Mix_PlayMusic(music, -1);

	int ret_menu = loadMenuGame(1);
	if (ret_menu == 0)
		return 0;

	bool playAgain = true;
	while (playAgain)
	{
		reset();
		playAgain = false;

		if (load_img(screen, "img//background.bmp", NULL, &rect) == false)
			return 0;

		bool isQuit = false;

		int x_pos = 0;
		int a_frame = 0;

		loadSound();

		Mix_PlayMusic(music, -1);

		while (!isQuit)
		{

			while (SDL_PollEvent(&events) != 0)
			{
				if (events.type == SDL_QUIT)
					isQuit = true;
			}

			SDL_Rect temp_rect = loadBackGroung(x_pos);
			if (load_img(screen, "img//background.bmp", &temp_rect, &rect) == false)
				return 0;

			handingCactus(cactus);

			if (loadPlayerAnimation(screen, a_frame, cactus) == false)
				return 0;

			if (checkChunk == true)
			{
				Mix_PlayChannel(-1, chunk, 0);
				checkChunk = false;
			}

			timeGame();


			if (checkCollision(cactus) == false)
			{
				if (loadMenuGame(2) == 0)
					return 0;
				else
					playAgain = true;

			}
			if (playAgain == true)
				break;
			SDL_RenderPresent(screen);
		}

	}
	
	return 0;
}