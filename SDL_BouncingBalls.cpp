#include <Game.h>


// Ali Nakhaee
// https://github.com/Haj4li

//Screen dimension constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 650

#define ROWS 12
#define COLS 11


short dificulty = 4;
short level =9;
unsigned int timer = 0;

short ballmap[ROWS][COLS] = {0};

short currentBal = 0;
short nextBall = 0;
bool ball_ingame = false;
bool gameover = false;
bool win = false;

bool gamepaused = false;


std::vector<int> ingamecolors;

void addcolor(int color)
{
	for (const auto& c : ingamecolors)
	{
		if (c == color)
			return;
	}
	ingamecolors.push_back(color);
}
int getRandomColor()
{
	srand(time(0));
	int r = rand() % 100;
	if (r > 90)
	{
		return 9;
	}
	if (ingamecolors.empty())
		return 0;
	return ingamecolors[rand() % ingamecolors.size()];
}

SDL_Rect ball_pos;
void randomizeBalls(short ballmap[ROWS][COLS]);

Game game(SCREEN_WIDTH, SCREEN_WIDTH, "Bouncing Balls");

struct Ball
{
	unsigned short r, g, b;
	void Color(unsigned short r = 0, unsigned short g = 0, unsigned short b = 0)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

};
Ball balls[10];

Sprite ball;


bool checkAABBCollision(const SDL_Rect& rect1, const SDL_Rect& rect2) {
	// Check for overlap on the x-axis
	if (rect1.x + rect1.w < rect2.x || rect2.x + rect2.w < rect1.x) {
		return false;
	}

	// Check for overlap on the y-axis
	if (rect1.y + rect1.h < rect2.y || rect2.y + rect2.h < rect1.y) {
		return false;
	}

	// There is overlap on both axes, so collision occurred
	return true;
}
std::vector<std::pair<int, int>> matchingDirections;

std::vector<std::pair<int, int>> checkDirections(int row, int col, int color)
{


	// Define the directions to check
	int directions[8][2] = {
		{-1, 0},    // North
		{-1, 1},    // Northeast
		{0, 1},     // East
		{1, 1},     // Southeast
		{1, 0},     // South
		{1, -1},    // Southwest
		{0, -1},    // West
		{-1, -1}    // Northwest
	};

	
	int currentColor = ballmap[row][col];


	// Check each direction
	
	for (int i = 0; i < 8; ++i) {
		int newRow = row + directions[i][0];
		int newCol = col + directions[i][1];
		bool cc = true;
		// Check if the new position is within bounds
		if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
			if (ballmap[newRow][newCol] == color) {

				for (const auto& pair : matchingDirections) {
					if (pair == std::make_pair(newRow, newCol)) {
						cc = false;
						continue;
					}
				}
				if (cc)
				{
					matchingDirections.push_back(std::make_pair(newRow, newCol));


					checkDirections(newRow, newCol, color);
				}
				
				
			}
		}
	}

	return matchingDirections;
}
bool isHanging(int row, int col)
{
	int directions[3][2] = {
		{-1, 0},    // North
		{-1, 1},    // North
		{-1, -1},    // North
	};
	int currentColor = ballmap[row][col];

	for (int i = 0; i < 3; ++i) {
		int newRow = row + directions[i][0];
		int newCol = col + directions[i][1];
		bool cc = true;
		// Check if the new position is within bounds
		if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
			if (ballmap[newRow][newCol] != -1) {
				return false;
			}
		}
	}
	return true;
}

