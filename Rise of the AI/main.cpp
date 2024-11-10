/**
* Author: Daniel Torres
* Assignment: Rise of the AI
* Date due: 2024-11-9, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"


// σσσσσ GAME STATE σσσσσ //
struct GameState
{
    Entity* player;
    Entity* enemies;
    //Entity* fireballs;

    Map* map;

    //Mix_Music* bgm;
    //Mix_Chunk* jump_sfx;
};

enum AppStatus { RUNNING, TERMINATED };
enum FilterType { NEAREST, LINEAR };

// σσσσσ CONSTANTS σσσσσ //
constexpr int WINDOW_WIDTH = 640 * 1.5,
WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char GAME_WINDOW_NAME[] = "Rise of the AI!";
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",

// σσσσσ FILEPATHS FOR ASSETS σσσσσ //
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr char MAP_FILEPATH[] = "tilesheet.png";
constexpr char FONT_FILEPATH[] = "font1.png";
constexpr char PLAYER_IDLE_FILEPATH[] = "assets/player/Idle.png";
constexpr char PLAYER_WALK_FILEPATH[] = "assets/player/Run.png";

constexpr char MAP_TILESET_FILEPATH[] = "tilesheet.png";

//BGM_FILEPATH[] = "assets/audio/dooblydoo.mp3",
//JUMP_SFX_FILEPATH[] = "assets/audio/bounce.wav";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

// σσσσσ TEXTURE ID FORWARD DECLARATIONS σσσσσ //
GLuint g_font_texture_id;
GLuint player_idle_texture_id;
GLuint player_walk_texture_id;

constexpr int FONTBANK_SIZE = 16;

unsigned int LEVEL_DATA[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0, 17,  0,  0, 25,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0, 10, 11, 11, 11, 11, 11, 12,  0,  0,  0,  0,  0,  0,  0,
     0,  0, 33,  0, 34,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 27,  0,  0,
     0,  0, 10, 11, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 11, 12,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16
};




// σσσσσ VARIABLES σσσσσ //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f,
g_accumulator = 0.0f;

bool shoot = false;
int max_attempts = ENEMY_COUNT + 1;
int counter = 0;


void initialise();
void process_input();
void update();
void render();
void shutdown();

// σσσσσ GENERAL FUNCTIONS σσσσσ //
GLuint load_texture(const char* filepath, FilterType filterType)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}


void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairsσone for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

float left_bound_offset = -(21 * 0.5) / 2.0f;
float top_bound_offset = (16 * 0.5) / 2.0f;
glm::vec3 view_offset(left_bound_offset, top_bound_offset, 0.0f);

void initialise()
{
    // σσσσσ GENERAL σσσσσ //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // σσσσσ VIDEO SETUP σσσσσ //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    //Step 1: Apply Offset to the View Matrix
    
    g_view_matrix = glm::translate(g_view_matrix, view_offset);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // σσσσσ MAP SET-UP & FONT SETUP σσσσσ //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH, NEAREST);
    g_game_state.map = new Map(21, 16, LEVEL_DATA, map_texture_id, 0.5f, 7, 6);
    g_font_texture_id = load_texture(FONT_FILEPATH, NEAREST);

    // σσσσσ PLAYER SET-UP σσσσσ //
    player_idle_texture_id = load_texture(PLAYER_IDLE_FILEPATH, NEAREST);
    player_walk_texture_id = load_texture(PLAYER_WALK_FILEPATH, NEAREST);
    //GLuint player_dead_texture_id = load_texture("assets/player/Dead.png", NEAREST);
    
    


    std::vector<GLuint> player_texture_ids = {
        player_idle_texture_id,   // IDLE spritesheet
        player_walk_texture_id,   // WALK spritesheet
        //player_dead_texture_id, //DEAD spritesheet
        //player_attack_texture_id //attack spritesheet
    };


    std::vector<std::vector<int>> player_animations = {
        {0, 1, 2, 3, 4, 5, 6, 7},       // IDLE animation frames
        {0, 1, 2, 3, 4, 5, 6, 7},       // RUN animation frames
        //{0, 1, 2, 3},                   // DEAD animation frames
        //{0, 1, 2, 3, 4, 5, 6, 7, 8}     // ATTACK animation frames
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -5.905f, 0.0f);

    g_game_state.player = new Entity(
        player_texture_ids,        // texture id
        2.5f,                      // speed
        acceleration,              // acceleration
        3.0f,                      // jumping power
        player_animations,         // animation index sets
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        1,                         // animation row amount
        0.15f,                     // width
        1.5f,                      // height
        PLAYER
    );

    g_game_state.player->set_scale(glm::vec3(1.5f, 1.5f, 1.0f));
    g_game_state.player->set_position(-view_offset + (glm::vec3(-2.5f, -0.75f, 0.0f)));

    // Jumping
    g_game_state.player->set_jumping_power(5.0f);

    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    //g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    //    Mix_PlayMusic(g_game_state.bgm, -1);
    //    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);

    //g_game_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);

    // ————— ENEMIES ————— //
    g_game_state.enemies = new Entity[ENEMY_COUNT];

    std::vector<GLuint> enemy_texture_ids = {

        load_texture("assets/enemy/Idle.png", NEAREST), // IDLE spritesheet
        load_texture("assets/enemy/Run.png", NEAREST),  // WALK spritesheet
        
         
    };


    std::vector<std::vector<int>> enemy_animations = {

        {0, 1, 2, 3, 4, 5, 6,},          // IDLE animation frames
        {0, 1, 2, 3, 4, 5, 6, 7},        // RUN animation frames
        

    };

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_game_state.enemies[i] = Entity(enemy_texture_ids, enemy_animations, 1.5f, 0.25f, 1.5f, ENEMY, GUARD, IDLING);
        g_game_state.enemies[i].set_scale(glm::vec3(1.5f, 1.5f, 1.0f));
    }

    // enemy 0 on the right platform
    g_game_state.enemies[0].set_position(-view_offset + (glm::vec3(2.5f, -0.75f, 0.0f)));
    g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[0].set_acceleration(acceleration);

    // enemy 1 by the left platform
    g_game_state.enemies[1].set_position(-view_offset + (glm::vec3(-3.95f, -0.1f, 0.0f)));
    g_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[1].set_acceleration(acceleration);
    g_game_state.enemies[1].set_ai_type(JUMPER);
    g_game_state.enemies[1].set_jumping_power(4.0f);


    // enemy 2 on the center platform
    g_game_state.enemies[2].set_position(-view_offset + (glm::vec3(0.0f, 1.5f, 0.0f)));
    g_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[2].set_acceleration(acceleration);
    g_game_state.enemies[2].set_ai_type(WALKER);
    



    // ---- FIREBALL ----//
    //GLuint player_attack_texture_id = load_texture("assets/player/Charge_1.png", NEAREST);
    ////glm::mat4 g_fireball_matrix = glm::mat4(1.0f);
    ////glm::vec3 g_fireball_position = -view_offset + (glm::vec3(-2.5f, -0.75f, 0.0f));
    ////glm::vec3 g_fireball_movement = glm::vec3(1.0f, 0.0f, 0.0f);
    //std::vector<std::vector<int>> fireball_animations = {

    //    {0, 1, 2, 3, 4, 5, 6, 7, 8},          // IDLE animation frames
    //                   

    //};
    //g_game_state.fireballs = new Entity[ENEMY_COUNT + 1];
    //for (int i = 0; i < ENEMY_COUNT + 1; i++) {
    //    g_game_state.fireballs[i] = Entity(player_attack_texture_id, 7.0f, 0.25, 0.25, FIREBALL);
    //    g_game_state.fireballs[i].set_position(g_game_state.player->get_position());
    //}




    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;

            case SDLK_SPACE:
                // Jump
                if (g_game_state.player->get_collided_bottom())
                {
                    g_game_state.player->jump();
                    //Mix_PlayChannel(-1, g_game_state.jump_sfx, 0);
                }
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A]) {
        g_game_state.player->move_left();
        g_game_state.player->face_left();
        g_game_state.player->set_texture_id(WALK);
        g_game_state.player->set_animation_state(WALK);
    }
    else if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D]) {
        g_game_state.player->move_right();
        g_game_state.player->face_right();
        g_game_state.player->set_texture_id(WALK);
        g_game_state.player->set_animation_state(WALK);

    }
    

    else {
        g_game_state.player->set_texture_id(IDLE);
        g_game_state.player->set_animation_state(IDLE);
    }

    if (key_state[SDL_SCANCODE_LSHIFT]) { shoot = true; }
    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
        g_game_state.player->normalise_movement();
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            Entity& enemy = g_game_state.enemies[i];
            enemy.update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);

            
            if (g_game_state.player->check_collision(&enemy) &&
                g_game_state.player->get_position().y > enemy.get_position().y &&
                g_game_state.player->get_velocity().y < 0)
            {
                enemy.deactivate();
                counter++;
            }
            
            else if (enemy.m_is_active && g_game_state.player->check_collision(&enemy))
            {
                g_game_state.player->lost = true;
                g_game_state.player->deactivate();
                break;  
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;
    if (g_game_state.enemies[0].m_is_active == false) {
        if (g_game_state.enemies[1].m_is_active == false) {
            if (g_game_state.enemies[2].m_is_active == false) {
                g_game_state.player->won = true;
            }
        }
    }
    
}



void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {

        g_game_state.enemies[i].render(&g_shader_program);

    }
    if (g_game_state.player->lost)
    {
        draw_text(&g_shader_program, g_font_texture_id, "YOU LOST!", 0.3f, 0.0f,
        -view_offset + glm::vec3(-1.1f, 2.5f, 0.0f)); 
    }
    else if (g_game_state.player->won) {
        draw_text(&g_shader_program, g_font_texture_id, "YOU WON!", 0.3f, 0.0f,
            -view_offset + glm::vec3(-1.1f, 2.5f, 0.0f));
    }
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete[] g_game_state.enemies;
    delete    g_game_state.player;
    delete    g_game_state.map;
    //Mix_FreeChunk(g_game_state.jump_sfx);
    //Mix_FreeMusic(g_game_state.bgm);
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}