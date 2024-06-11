#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <map>

#include "view.h"
#include "logic.h"
#include "towerGUI.h"
#include "barracks.h"
#include "constants.h"
#include "weather.h"

using namespace std;

View::View() {
    using namespace window;
    SCREEN_WIDTH = window::screenWidth;
    SCREEN_HEIGHT = window::screenHeight;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        cout << "Error. SDL could not initialize" << endl;
    };

    window = SDL_CreateWindow("Radiation Rampage",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Delay(100);
    if (window == NULL){
        cout << "Error. Could not create window" << endl;
    };

    renderer =  SDL_CreateRenderer(window,
                       -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL){
        cout << "Error creating renderer" << endl;
    };

    if (TTF_Init() < 0) {
    std::cerr << "Error. Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    }

    logic = std::make_shared<Logic>();
    tower_gui = std::make_shared<TOWERGUI>(renderer);
    update_tower_gui = std::make_shared<TOWERGUI>(renderer);
    hud = std::make_shared<HUD>(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    start = std::make_shared<startScreen>(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    laserAttackAnimation.active = false;
    bombAttackAnimation.active = false;

    loadTowerTextures();
    loadEnemyTextures();
}

void View::loadTowerTextures() {
    barracksTexture = IMG_LoadTexture(renderer, "../resource/barrackstower.png");
    bombTexture = IMG_LoadTexture(renderer, "../resource/bombtower.png");
    laserTexture = IMG_LoadTexture(renderer, "../resource/lasertower.png");
    barracksSoldierTexture = IMG_LoadTexture(renderer, "../resource/BarracksSoldiers.png");

    barracksUpgradeTexture = IMG_LoadTexture(renderer, "../resource/BarracksUpgradeTower.png");
    bombUpgradeTexture = IMG_LoadTexture(renderer, "../resource/BombUpgradeTower.png");
    laserUpgradeTexture = IMG_LoadTexture(renderer, "../resource/LaserUpgradeTower.png");
    bombEffectTexture = IMG_LoadTexture(renderer, "../resource/bombEffect.png");
}

void View::loadEnemyTextures() {
    humanRaiderTexture = IMG_LoadTexture(renderer, "../resource/HumanRaider.png");
    hawkNorthTexture = IMG_LoadTexture(renderer, "../resource/HawkNorth.png");
    mutantHawkNorthTexture = IMG_LoadTexture(renderer, "../resource/MutantHawkNorth.png");
    hawkSouthTexture = IMG_LoadTexture(renderer, "../resource/HawkSouth.png");
    mutantHawkSouthTexture = IMG_LoadTexture(renderer, "../resource/MutantHawkSouth.png");
    hawkEastTexture = IMG_LoadTexture(renderer, "../resource/HawkEast.png");
    mutantHawkEastTexture = IMG_LoadTexture(renderer, "../resource/MutantHawkEast.png");    
    dogTexture = IMG_LoadTexture(renderer, "../resource/Dog.png");
    mutantDogTexture = IMG_LoadTexture(renderer, "../resource/MutantDog.png");
    humanRaiderLeaderTexture = IMG_LoadTexture(renderer, "../resource/HumanRaiderLeader.png");
    mutantRaiderTexture = IMG_LoadTexture(renderer, "../resource/MutantRaider.png");
    mutantLeaderTexture = IMG_LoadTexture(renderer, "../resource/MutantRaiderLeader.png");
}

// Render all enemies at once
void View::renderEnemies(const std::vector<Enemy>& enemies) {
    std::vector<SDL_Rect> enemyRects;
    std::vector<SDL_Texture*> enemyTextures;

    for (const auto& enemy : enemies) {
        SDL_Rect enemyRect;
        enemyRect.w = 70;
        enemyRect.h = 70;
        enemyRect.x = enemy.getX() - enemyRect.w / 2;
        enemyRect.y = enemy.getY() - enemyRect.h / 2;
        enemyRects.push_back(enemyRect);

        // TODO: Add other enemy textures
        std::string enemyName = enemy.getName();
        if (enemyName == "HumanRaider"){
            enemyTextures.push_back(humanRaiderTexture);
        }else if (enemyName == "Hawk"){
            if (enemy.getDirection() == Direction::SOUTH) {
                enemyTextures.push_back(hawkSouthTexture);
            } else if (enemy.getDirection() == Direction::NORTH) {
                enemyTextures.push_back(hawkNorthTexture);
            } else {
                enemyTextures.push_back(hawkEastTexture);
            }
        }else if (enemyName == "MutantHawk"){
            if (enemy.getDirection() == Direction::SOUTH) {
                enemyTextures.push_back(mutantHawkSouthTexture);
            } else if (enemy.getDirection() == Direction::NORTH) {
                enemyTextures.push_back(mutantHawkNorthTexture);
            } else {
                enemyTextures.push_back(mutantHawkEastTexture);
            }
        }else if (enemyName == "RaiderLeader"){
            enemyTextures.push_back(humanRaiderLeaderTexture); 
        }else if (enemyName == "Dog"){
            enemyTextures.push_back(dogTexture);  
        }else if (enemyName == "MutantDog"){
            enemyTextures.push_back(mutantDogTexture);  
        }else if (enemyName == "MutantRaider"){
            enemyTextures.push_back(mutantRaiderTexture);  
        }else if (enemyName == "MutantLeader"){
            enemyTextures.push_back(mutantLeaderTexture);  
        }
    }

    for (size_t i = 0; i < enemies.size(); ++i) {
        SDL_RenderCopy(renderer, enemyTextures[i], nullptr, &enemyRects[i]);
    }
}

bool View::update(Logic& logic) {
    // Running is returned to update and is updated when player hits x or quits
    SDL_Event event; 
    bool running = true;

    while (SDL_PollEvent(&event)!=0){
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (logic.onStart()){
        	handleStartScreen(event);
        	if(start->getSelected() == 0){
        		logic.switchStart(false);
            }else if(start->getSelected() == 1){
        		start->setLeaderboard(true);
        		start->setSelected(-1);
        	}else if(start->getSelected() == 2){
        		start->setInstruct(true);
        		start->setSelected(-1);
        	}else if(start->getSelected() == 3){
                running = false;
        	}
        }
        else if (event.type == SDL_KEYDOWN) {
    		if(logic.getHealth() <= 0){
    			logic.reset();
    			start->setSelected(-1);
            }else if (event.key.keysym.sym == SDLK_q) {
                running = false;
            }else if (event.key.keysym.sym == SDLK_p) {
            	if(logic.isPaused()){
                	logic.setUnpaused();
            	}else{
            		logic.setPaused();
            	}
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && logic.isPaused() == false) {
            handleTowerClick(event);
            handleTowerTypeSelection(event, logic);
        }
        if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
            logic.setPaused();
        } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
            logic.setUnpaused();
        }
    }
    
    SDL_RenderClear(renderer);
    SDL_Texture* texture = IMG_LoadTexture(renderer, "../resource/Map.png");
    
    // Render the start screen
    if (logic.onStart()) {
        start->render();
    } else {
	    SDL_Rect destination;
	    destination.x = 0;
	    destination.y = 0;
	    destination.w = SCREEN_WIDTH;
	    destination.h = SCREEN_HEIGHT;
	    SDL_RenderCopy(renderer, texture, NULL, &destination);
	    renderGUI();
	    renderTowerLocations();
	    renderSoldiers();
	    renderHUD(logic);
	    renderWaveTime(*logic.getManager());
        renderWeatherName(*logic.getWeather());
        renderEnemies(logic.getEnemiesOnField());
        if (failedTransMessage.active){
            // Render error message box when money transaction fails
            renderFailedTransMessage();

            if (SDL_GetTicks() - failedTransMessage.startTime >= 3000){
                failedTransMessage.active = false;
            }   
        }

        // Render attack animations
        if (laserAttackAnimation.active){
            renderAttackAnimation(DamageType::LASER);
        }
        if (bombAttackAnimation.active){
            renderAttackAnimation(DamageType::BOMB);
        }
    }
    
    // Render lost or pause screen
    if(logic.getHealth() <= 0){
    	renderLost(logic);
    }else if(logic.isPaused() && !logic.onStart()){
    	renderPause();
    }

    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
    return running;
}

// Show GUI for tower if mouse click occurs within tower region
void View::handleTowerClick(const SDL_Event& event) {
    for (auto& location : TowerLocationManager::getTowerLocations()) {
        if (!location.occupied && event.button.x >= location.x && event.button.x <= location.x + location.size &&
            event.button.y >= location.y && event.button.y <= location.y + location.size) {
            tower_gui->show(location);
            update_tower_gui->hide();
            return;
        } else if (location.occupied && event.button.x >= location.x && event.button.x <= location.x + location.size &&
                 event.button.y >= location.y && event.button.y <= location.y + location.size) {
            update_tower_gui->show(location);
            tower_gui->hide();
            return;
        }
    }
}

// Pass mouse coordinates to GUI for option selection
void View::handleTowerTypeSelection(const SDL_Event& event, Logic& logic) {
    bool errorFreeTowerAction = true;
    if (tower_gui->isVisible()) {
        errorFreeTowerAction = tower_gui->selectTowerType(event.button.x, event.button.y, this, logic);
    } else if (update_tower_gui->isVisible()) {
        errorFreeTowerAction = update_tower_gui->selectTowerType(event.button.x, event.button.y, this, logic);
    }

    if (errorFreeTowerAction == false && !logic.onStart()){
        failedTransMessage.active = true;
        failedTransMessage.startTime = SDL_GetTicks();
    }
}

void View::handleStartScreen(const SDL_Event& event) {
	if (event.type == SDL_KEYDOWN && start->getInstruct() == false && start->getLeaderboard() == false) {
		switch (event.key.keysym.sym) {
        	case SDLK_LEFT:
            	start->moveSelection(-1);
                break;
            case SDLK_RIGHT:
                start->moveSelection(1);
                break;
            case SDLK_RETURN:
                if (start->getSelected() == -1) {
                    start->setSelected(0);
                }
                for (int i = 0; i < 4; i++) {
			        if (start->getBoxes()[i].selected) {
						start->setSelected(i);
			            break;
			        }
			    }
                break;
            }
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		// Handle clicking to select a box
	    int mouseX, mouseY;
	    SDL_GetMouseState(&mouseX, &mouseY);
	    SDL_Point mousePosition = {mouseX, mouseY};
		if(start->getInstruct() == false && start->getLeaderboard() == false){
	        for (int i = 0; i < 4; i++){
	            if (SDL_PointInRect(&mousePosition, &start->getBoxes()[i].rect)) {
	            	start->selectBox(i);
	                start->setSelected(i);
	                break;
	            }
	        }
        }else if ((start->getInstruct() || start->getLeaderboard()) && SDL_PointInRect(&mousePosition, start->getClose())) {
        	// Check if the click is within the close button's rectangle
	        start->setInstruct(false);
            start->setLeaderboard(false);
        }
	}
}

// Render respective tower images
void View::renderTowerLocations() {
    using namespace tower;
    for (const auto& location : TowerLocationManager::getTowerLocations()) {
        if (location.occupied) {
            SDL_Texture* towerTexture = nullptr;

            // Render background health bar
            SDL_SetRenderDrawColor(renderer, 211,211,211, 1);
            SDL_Rect towerHealthBackground = {location.x, location.y + location.size, location.size, 5};
            SDL_RenderFillRect(renderer, &towerHealthBackground);
            float curHealthPercent;

            if (location.towerType.compare("Barracks") == 0) {
                if (location.tower->isUpgraded()) {
                    towerTexture = barracksUpgradeTexture;
                } else {
                    towerTexture = barracksTexture;
                }
                curHealthPercent = (location.tower->getHealth()*1.0) / (tower::barracksHealth * 1.0);
            } else if (location.towerType.compare("Bomb") == 0) {
                if (location.tower->isUpgraded()) {
                    towerTexture = bombUpgradeTexture;
                } else {
                    towerTexture = bombTexture;
                }
                curHealthPercent = (location.tower->getHealth()*1.0) / (tower::bombHealth * 1.0);

            } else if (location.towerType.compare("Laser") == 0) {
                if (location.tower->isUpgraded()) {
                    towerTexture = laserUpgradeTexture;
                } else {
                    towerTexture = laserTexture;
                }
                curHealthPercent = (location.tower->getHealth()*1.0) / (tower::laserHealth * 1.0);
            }
            if (towerTexture != nullptr) {
                SDL_Rect towerRect = { location.x, location.y, location.size, location.size };
                SDL_RenderCopy(renderer, towerTexture, nullptr, &towerRect);

                //Render tower range radius
                if (update_tower_gui->isVisible()) {
                    renderTowerRadius(update_tower_gui->getLocation());
                }
            }
            
            // Render health progress 
            if (location.tower->isDestroyed() == false){
                SDL_SetRenderDrawColor(renderer, 63, 195, 128, 1);
                SDL_Rect towerHealthProgress = {location.x, location.y + location.size, (int)(location.size * curHealthPercent), 5};
                SDL_RenderFillRect(renderer, &towerHealthProgress);
            }
        }
    }
}

// Render a circle around a tower, indicating its attack range
void View::renderTowerRadius(const TowerLocation& location) {
    if (location.occupied) {
        int circleX = location.x + location.size / 2;
        int circleY = location.y + location.size / 2;
        int radius = location.tower->getRange();

        ellipseRGBA(renderer, circleX, circleY, radius, radius, 255, 0, 0, 255);
    }
}

void View::renderGUI() {
    tower_gui->render();
    update_tower_gui->render();
}


void View::renderHUD(const Logic& logic) {
    hud->update(logic.getMoney(), logic.getHealth(), (*logic.getManager()).getCurrWave());
	hud->render();
}

void View::triggerAttackAnimation(int startX, int startY, int endX, int endY, DamageType attackType){
    if (attackType == DamageType::LASER){
        laserAttackAnimation.active = true;
        laserAttackAnimation.startX = startX;
        laserAttackAnimation.startY = startY;
        laserAttackAnimation.endX = endX;
        laserAttackAnimation.endY = endY;
        laserAttackAnimation.startTime = SDL_GetTicks();
    }else if(attackType == DamageType::BOMB){
        bombAttackAnimation.active = true;
        bombAttackAnimation.startX = startX;
        bombAttackAnimation.startY = startY;
        bombAttackAnimation.endX = endX;
        bombAttackAnimation.endY = endY;
        bombAttackAnimation.startTime = SDL_GetTicks();
    }
}

void View::renderAttackAnimation(DamageType attackType) {
    using namespace tower;
    if (attackType == DamageType::LASER){
        thickLineRGBA(renderer, laserAttackAnimation.startX, laserAttackAnimation.startY,
                      laserAttackAnimation.endX, laserAttackAnimation.endY,
                      4, 255, 255, 0, 255); // Render a yellow line

        // Disable animation again after a short moment
        if (SDL_GetTicks() - laserAttackAnimation.startTime >= 100) {
            laserAttackAnimation.active = false;
        }
    }
    else if (attackType == DamageType::BOMB){
        ellipseRGBA(renderer, bombAttackAnimation.endX, bombAttackAnimation.endY, tower::bombRangeBombEffect, tower::bombRangeBombEffect, 255, 0, 0, 255);

        int bombEffectRectSize = 70;
        SDL_Rect bombEffectRect = { bombAttackAnimation.endX - (bombEffectRectSize/2) , bombAttackAnimation.endY - (bombEffectRectSize/2), bombEffectRectSize, bombEffectRectSize };
        SDL_RenderCopy(renderer, bombEffectTexture, nullptr, &bombEffectRect);
       
        // Disable animation again after a short moment
        if (SDL_GetTicks() - bombAttackAnimation.startTime >= 700) {
            bombAttackAnimation.active = false;
        }
    }
}

void View::renderSoldiers() {
    for (const auto& location : TowerLocationManager::getTowerLocations()) {
        if (location.occupied && location.towerType == "Barracks") {
            std::shared_ptr<Barracks> barracksTower = std::dynamic_pointer_cast<Barracks>(location.tower);
            if (barracksTower) {
                // Get the soldier associated with this barracks tower
                std::pair<int, int> soldierLocation = barracksTower->getTowerSoldierMapping();
                Soldier& soldier = barracksTower->getSoldierAtLocation(soldierLocation);
                
                // Render soldier if it is alive and the tower is not destroyed
                if (soldier.isAlive() && !barracksTower->isDestroyed()) {
                    SDL_Rect soldierRect = { soldierLocation.first, soldierLocation.second, 120, 50 };
                    SDL_RenderCopy(renderer, barracksSoldierTexture, nullptr, &soldierRect);
                }
            }
        }
    }
}

void View::renderLost(Logic& logic) {
	logic.setPaused();
	
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    // Calculate the dimensions and position of the rectangle
    int rectWidth = 500;
    int rectHeight = 400;
    int rectX = (SCREEN_WIDTH - rectWidth) / 2; // Center horizontally
    int rectY = (SCREEN_HEIGHT - rectHeight) / 2; // Center vertically

    // Render the filled rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black color
    SDL_Rect rect = { rectX, rectY, rectWidth, rectHeight };
    SDL_RenderFillRect(renderer, &rect);

	// Create a font
	TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 75);
	
	// Create a surface containing the rendered text
	std::string text = "You lost!";
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	
	// Get the dimensions of the rendered text
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	
	// Set the position
	int x = (SCREEN_WIDTH - textWidth) / 2; // Center horizontally
	int y = (SCREEN_HEIGHT - textHeight) / 2; // Center vertically
	
	// Render the text texture
	SDL_Rect renderQuad = {x, y, textWidth, textHeight};
	SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
	
    TTF_SetFontSize(font, 23);
	
	text = "Press any button to return to the start screen";
	textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	
	textWidth = textSurface->w;
	textHeight = textSurface->h;
	
	x = (SCREEN_WIDTH - textWidth) / 2; // Center horizontally
	y = (SCREEN_HEIGHT - textHeight) / 2 + 100;
	
	renderQuad = {x, y, textWidth, textHeight};
	SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
	
	// Cleanup resources
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
	TTF_CloseFont(font);
}

void View::renderPause() {	
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    // Calculate the dimensions and position of the rectangle
    int rectWidth = 500;
    int rectHeight = 400;
    int rectX = (SCREEN_WIDTH - rectWidth) / 2; // Center horizontally
    int rectY = (SCREEN_HEIGHT - rectHeight) / 2; // Center vertically

    // Render the filled rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black color
    SDL_Rect rect = { rectX, rectY, rectWidth, rectHeight };
    SDL_RenderFillRect(renderer, &rect);

	// Create a font
	TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 75);
	
	// Create a surface containing the rendered text
	std::string text = "Game paused";
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	
	// Get the dimensions of the rendered text
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	
	// Set the position
	int x = (SCREEN_WIDTH - textWidth) / 2; // Center horizontally
	int y = (SCREEN_HEIGHT - textHeight) / 2; // Center vertically
	
	// Render the text texture
	SDL_Rect renderQuad = {x, y, textWidth, textHeight};
	SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
	
	// Cleanup resources
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
	TTF_CloseFont(font);
}