void Draw_Balls()
{
	int xoffset = (SCREEN_WIDTH - (ball.GetSize().x * ROWS)) / 4;
	


	
	int step = 10;
	int yoffset = 2;
	yoffset = ((ball.GetSize().y) + yoffset);
	int total_balls = COLS;

	unsigned short plus = 0;
	// Step 1: Detect collisions

	bool collisionDetected = false;
	win = true;
	for (int r = 0; r < ROWS; r++) {
		for (int i = 0; i < total_balls; i++) {

			if (ballmap[r][i] >= 0 && !(ballmap[r][i] == 9 && r == 0))
			{
				win = false;
			}
			if (isHanging(r, i) && r != 0)
				ballmap[r][i] = -1;


			if (ballmap[r][i] == -1 && r != 0)
				continue;
			

			SDL_Rect cball = { (i * ball.GetSize().x) + (i * 3) + step, ((-1) * (ball.GetSize().y * ROWS) + (ball.GetSize().y * level)) + (yoffset * r), ball.GetSize().x, ball.GetSize().y };

			if (checkAABBCollision(cball, ball_pos) && ball_ingame && !collisionDetected) {
				matchingDirections.clear();
				int cr = r, cc = i;
				bool hascheck = false;
				game.PlayEffect(2);
				if (r == 0 && ballmap[r][i] == -1)
				{
					ballmap[r][i] = currentBal;
					collisionDetected = true;
					hascheck = true;
				}
				else if (ballmap[r][i] >= 0) {
					
					cr = -1;
					if (r < ROWS - 1 && ballmap[r + 1][i] == -1) {
						ballmap[r + 1][i] = currentBal;
						cr = r + 1;
					}
					else if (r < ROWS - 1 && i < COLS-1 && ballmap[r + 1][i+1] == -1) {
						ballmap[r + 1][i] = currentBal;
						cr = r + 1;
						cc = i + 1;
					}
					else if (r < ROWS - 1 && i > 0 && ballmap[r + 1][i - 1] == -1) {
						ballmap[r + 1][i] = currentBal;
						cr = r + 1;
						cc = i - 1;
					}

					if (cr == -1)
						continue;

					collisionDetected = true;
					hascheck = true;

				}
				if (hascheck && !ballmap[r][i] != 9 && currentBal != 9)
				{
					

					matchingDirections = checkDirections(cr, cc, currentBal);
					if (!matchingDirections.empty() && matchingDirections.size() >= 3) {
						for (const auto& direction : matchingDirections) {

							ballmap[direction.first][direction.second] = -1;
							ballmap[cr][i] = -1;
						}
					}
				}
			}
		}
	}
	if (win && !gameover)
	{
		game.ToggleMusic();
		game.PlayEffect(3);

		gameover = true;
		gamepaused = true;
	}

	// Step 2: Draw balls and update ballmap
	ingamecolors.clear();
	for (int r = 0; r < ROWS; r++) {
		for (int i = 0; i < total_balls; i++) {
			if (ballmap[r][i] == -1) {
				continue;
			}
			if (ballmap[r][i] != 9)
				addcolor(ballmap[r][i]);


			ball.SetColor(balls[ballmap[r][i]].r, balls[ballmap[r][i]].g, balls[ballmap[r][i]].b);
			ball.Draw((i * ball.GetSize().x) + (i * 3) + step, ((-1) * (ball.GetSize().y * ROWS) + (ball.GetSize().y * level)) + (yoffset * r));

			if (!gameover && ((-1) * (ball.GetSize().y * ROWS) + (ball.GetSize().y * level)) + (yoffset * r) >= 400)
			{
				game.ToggleMusic();
				gameover = true;
				gamepaused = true;
				game.PlayEffect(4);
				printf("Game Over");
			}
		}
	}

	// Step 3: Update ball_ingame and currentBal if collision occurred
	if (collisionDetected) {
		ball_ingame = false;
		currentBal = nextBall;
		srand(time(0));
		nextBall = getRandomColor();
		
	}

}

