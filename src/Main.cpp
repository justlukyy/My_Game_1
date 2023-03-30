#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<vector>
#include"Utils.hpp"

#include"RenderWindow.hpp"
#include"entity.hpp"
#include"Math.hpp"



int main(int argc , char* argv[]){
    if (SDL_Init(SDL_INIT_VIDEO)> 0)
        std::cout << "SDL_Init has failed . SDL Error is: " << SDL_GetError() << std::endl;
    if(!(IMG_Init(IMG_INIT_PNG)))
        std::cout << "IMG_Init has failed . SDL Error is: " << SDL_GetError() << std::endl;
    
    RenderWindow window("Gamev1.0 ",1280,720);
    


    SDL_Texture* characterTexture = window.loadTexture("res/img/MyG_Standing.png");
    SDL_Texture* backgroundTexture = window.loadTexture("res/img/ground_grass_1.png");


        
std::vector<Entity> entitiies = {   Entity(Vector2f(0,0),backgroundTexture),
                                    Entity(Vector2f(30,30),backgroundTexture),
                                    Entity(Vector2f(60,60),backgroundTexture)};
    {
        Entity Tate(Vector2f(300,50), characterTexture) ;
        entitiies.push_back(Tate) ; 
    }
    bool gameRunning = true;
    
    SDL_Event event;
    const float timeStep = 0.01f;
    float accumulator = 0.0f ;
    float currentTime = utils::hireTimeInSeconds() ; 
    while (gameRunning){
        int starTicks =  SDL_GetTicks() ;
        float newTime = utils::hireTimeInSeconds(); 
        float frameTime = newTime - currentTime ; 
        currentTime  = newTime ; 
        accumulator += frameTime ; 
        while(accumulator >= timeStep) {
            while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)
                gameRunning = false;
        }
            accumulator -= timeStep; 
        }
        const float alpha = accumulator/ timeStep ; 
        
        window.clear();
        for(Entity& p : entitiies){
            window.render(p) ; 
        }

        window.display();
        int frameTicks = SDL_GetTicks() - starTicks ; 
        if(frameTicks < 1000/window.getRefreshRate())
            SDL_Delay(1000/window.getRefreshRate()-frameTicks);
        
    
    }



    window.cleanUp();
    SDL_Quit();
    
    return 0;
}