void View::renderWaveTime(const WaveManager& manager) {
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    int rectWidth = 250;
    int rectHeight = 50;
    int rectX = SCREEN_WIDTH - rectWidth - 20;
    int rectY = 20; // 20 pixels from the top

    // Render the filled rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black color
    SDL_Rect rect = { rectX, rectY, rectWidth, rectHeight };
    SDL_RenderFillRect(renderer, &rect);

    // Create a font
    TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 18);

    // Get the time until the next wave from the WaveManager
    int timeUntilNextWave = int(trunc(manager.getWaveTime()));
    std::string text = "Next Wave in " + std::to_string(timeUntilNextWave) + " seconds";
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);

    // Create a texture from the text surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get the dimensions of the rendered text
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Set the position of the text
    int x = rectX + (rectWidth - textWidth) / 2; // Center horizontally within the rectangle
    int y = rectY + (rectHeight - textHeight) / 2; // Center vertically within the rectangle

    // Render the text texture
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    // Cleanup resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void View::renderWeatherName(const Weather& weather) {
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color

    int rectWidth = 250;
    int rectHeight = 50;
    int rectX = SCREEN_WIDTH - rectWidth - 20;
    int rectY = SCREEN_HEIGHT - rectHeight - 20; // 20 pixels from the bottom

    // Render the filled rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black color
    SDL_Rect rect = { rectX, rectY, rectWidth, rectHeight };
    SDL_RenderFillRect(renderer, &rect);

    // Create a font
    TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 18);

    // Get the time until the next wave from the WaveManager
    std::string name = weather.getWeatherName();
    std::string text = "Weather: " + name;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);

    // Create a texture from the text surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get the dimensions of the rendered text
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Set the position of the text
    int x = rectX + (rectWidth - textWidth) / 2; // Center horizontally within the rectangle
    int y = rectY + (rectHeight - textHeight) / 2; // Center vertically within the rectangle

    // Render the text texture
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
    
    // Render the weather tint overlay
    renderWeatherTint(weather);

    // Cleanup resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void View::renderWeatherTint(const Weather& weather) {
    // Define the mapping of strings to actions
    std::map<std::string, int> map = {
        {"Acid Rain", 1},
        {"Sandstorm", 2},
        {"Radiation", 3},
        {"Earthquake", 4},
        {"East Wind", 5},
	    {"West Wind", 6}
    };

    std::string name = weather.getWeatherName();
    
   // Render tint to screen based on weather
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    switch(map[name]) {
        case 1: // Acid Rain
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 50); // Green
            break;
        case 2: // Sandstorm
            SDL_SetRenderDrawColor(renderer, 244, 164, 96, 50); // Sand color
            break;
        case 3: // Radiation
            SDL_SetRenderDrawColor(renderer, 0, 128, 0, 50); // Neon green
            break;
        case 4: // Earthquake
            SDL_SetRenderDrawColor(renderer, 245, 245, 220, 50); // Beige
            break;
        case 5: // Wind
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50); // White
            break;
	    case 6:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50); // White
            break;
	    default: 
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // No color
            break;
    }

    SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlayRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void View::renderFailedTransMessage() {
    SDL_SetRenderDrawColor(renderer, 220, 99, 83, 1);
    SDL_Rect messageRect = {(SCREEN_WIDTH/2)-(SCREEN_WIDTH/8), SCREEN_HEIGHT/100, SCREEN_WIDTH/4, SCREEN_HEIGHT/10};
    SDL_RenderFillRect(renderer, &messageRect);

    SDL_Color textColor = { 255, 255, 255 };
    TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 30);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Insufficient Funds", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { messageRect.x + messageRect.w / 8 ,  messageRect.y + messageRect.h / 8 , textSurface->w, (textSurface->h ) };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Cleanup resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