SDL_Point moveTowardsAngle(SDL_Point center, double angle, int distance) {
	double radianAngle = angle * M_PI / 180.0;
	int deltaX = static_cast<int>(std::cos(radianAngle) * distance);
	int deltaY = static_cast<int>(std::sin(radianAngle) * distance);
	SDL_Point newPosition = { center.x + deltaX, center.y + deltaY };
	return newPosition;
}
int calculateScore(int timeSpent, int shots) {
	double timeFactor = 1.0;
	double shotsFactor = 1.0;

	double score = 10000 / (pow(timeSpent, timeFactor) + pow(shots, shotsFactor));

	return static_cast<int>(score); 
}
int main(int argc, char* args[])
{

	balls[0].Color(255, 0, 0);
	balls[1].Color(0, 255, 0);
	balls[2].Color(0, 0, 255);
	balls[3].Color(0, 255, 255);
	balls[4].Color(255, 0, 255);
	balls[5].Color(255, 255, 0);
	balls[6].Color(100, 255, 20);
	balls[7].Color(255, 150, 50);
	balls[8].Color(150, 25, 100);

	balls[9].Color(0, 0, 0);

	unsigned int shots = 0;


	// Get the current time in milliseconds
	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);

	// Get the number of milliseconds
	auto milliseconds = currentTimeMs.time_since_epoch().count();




	//Start up SDL and create window
	if (!game.isInitialized())
	{
		printf("Failed to initialize!\n");
		return -1;
	}

	randomizeBalls(ballmap);

	ball.Load("ball.png", game.GetRenderer());


	bool running = true;
	SDL_Event e;
	int x = 0, y = 0,tx = 0,ty = 0,fx;

	int laserx = 0, lasery = 0;
	int bx = 75, by = 550;

	SDL_Texture* text = NULL;


	// buttons 
	SDL_Rect bresume = { SCREEN_WIDTH / 4 + 95, 200,100,30 };
	SDL_Rect bsettings = { SCREEN_WIDTH / 4 + 95, 250,100,30 };

	SDL_Rect bexit = { SCREEN_WIDTH / 4 + 95, 300,100,30 };

	bool show_settings = false;

	SDL_Rect bmusic = { SCREEN_WIDTH / 4 + 95, 350,100,30 };

	double angle = 0;
	int cx = SCREEN_WIDTH / 2 - ball.GetSize().x / 2;
	int cy = SCREEN_HEIGHT - 100;

	SDL_Point center = { -15, 24};


	currentBal = getRandomColor();
	nextBall = getRandomColor();

	bool unpause = true;

	bool inmenu = true;

	int seconds = 0;
	int minutes = 0;
	game.ToggleMusic();

	std::vector<std::string> scoreboard;
	std::ifstream file("data.txt");

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line)) {
			scoreboard.push_back(line);
		}

		file.close();
	}
	
	bool done = false;


	while (running)
	{
		bool clicked = false;
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				running = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				short temp;
				switch (e.key.keysym.sym)
				{
				case SDLK_r:
					//randomizeBalls(ballmap);
					//level = 9;
					//ball_ingame = false;
					//gameover = false;
					break;
				case SDLK_SPACE:
					temp = nextBall;
					nextBall = currentBal;
					currentBal = temp;
					break;
				case SDLK_ESCAPE:
					if (!gameover)
						gamepaused = !gamepaused;
					break;
				}
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				
				SDL_GetMouseState(&x, &y);
				
				
				if (!gamepaused && !inmenu && !gameover)
					angle = std::atan2(y - (cy + ball.GetSize().y / 2), x - (cx + ball.GetSize().x / 2)) * 180 / M_PI;

				SDL_Point newPosition = moveTowardsAngle(SDL_Point{ cx - center.x,cy - center.y }, angle, 5000);
				laserx = newPosition.x;
				lasery = newPosition.y;

			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
				if (!gamepaused && !inmenu && !gameover && !ball_ingame)
				{
					tx = laserx;
					ty = lasery;

					bx = cx;
					by = cy;

					ball_ingame = true;
					shots++;
					game.PlayEffect(1);
				}
				
				else
				{
					clicked = true;
				}
			}
			
		
		}

		if (ball_ingame && !gameover && !gamepaused && !inmenu)
		{
			int dx = tx - bx;
			int dy = ty - by;

			// Calculate the total distance
			int distance = std::sqrt(dx * dx + dy * dy);

			// Calculate the number of steps required to reach the target
			int steps = distance / 18;

			// Calculate the step size for x and y
			float stepX = dx / static_cast<float>(steps);
			float stepY = dy / static_cast<float>(steps);

			int nextX = bx + static_cast<int>(stepX);
			int nextY = by + static_cast<int>(stepY);

			if (nextX < 0 || nextX > SCREEN_WIDTH)
			{
				tx = -tx;
			}


			// Update the object's position
			bx += static_cast<int>(stepX);
			by += static_cast<int>(stepY);
		}
		else
		{
			bx = cx, by = cy;
		}
		



		game.Update();

		// Draw Game



		if (!gamepaused && !inmenu)
			timer++;


		if (!inmenu)
		{
			Draw_Balls();

			SDL_SetRenderDrawColor(game.GetRenderer(), 255, 0, 0, 255);
			SDL_RenderDrawLine(game.GetRenderer(), cx, cy, laserx, lasery);
			// current
			ball.SetColor(balls[currentBal].r, balls[currentBal].g, balls[currentBal].b);
			ball.Draw(bx - ball.GetSize().x/2, by - ball.GetSize().y/2);

			ball.SetColor(balls[nextBall].r, balls[nextBall].g, balls[nextBall].b);
			ball.Draw(10, 550);


			ball_pos = { bx + 8,by + 8,ball.GetSize().x - 16,ball.GetSize().y - 16 };

			


			if (timer >= 500 && !gamepaused) {
				level++;
				timer = 0;
			}

			if (!gamepaused)
			{
				// Get the current time in milliseconds
				auto passedtime = std::chrono::system_clock::now();
				auto passedms = std::chrono::time_point_cast<std::chrono::milliseconds>(passedtime);

				// Get the number of milliseconds
				auto passedmilies = passedms.time_since_epoch().count() - milliseconds;

				// Convert milliseconds to seconds and minutes
				seconds = (passedmilies / 1000);
				minutes = seconds / 60;
			}
			

			game.DrawText("Shots: " + std::to_string(shots), SCREEN_WIDTH / 2 + 150, 530, { 0,0,0,255 }, text);
			game.DrawText("Time: " + std::to_string(minutes) + ":"+std::to_string(seconds % 60), SCREEN_WIDTH / 2 + 150, 550, {0,0,0,255}, text);

		}


		// Draw pause menu

		if (inmenu || gamepaused)
		{
			if (!inmenu)
			{
				SDL_Rect pauseBG = { SCREEN_WIDTH / 4,150,SCREEN_WIDTH - SCREEN_WIDTH / 2,300 };
				SDL_SetRenderDrawColor(game.GetRenderer(), 255, 255, 153, 150);
				SDL_RenderFillRect(game.GetRenderer(), &pauseBG);

				if (gameover && win)
				{
					game.DrawText("You Won!", SCREEN_WIDTH / 4 + 90, 170, { 0,255,0,255 }, text);
				}
				else if (gameover && !win)
				{
					game.DrawText("GAME OVER!", SCREEN_WIDTH / 4 + 90, 170, { 255,0,0,255 }, text);

				}
				if (gameover)
				{
					int score = 0;
					if (win)
						score = (calculateScore(seconds, shots));
					
					game.DrawText("User: Ali", SCREEN_WIDTH / 4 + 10, 400, { 0,0,0,255 }, text);
					game.DrawText("Time: " + std::to_string(minutes) + ":" + std::to_string(seconds % 60), SCREEN_WIDTH / 4 + 180, 400, { 0,0,0,255 }, text);
					game.DrawText("Score: " + std::to_string(score), SCREEN_WIDTH / 4 + 180, 380, { 0,0,0,255 }, text);
					if (!done && win)
					{
						
						scoreboard.insert(scoreboard.begin(), "Name: Ali, Score: " + std::to_string(score));
						std::ofstream file("data.txt"); // Open the file for writing

						if (file.is_open()) {
							for (const std::string& line : scoreboard) {
								file << line << std::endl;
							}
							file.close();
						}
						done = true;
					}

				}
			}
			if (checkAABBCollision(bresume, { x,y,1,1 }))
			{
				SDL_SetRenderDrawColor(game.GetRenderer(), 0, 255, 0, 150);
				if (clicked)
				{
					if (inmenu)
					{
						currentTime = std::chrono::system_clock::now();
						currentTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);

						// Get the number of milliseconds
						milliseconds = currentTimeMs.time_since_epoch().count();

						randomizeBalls(ballmap);
						level = 9;
						ball_ingame = false;
						gameover = false;
						inmenu = false;
						currentBal = getRandomColor();
						nextBall = getRandomColor();
						shots = 0;
						done = false;
					}
					else
					{
						if (!gameover)
							unpause = true;
						else if (gameover)
						{
							currentTime = std::chrono::system_clock::now();
							currentTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);

							// Get the number of milliseconds
							milliseconds = currentTimeMs.time_since_epoch().count();

							unpause = true;
							randomizeBalls(ballmap);
							level = 9;
							ball_ingame = false;
							gameover = false;
							shots = 0;
							currentBal = getRandomColor();
							nextBall = getRandomColor();

							game.ToggleMusic();

							done = false;
						}
					}
					
				}
					
			}
			else
				SDL_SetRenderDrawColor(game.GetRenderer(), 255, 255, 0, 150);

			SDL_RenderFillRect(game.GetRenderer(), &bresume);

			if (checkAABBCollision(bexit, { x,y,1,1 }))
			{
				SDL_SetRenderDrawColor(game.GetRenderer(), 0, 255, 0, 150);
				if (clicked && inmenu)
					running = false;
				else if (clicked && gamepaused)
				{
					inmenu = true;
					unpause = true;
				}
			}
			else
				SDL_SetRenderDrawColor(game.GetRenderer(), 255, 255, 0, 150);

			SDL_RenderFillRect(game.GetRenderer(), &bexit);


			if (checkAABBCollision(bsettings, { x,y,1,1 }))
			{
				SDL_SetRenderDrawColor(game.GetRenderer(), 0, 255, 0, 150);
				if (clicked)
					show_settings = !show_settings; // TODO : settings
			}
			else
				SDL_SetRenderDrawColor(game.GetRenderer(), 255, 255, 0, 150);

			SDL_RenderFillRect(game.GetRenderer(), &bsettings);

			if (show_settings)
			{
				if (checkAABBCollision(bmusic, { x,y,1,1 }))
				{
					SDL_SetRenderDrawColor(game.GetRenderer(), 0, 255, 0, 150);
					if (clicked)
						game.ToggleMusic();
				}
				else
					SDL_SetRenderDrawColor(game.GetRenderer(), 255, 255, 0, 150);

				SDL_RenderFillRect(game.GetRenderer(), &bmusic);
				game.DrawText("MUSIC", SCREEN_WIDTH / 4 + 100, 150 + 200, { 255,0,0,255 }, text);
			}
			


			// Buttons
			if(inmenu)
				game.DrawText("PLAY", SCREEN_WIDTH / 4 + 100, 150 + 50, { 255,0,0,255 }, text);
			else
				if (!gameover)
					game.DrawText("Resume", SCREEN_WIDTH / 4 + 100, 150 + 50, { 255,0,0,255 }, text);
				else
					game.DrawText("Reset", SCREEN_WIDTH / 4 + 100, 150 + 50, { 255,0,0,255 }, text);


			game.DrawText("Settings", SCREEN_WIDTH / 4 + 100, 150 + 100, { 255,0,0,255 }, text);

			
			if (inmenu)
				game.DrawText("Exit", SCREEN_WIDTH / 4 + 100, 150 + 150, { 255,0,0,255 }, text);
			else
				game.DrawText("Menu", SCREEN_WIDTH / 4 + 100, 150 + 150, { 255,0,0,255 }, text);
			
			if (inmenu)
			{
				game.DrawText(" - Score Board - ", SCREEN_WIDTH / 4 + 65, 400, { 255,0,0,255 }, text);
				for (short i = 0; i < scoreboard.size(); i++)
				{
					game.DrawText(scoreboard[i], SCREEN_WIDTH / 4 + 50, 420 + (i * 20), { 255,0,0,255 }, text);
				}
			}
			
			

		}




		

		game.Present();

		if (unpause)
		{
			gamepaused = false;
			unpause = false;
		}

	}

	SDL_DestroyTexture(text);
	return 0;
}



void randomizeBalls(short ballmap[ROWS][COLS]) {
	srand(time(0)); // Seed the random number generator with the current time

	

	for (short i = 0; i < ROWS; i++) {
		for (short j = 0; j < COLS; j++) {
			ballmap[i][j] = rand() % dificulty + 1; // Generate a random number
			addcolor(ballmap[i][j]);
		}
	}

	// Group the randomized values together
	for (short i = 0; i < ROWS; i++) {
		for (short j = 0; j < COLS; j++) {

			if (i >= ROWS-7)
			{
				ballmap[i][j] = -1;
				continue;
			}

			short randomRow = rand() % ROWS;
			short randomCol = rand() % COLS;
			std::swap(ballmap[i][j], ballmap[randomRow][randomCol]);
		}
	}

}