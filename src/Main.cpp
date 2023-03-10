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

   Entity entities[3] = { Entity(0,0,characterTexture),
                          Entity(30,30,characterTexture),
                          Entity(60,60,characterTexture)};

    bool gameRunning = true;
    
    SDL_Event event;
    while (gameRunning){
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)
                gameRunning = false;
        }
        window.clear();
        for(int i = 0 ; i <3 ; i ++){
            window.render(entities[i]) ; 
        }
        window.display();
    }



    window.cleanUp();
    SDL_Quit();
    
    return 0;
}