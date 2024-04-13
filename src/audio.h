#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <vector>
#include <random>
#include <string>
class BGM {
private:
    Mix_Music* music = NULL;
public:
    BGM() {}
    ~BGM() {
        Mix_FreeMusic(music);
        music = NULL;
    }
    void load(string name) {
        string path = filesystem::current_path().string() + "\\assets\\audio\\bgm\\" + name + ".wav";
        music = Mix_LoadMUS(path.c_str());
        if (music == NULL) {
            printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
            exit(1);
        }
    }
    void play() {
        Mix_PlayMusic(music, -1);
    }
    void stop() {
        Mix_HaltMusic();
    }
};
class SE {
private:
    Mix_Chunk* se = NULL;
public:
    SE() {}
    ~SE() {
        Mix_FreeChunk(se);
        se = NULL;
    }
    void load(string name) {
        string path = filesystem::current_path().string() + "\\assets\\audio\\se\\" + name + ".wav";
        se = Mix_LoadWAV(path.c_str());
        if (se == NULL) {
            printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
            exit(1);
        }
    }
    void play() {
        Mix_PlayChannel(-1, se, 0);
    }
};
