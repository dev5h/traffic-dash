// TODO: Add score
#include "raylib.h"
#include <iostream>
#include <vector>
#include <cstdlib>   // For rand() and srand()
#include <ctime>     // For time()
#include <algorithm> // For basic sorting and producing minimal code
#define ROADLINE_H 25
#define FENCH_W 7 // Fench width in Pixel

// Constants
const int screenWidth = 600;
const int screenHeight = 800;
// Game states
bool GAME_OVER = false;
bool GAME_PAUSED = false;
bool GAME_STARTED = false;

// Globals
int score = 0;

// A function to pause the game
void pauseGame(Texture2D &pauseBtn)
{
    pauseBtn = LoadTexture("assets/play_btn.png");
    GAME_PAUSED = true;
}
// Function to play the game
void playGame(Texture2D &pauseBtn)
{
    pauseBtn = LoadTexture("assets/pause_btn.png");
    GAME_PAUSED = false;
}
// Function to detect mouse hover on an image texture
bool isTextureHovered(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
// Function to get the collision rectangle for a texture
Rectangle GetTextureCollisionRect(Texture2D texture, Vector2 position)
{
    Rectangle collisionRect;
    collisionRect.x = position.x;
    collisionRect.y = position.y;
    collisionRect.width = (float)texture.width;
    collisionRect.height = (float)texture.height;
    return collisionRect;
}
float findSmallestFloat(const std::vector<float> &numbers)
{
    if (numbers.empty())
    {
        return 0.0f;
    }

    // Use std::min_element to find the smallest element in the vector
    auto minElement = std::min_element(numbers.begin(), numbers.end());

    // Return the smallest element (dereference the iterator)
    return *minElement;
}
// Function to Generate random float values
float getRandomFloat(float min, float max)
{
    // Generate a random float in the range [min, max]
    float randomFloat = static_cast<float>(std::rand()) / RAND_MAX;
    return min + randomFloat * (max - min);
}
// Function to generate random float numbers with a minimum difference
std::vector<float> generateRandomFloatsWithMinDifference(float min, float max, float x, int count)
{
    /* Algorithm is authored by Shahoriar Rahman Shazin */
    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // First we create an empty vector
    std::vector<float> vec;
    // Generate The first float in range
    vec.push_back(getRandomFloat(min, max));
    // Now we're gonna generate the remaining numbers
    bool status;
    for (auto i = 0; i < count - 1; i++)
    {
        // The outer loop will generate a number
        float tmp = getRandomFloat(min, max);
        status = false;
        int n = 10;
        // This outer loop will attempt to find a suitable value . It will try n times
        for (auto j = 0; j < n; j++)
        {

            for (auto k = 0; k < vec.size(); k++)
            {

                if (std::abs(tmp - vec.at(k)) > x)
                {
                    // We're good to go
                    status = true;
                }
                else
                {

                    status = false;
                    break;
                }
            }
            // If the final status after one run of the k loop is true than we break the loop
            if (status == true)
            {
                break;
            }
            // If the final status after one run of the k loop is false than we reassign a value and rerun the j loop
            else
            {
                // reassign a random value for next approach
                tmp = getRandomFloat(min, max);
            }
        }
        // Check finally if status == true
        if (status)
        {
            vec.push_back(tmp);
        }
    }
    return vec;
}

// Function to get a random float element from an int vector
float getRandom(const std::vector<float> &vec)
{
    // Generate a random index within the vector's bounds
    int randomIndex = std::rand() % vec.size();
    // Return the random element using the random index
    return vec[randomIndex];
}
Texture2D getRandomSprite(const std::vector<Texture2D> &vec)
{

    // Generate a random index within the vector's bounds
    int randomIndex = std::rand() % vec.size();
    // Return the random element using the random index
    return vec[randomIndex];
}

class RoadLine
{
private:
    int h = ROADLINE_H;
    int w = 3;

public:
    void draw(int x, int y)
    {
        DrawRectangle(x, y, w, h, WHITE);
    }
};

int main()
{
    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initializing The window
    InitWindow(screenWidth, screenHeight, "Raylib Rectangle");
    // Hiding window border
    // SetWindowState(FLAG_WINDOW_UNDECORATED);

    const int grass_border_width = 100;

    int num_RoadLines = 20;
    int num_RoadSideObjPerFrame = 20;
    int num_Fench = 10;
    int num_EnemyCar = 50;
    int roadStartPos = grass_border_width;
    int laneWidth = 80;

    Color grassColor = {6, 167, 48, 255};
    Color roadColor = {69, 69, 77, 255};

    Texture2D leftGrassTexture = LoadTexture("assets/grass005.png");
    Texture2D rightGrassTexture = LoadTexture("assets/grass.png");
    Texture2D textureFench = LoadTexture("assets/fench.png");
    Texture2D playerCarTexture = LoadTexture("assets/Car.png");
    Texture2D boom = LoadTexture("assets/fire.png");
    Texture2D pauseBtn = LoadTexture("assets/pause_btn.png");
    Texture2D scoreBoard = LoadTexture("assets/scoreboard.png");
    float pauseBtnOffset[2] = {screenWidth - 10 - pauseBtn.width, 10};
    Rectangle rectPauseBtn = {pauseBtnOffset[0], pauseBtnOffset[1], pauseBtn.width, pauseBtn.height};
    std::vector<float> laneDeploymentCoords = {140, 220, 300, 380, 460};
    float playerCarX = laneDeploymentCoords.at(2) - playerCarTexture.width / 2;
    float playerCarY = screenHeight - playerCarTexture.height;
    // Maximum a car can be moved
    float thresholdOffsets[2][2] = {
        // Left, right
        {grass_border_width + textureFench.width, screenWidth - grass_border_width - playerCarTexture.width - textureFench.width},
        // Up, down
        {0, screenHeight - playerCarTexture.height}};

    std::vector<Texture2D> enemeyCars = {
        LoadTexture("assets/Ambulance.png"),
        LoadTexture("assets/Mini_truck.png"),
        LoadTexture("assets/Audi.png"),
        LoadTexture("assets/Police.png"),
        LoadTexture("assets/Black_viper.png"),
        LoadTexture("assets/taxi.png"),
        LoadTexture("assets/truck.png"),
        LoadTexture("assets/Mini_van.png")};
    std::vector<float> enemyCarYCoors = generateRandomFloatsWithMinDifference(-800, screenHeight / 2, 90, num_EnemyCar);
    std::vector<Texture2D> enemyCarSprites;
    for (int i = 0; i < enemyCarYCoors.size(); i++)
    {
        enemyCarSprites.push_back(getRandomSprite(enemeyCars));
    }
    std::vector<float> enemyCarX;
    for (int i = 0; i < enemyCarYCoors.size(); i++)
    {
        enemyCarX.push_back(getRandom(laneDeploymentCoords));
    }
    float leftGrassYOffsets[] = {0, screenHeight * -1};
    float rightGrassYOffsets[] = {screenHeight - grass_border_width, screenHeight * -1};
    Rectangle lefttGrassContainer = {leftGrassYOffsets[0], 0, grass_border_width, screenHeight};
    Rectangle lefttGrassContainer2 = {leftGrassYOffsets[1], 0, grass_border_width, screenHeight};
    Rectangle rightGrassContainer = {screenWidth - grass_border_width, 0, grass_border_width, screenHeight};
    std::vector<float> roadLineYCoords;

    for (int i = 0; i < num_RoadLines; i++)
    {
        roadLineYCoords.push_back(screenHeight - (i * ROADLINE_H * 2));
    }
    // Fench Y Offsets
    std::vector<float> fenchYOffsets;
    for (int i = 0; i < num_RoadLines; i++)
    {
        fenchYOffsets.push_back(screenHeight - (i * textureFench.height * 1));
    }

    // alloted coordinates for the roadside objects
    std::vector<Texture2D> roadSideAllotedObjectArray = {
        LoadTexture("assets/tree.png"),
        LoadTexture("assets/palm_tree.png"),
        LoadTexture("assets/stone_path.png"),
        LoadTexture("assets/rock_mini.png"),
        LoadTexture("assets/mini_bush.png"),
        LoadTexture("assets/fire.png"),
        LoadTexture("assets/rock.png"),
        LoadTexture("assets/hut1.png"),
    };
    // Splash Screen Texture
    Texture2D splash_screen = LoadTexture("assets/splash.png");
    Texture2D btn_play = LoadTexture("assets/btn_play.png");
    float playBtnOffset[4] = {69, 347, btn_play.width, btn_play.height};
    // Generate Y Offsets for  the road side objects at LEFT
    std::vector<float> randomYOffsetsForRoadSideObject;
    std::vector<Texture2D> roadSideGeneratedObjectList;
    // Generating Y Offsets
    for (int i = 0; i < num_RoadSideObjPerFrame; i++)
    {
        float rnd = getRandomFloat(1, 1.5);
        randomYOffsetsForRoadSideObject.push_back(screenHeight - (rnd * i * 5 * i));

        roadSideGeneratedObjectList.push_back(getRandomSprite(roadSideAllotedObjectArray));
    }
    // Doing the same for right side footpath
    // Generate Y Offsets for  the road side objects at LEFT
    std::vector<float> randomYOffsetsForRoadSideObjectRight;
    std::vector<Texture2D> roadSideGeneratedObjectListRight;
    // Generating Y Offsets
    for (int i = 0; i < num_RoadSideObjPerFrame; i++)
    {
        float rnd = getRandomFloat(1, 1.5);
        randomYOffsetsForRoadSideObjectRight.push_back(screenHeight - (rnd * i * 5 * i));

        roadSideGeneratedObjectListRight.push_back(getRandomSprite(roadSideAllotedObjectArray));
    }

    // Speed
    float roadLineYOffset = 150.0f;
    float deltaTime = 0.0f;
    float currentTime = GetTime();
    while (!WindowShouldClose())
    {
        // Main game loop
        // Calculating delta

        float newTime = GetTime();
        deltaTime = newTime - currentTime;
        currentTime = newTime;
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE); // Clear the background to white
        if (!GAME_STARTED)
        {

            // Display splash screen
            DrawTexture(splash_screen, 0, 0, WHITE);
            // draw play button
            DrawTexture(btn_play, playBtnOffset[0], playBtnOffset[1], WHITE);
            // bind click event
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isTextureHovered((Rectangle){69, 347, btn_play.width, btn_play.height}))
            {
                GAME_STARTED = true;
            }
        }
        else
        {

            // Draw the road
            DrawRectangle(grass_border_width, 0, screenWidth - (grass_border_width * 2), screenHeight, roadColor);
            // Draw the grass container
            // left grass
            DrawTextureRec(leftGrassTexture, lefttGrassContainer, (Vector2){0, leftGrassYOffsets[0]}, WHITE);
            DrawTextureRec(leftGrassTexture, lefttGrassContainer2, (Vector2){0, leftGrassYOffsets[1]}, WHITE);
            // right grass
            DrawTextureRec(leftGrassTexture, lefttGrassContainer, (Vector2){screenWidth - grass_border_width, leftGrassYOffsets[0]}, WHITE);
            DrawTextureRec(leftGrassTexture, lefttGrassContainer2, (Vector2){screenWidth - grass_border_width, leftGrassYOffsets[1]}, WHITE);
            // Move grass left
            if (!GAME_OVER && !GAME_PAUSED)
            {
                if (leftGrassYOffsets[0] < 800)
                {
                    leftGrassYOffsets[0] += roadLineYOffset * deltaTime;
                }
                else
                {
                    leftGrassYOffsets[0] = leftGrassYOffsets[1] - screenHeight + 2;
                }
                if (leftGrassYOffsets[1] < 800)
                {
                    leftGrassYOffsets[1] += roadLineYOffset * deltaTime;
                }
                else
                {
                    leftGrassYOffsets[1] = leftGrassYOffsets[0] - screenHeight + 2;
                }
            }

            // Drawing Roadside Objects on Grass
            // Left
            for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
            {
                float xcor = (grass_border_width / 2) - (roadSideGeneratedObjectList.at(i).width / 2);
                DrawTexture(roadSideGeneratedObjectList.at(i), xcor, randomYOffsetsForRoadSideObject.at(i), WHITE);
            }

            // Now move those roadside objects
            if (!GAME_OVER && !GAME_PAUSED)
            {
                for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
                {
                    randomYOffsetsForRoadSideObject[i] += (roadLineYOffset * deltaTime);
                }
            }
            // Shift
            for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
            {
                /*
                 * ALgorithm:
                 * First we check if any object goes below the screen
                 * If yes than we shift the y Offset in the following way:
                 * We find the smallest coordinate in the yOffset vector
                 * Than we bind a yoffset/coordinate such that the coordinate has a random difference
                 * i.e diff > x(random) but x will have the same lowest value from the generation loop
                 * Algorithm proposed by Shazin
                 */
                // Here we go
                // Check  if the y Offset surpasses the screenHeight
                if (randomYOffsetsForRoadSideObject.at(i) > screenHeight)
                {
                    // get the smallest yOffset literal from the vector
                    float leastOffset = findSmallestFloat(randomYOffsetsForRoadSideObject);
                    // Generate a random diff in the same way as the generation loop
                    float diff = getRandomFloat(1, 1.5) * 5 * i + getRandomFloat(50, 100); // getRandomFloat(20, 40) is for special cases where i = 0;
                    // Finally shift the offset
                    randomYOffsetsForRoadSideObject[i] = leastOffset - diff;
                    // Additionally we are gonna randomize the corrsponding sprite as well
                    roadSideGeneratedObjectList[i] = getRandomSprite(roadSideAllotedObjectArray);
                }
            }

            // Right side. Same code as for left side
            for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
            {
                float xcor = screenWidth - grass_border_width / 2 - roadSideGeneratedObjectListRight.at(i).width / 2;
                DrawTexture(roadSideGeneratedObjectListRight.at(i), xcor, randomYOffsetsForRoadSideObjectRight.at(i), WHITE);
            }

            // Move right side objects
            if (!GAME_OVER && !GAME_PAUSED)
            {
                for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
                {
                    randomYOffsetsForRoadSideObjectRight[i] += (roadLineYOffset * deltaTime);
                }
            }
            // Shift right, same algo as left side. check left side ^
            for (auto i = 0; i < num_RoadSideObjPerFrame; i++)
            {
                if (randomYOffsetsForRoadSideObjectRight.at(i) > screenHeight)
                {
                    float leastOffset = findSmallestFloat(randomYOffsetsForRoadSideObjectRight);
                    float diff = getRandomFloat(1, 1.5) * 5 * i + getRandomFloat(50, 100);
                    randomYOffsetsForRoadSideObjectRight[i] = leastOffset - diff;
                    roadSideGeneratedObjectListRight[i] = getRandomSprite(roadSideAllotedObjectArray);
                }
            }

            // Move Road lines up
            if (!GAME_OVER && !GAME_PAUSED)
            {
                for (int i = 0; i < num_RoadLines; i++)
                {
                    roadLineYCoords[i] += (roadLineYOffset * deltaTime);
                }
            }
            // Shifting the valus
            for (int i = 0; i < num_RoadLines; i++)
            {
                if (roadLineYCoords.at(i) > screenHeight + 50)
                {
                    roadLineYCoords[i] = -150;
                }
            }
            // Draw Road Lines
            for (int _i = 0; _i < 4; _i++)
            {
                for (int i = 0; i < num_RoadLines; i++)
                {
                    DrawRectangle(roadStartPos + laneWidth + (laneWidth * _i), roadLineYCoords.at(i), 4, ROADLINE_H, WHITE);
                }
            }
            // Draw the fench Left
            for (int i = 0; i < num_Fench; i++)
            {
                // We are gonnna follow the same algorithm as the road tile below
                DrawTexture(textureFench, grass_border_width, fenchYOffsets.at(i), WHITE);
            }
            // LEFT: Move those fenches we are gonna use the same algortithm as road lines
            if (!GAME_OVER && !GAME_PAUSED)
            {
                for (int i = 0; i < num_Fench; i++)
                {
                    fenchYOffsets[i] += (roadLineYOffset * deltaTime);
                }
            }
            // Shift LEFT
            for (int i = 0; i < num_Fench; i++)
            {
                // Check if it goes below the screen
                if (fenchYOffsets.at(i) > screenHeight)
                {
                    // Find the lowest coordinate
                    float x = (screenHeight - num_Fench * textureFench.height);
                    // Now assign that value
                    fenchYOffsets[i] = x;
                }
            }
            // Draw the fench Left
            for (int i = 0; i < num_Fench; i++)
            {
                // We are gonnna follow the same algorithm as the road tile below
                DrawTexture(textureFench, screenWidth - grass_border_width - textureFench.width, fenchYOffsets.at(i), WHITE);
            }
            /*
             * NOTE: Here we don't need to move or shift right fenches
             * since we are reusing the Y Coordinates
             */

            // Drawing The Player Car
            DrawTexture(playerCarTexture, playerCarX, playerCarY, WHITE);
            // Player car movement
            if (!GAME_OVER && !GAME_PAUSED)
            {
                // Check if the left arrow key has been pressed
                // Left movement
                if (IsKeyDown(KEY_LEFT) && playerCarX > thresholdOffsets[0][0])
                {
                    playerCarX -= roadLineYOffset * deltaTime;
                }
                // Right
                if (IsKeyDown(KEY_RIGHT) && playerCarX < thresholdOffsets[0][1])
                {
                    playerCarX += roadLineYOffset * deltaTime;
                }
                // Up
                if (IsKeyDown(KEY_UP) && playerCarY > thresholdOffsets[1][0])
                {
                    playerCarY -= roadLineYOffset * deltaTime;
                }
                // Down
                if (IsKeyDown(KEY_DOWN) && playerCarY < thresholdOffsets[1][1])
                {
                    playerCarY += roadLineYOffset * deltaTime;
                }
            }

            // Enemy Deployment/Drawing
            /*
             * Step Algorithm:
             * 1) Select a Random lane for Deployment
             * 2) Select a random car sprite
             * 3) The main challenge is to randomly selecting y coordinate for the car
             */
            for (int i = 0; i < enemyCarYCoors.size(); i++)
            {
                DrawTexture(enemyCarSprites.at(i), enemyCarX.at(i) - enemyCarSprites.at(i).width / 2, enemyCarYCoors.at(i), WHITE);
            }
            // Move those cars towars +y
            if (!GAME_OVER && !GAME_PAUSED)
            {
                for (int i = 0; i < enemyCarYCoors.size(); i++)
                {
                    enemyCarYCoors[i] += roadLineYOffset * deltaTime;
                }
            }
            // Shift those cars when they goes outside the screen;
            // Here we are gonna use the same shifting algorithm as roadside objects
            for (int i = 0; i < enemyCarYCoors.size(); i++)
            {
                // Check if they outside view
                if (enemyCarYCoors.at(i) > screenHeight)
                {
                    // Increase the score first
                    score++;
                    // Get the smallest enemy car y coordinate
                    float leastY = findSmallestFloat(enemyCarYCoors);
                    // Generate a random diff in the same way as the generation loop
                    float diff = getRandomFloat(90, 120);
                    // Finally shift the offset
                    enemyCarYCoors[i] = leastY - diff;
                    // Additionally we are gonna randomize the corrsponding sprite as well
                    enemyCarSprites[i] = getRandomSprite(enemeyCars);
                }
            }
            // Check collision
            for (int i = 0; i < enemyCarYCoors.size(); i++)
            {
                Rectangle enemyCarPixelPerfectTexture = GetTextureCollisionRect(enemyCarSprites.at(i), (Vector2){enemyCarX.at(i), enemyCarYCoors.at(i)});
                Rectangle playerCarPixelPerfectTexture = GetTextureCollisionRect(playerCarTexture, (Vector2){playerCarX, playerCarY});
                if (CheckCollisionRecs(enemyCarPixelPerfectTexture, playerCarPixelPerfectTexture))
                {
                    float collisionX = std::max(enemyCarPixelPerfectTexture.x, playerCarPixelPerfectTexture.x);
                    float collisionY = std::max(enemyCarPixelPerfectTexture.y, playerCarPixelPerfectTexture.y);
                    DrawTexture(boom, collisionX, collisionY - boom.height, WHITE);
                    GAME_OVER = true;
                }
            }
            // Draw the pause button
            DrawTexture(pauseBtn, pauseBtnOffset[0], pauseBtnOffset[1], WHITE);
            // Binding click event on that button
            if (GAME_PAUSED)
            {
                // Draw a text to display paused
                int fontSize = 40;
                int textWidth = MeasureText("PAUSED", fontSize); // Measure the text width
                int textHeight = fontSize;                       // Assuming single-line text
                // Calculate the position to center the text both horizontally and vertically
                int x = (screenWidth - textWidth) / 2;
                int y = (screenHeight - textHeight) / 2;
                Color textColor = WHITE;
                DrawText("PAUSED", x, y, fontSize, textColor);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isTextureHovered(rectPauseBtn))
            {
                if (!GAME_PAUSED)
                {
                    pauseGame(pauseBtn);
                    // Draw a text to display paused
                    const char *message = "Paused";
                    int fontSize = 40;
                    int textWidth = MeasureText(message, fontSize); // Measure the text width
                    int textHeight = fontSize;                      // Assuming single-line text
                    // Calculate the position to center the text both horizontally and vertically
                    int x = (screenWidth - textWidth) / 2;
                    int y = (screenHeight - textHeight) / 2;
                    Color textColor = RED;
                    DrawText("Hello", 10, 200, fontSize, textColor);
                }
                else
                {
                    playGame(pauseBtn);
                }
            }
            // Draw scoreboard
            DrawTexture(scoreBoard, grass_border_width / 2 - scoreBoard.width / 2, 5, WHITE);
            // Draw Score Text
            std::string cppstring = "Score: " + std::to_string(score);
            char *scoreDsp = &cppstring[0];
            DrawText(scoreDsp, grass_border_width / 2 - MeasureText(scoreDsp, 12) / 2, 5 + scoreBoard.height / 2 - 12 / 2, 14, (Color){58, 21, 0, 255});
            if (GAME_OVER)
            {
                // Add an overlay
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 190});
                // Draw a text to display paused
                int fontSize = 50;
                int textWidth = MeasureText("GAME OVER", fontSize); // Measure the text width
                int textHeight = fontSize;                          // Assuming single-line text
                // Calculate the position to center the text both horizontally and vertically
                int x = (screenWidth - textWidth) / 2;
                int y = (screenHeight - textHeight) / 2;
                Color textColor = WHITE;
                DrawText("GAME OVER", x, y, fontSize, textColor);
                std::string scoreStd = "Final Score: " + std::to_string(score);
                char *scoreC = &scoreStd[0];
                DrawText(scoreC, (screenWidth - MeasureText(scoreC, 25)) / 2, y + 50, 25, WHITE);
                DrawText("Press \'R\': Restart - \'X\': Exit Game", (screenWidth - MeasureText("Press R: Restart - X: Exit Game", 25)) / 2, y + 50 * 2, 25, WHITE);
                // Restart Game
                if (IsKeyPressed(KEY_R))
                {

                    // enemyCarYCoors = generateRandomFloatsWithMinDifference(-800, screenHeight / 2, 90, num_EnemyCar);
                    for (auto i = 0; i < enemyCarYCoors.size(); i++)
                    {
                        enemyCarYCoors[i] -= 500.00f;
                    }
                    playerCarX = laneDeploymentCoords.at(2) - playerCarTexture.width / 2;
                    GAME_OVER = false;
                    score = 0;
                    GAME_PAUSED = false;
                }
                // Close game
                if (IsKeyPressed(KEY_X))
                {
                    CloseWindow();
                }
            }
        }

        EndDrawing();
    }

    // Unload the texture when done
    UnloadTexture(leftGrassTexture);
    UnloadTexture(rightGrassTexture);
    UnloadTexture(textureFench);
    UnloadTexture(playerCarTexture);
    UnloadTexture(boom);
    UnloadTexture(pauseBtn);
    // De-Initialization
    CloseWindow();

    return 0;
}
