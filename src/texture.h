#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <filesystem>

using namespace std;

extern SDL_Renderer* renderer;

class Texture {
    private:
		SDL_Texture* texture = NULL;
	public:
		Texture() {}

		~Texture() {
            free();
        }

        void setAlpha(int alpha) {
            SDL_SetTextureAlphaMod(texture, alpha);
        }

		bool load(string name) {
            free();
            const string path = filesystem::current_path().string() + "\\assets\\picture\\" + name + ".png";
            texture = IMG_LoadTexture(renderer, path.c_str());
            return texture != NULL;
        }

		void free() {
            if (texture != NULL) {
                SDL_DestroyTexture(texture);
                texture = NULL;
            }
        }

        void render(SDL_Rect* clip, SDL_Rect* dest) {
            SDL_RenderCopy(renderer, texture, clip, dest);
        }

		void render(int x, int y, SDL_Rect* clip = NULL, int width = 0, int height = 0, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {
            SDL_Rect renderQuad = { x, y, 32, 32 };
            if( clip != NULL ) {
                renderQuad.w = clip->w;
                renderQuad.h = clip->h;
            }
            if (width != 0) renderQuad.w = width;
            if (height != 0) renderQuad.h = height;

            // center
            renderQuad.x -= renderQuad.w / 2;
            renderQuad.y -= renderQuad.h / 2;

            SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip );
        }
};

