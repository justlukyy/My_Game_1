#include<iostream>
#include<SDL.h>
#include<SDL_image.h>

#include"RenderWindow.hpp"
#include"entity.hpp"
int main(int argc , char* argv[]){
    if (SDL_Init(SDL_INIT_VIDEO)> 0)
        std::cout << "SDL_Init has failed . SDL Error is: " << SDL_GetError() << std::endl;
    if(!(IMG_Init(IMG_INIT_PNG)))
        std::cout << "IMG_Init has failed . SDL Error is: " << SDL_GetError() << std::endl;
    
    RenderWindow window("Gamev1.0 ",1280,720);

    SDL_Texture* characterTexture = window.loadTexture("res/img/ground_grass_1.png");

   Entity first_platform(100,100,characterTexture);

    bool gameRunning = true;
    
    SDL_Event event;
    while (gameRunning){
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)
                gameRunning = false;
        }
        window.clear();
        window.render(first_platform);
        window.display();
    }



    window.cleanUp();
    SDL_Quit();
    
    return 0;
}