View::~View() {
    SDL_DestroyTexture(barracksTexture);
    SDL_DestroyTexture(bombTexture);
    SDL_DestroyTexture(laserTexture);
    SDL_DestroyTexture(barracksSoldierTexture);
    SDL_DestroyTexture(barracksUpgradeTexture);
    SDL_DestroyTexture(bombUpgradeTexture);
    SDL_DestroyTexture(laserUpgradeTexture);
    
    SDL_DestroyTexture(humanRaiderTexture);
    SDL_DestroyTexture(hawkNorthTexture);
    SDL_DestroyTexture(mutantHawkNorthTexture);
    SDL_DestroyTexture(hawkSouthTexture);
    SDL_DestroyTexture(mutantHawkSouthTexture);
    SDL_DestroyTexture(hawkEastTexture);
    SDL_DestroyTexture(mutantHawkEastTexture);
    SDL_DestroyTexture(dogTexture);
    SDL_DestroyTexture(humanRaiderTexture);
    SDL_DestroyTexture(mutantDogTexture);
    SDL_DestroyTexture(humanRaiderLeaderTexture);
    SDL_DestroyTexture(mutantRaiderTexture);
    SDL_DestroyTexture(mutantLeaderTexture);

    SDL_DestroyTexture(bombEffectTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
