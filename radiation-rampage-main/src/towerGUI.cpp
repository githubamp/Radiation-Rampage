#include "towerGUI.h"
#include "constants.h"

TOWERGUI::TOWERGUI(SDL_Renderer* renderer) : renderer(renderer), visible(false), location(0, 0) {
    options = { "Barracks", "Bomb", "Laser" }; // No tower in location
    updateOptions = { "Upgrade", "Repair", "Sell" }; // Tower in location
    upgradedOptions = {"Repair", "Sell" }; // Fully upgraded tower in location
}

TOWERGUI::~TOWERGUI() {
    for (auto& pair : towerTextures) {
        SDL_DestroyTexture(pair.second);
    }
}

void TOWERGUI::show(const TowerLocation& towerLocation) {
    visible = true;
    location = towerLocation;
}

TowerLocation TOWERGUI::getLocation() {
    return location;
}

void TOWERGUI::render() {
    using namespace tower;
    if (!visible) return;

    int adjustedY = location.y - 30; // Render GUI above tower
    int rectHeight = 25;
    int rectWidth = 195;
    int centerX = location.x - rectWidth / 2; // Center GUI above tower

    // Render GUI background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_Rect guiRect = { centerX, adjustedY, rectWidth, rectHeight };
    SDL_RenderFillRect(renderer, &guiRect);

    // Determine options based on if the location is occupied and the tower is fully upgraded
    const auto& currentOptions = (location.occupied) ?
        (location.tower->isUpgraded() ? upgradedOptions : updateOptions) : options;

    int optionWidth = (currentOptions.size() > 2) ? (rectWidth / currentOptions.size()) : (rectWidth / 2);

    // Render GUI options
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < currentOptions.size(); ++i) {
        SDL_Rect optionRect = { centerX + i * optionWidth, adjustedY, optionWidth, rectHeight };
        SDL_RenderDrawRect(renderer, &optionRect);

        // Render option text
        SDL_Color textColor = { 0, 0, 0 };
        TTF_Font* font = TTF_OpenFont("../resource/arial.ttf", 9);
        int towerActionCoins = 0;
        if (location.occupied){
            // Upgrade tower
            if (currentOptions[i] == "Upgrade"){
                towerActionCoins = location.tower->getUpgradeCost();
            }else if (currentOptions[i] == "Repair"){
                towerActionCoins = location.tower->getRepairCost();
            }else if(currentOptions[i] == "Sell"){
                towerActionCoins = location.tower->getSellEarnings();
            }
        }else{
            // Buy tower
            if (currentOptions[i] == "Barracks"){
                towerActionCoins = tower::barracksBuildCost;
            }else if (currentOptions[i] == "Bomb"){
                towerActionCoins = tower::bombBuildCost;
            }else if(currentOptions[i] == "Laser"){
                towerActionCoins = tower::laserBuildCost;
            }
        }

        // Quick fix for when repair "costs" $0, the repair cannot be applied because tower health is at max
        std::string currentOptionAndCoins;
        if (towerActionCoins == 0){
            currentOptionAndCoins = currentOptions[i] + " N/A";
        }else{
            currentOptionAndCoins = currentOptions[i] + " $" + std::to_string(towerActionCoins);
        }
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, currentOptionAndCoins.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect textRect = { optionRect.x + (optionWidth - textWidth) / 2, optionRect.y + (rectHeight - textHeight) / 2, textWidth, textHeight };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(font);
    }
}

void TOWERGUI::hide() {
    visible = false;
}

bool TOWERGUI::selectTowerType(int mouseX, int mouseY, View* view, Logic& logic) {
    using namespace tower;
    bool clickGUI = false;
    bool isErrorFreeTransaction = true;

    for (int i = 0; i < TowerLocationManager::getTowerLocations().size(); ++i) {
        TowerLocation& location = TowerLocationManager::getTowerLocations()[i];

        // Check if mouse is within bounds of any option
        int rectWidth = 195;
        int optionHeight = 25;

        // Determine options based on if the location is occupied and if the tower is fully upgraded
        const auto& currentOptions = (location.occupied) ?
            (location.tower->isUpgraded() ? upgradedOptions : updateOptions) : options;

        int optionWidth;
        if (currentOptions.size() > 2) {
            optionWidth = rectWidth / currentOptions.size();
        } else {
            // If only two options, each option is half the width
            optionWidth = rectWidth / 2;
        }

        for (int j = 0; j < currentOptions.size(); ++j) {
            int optionX = location.x - rectWidth / 2 + j * optionWidth;
            int optionY = location.y - 30;

            if (mouseX >= optionX && mouseX <= optionX + optionWidth &&
                mouseY >= optionY && mouseY <= optionY + optionHeight) {
                
                // If the location is not occupied, check if player has enough money, then create and store the tower
                if (!location.occupied) {
                    int towerBuildCost = 0;
                    if (currentOptions[j] == "Barracks"){
                        towerBuildCost = tower::barracksBuildCost;
                    }else if (currentOptions[j] == "Bomb"){
                        towerBuildCost = tower::bombBuildCost;
                    }else if(currentOptions[j] == "Laser"){
                        towerBuildCost = tower::laserBuildCost;
                    }
                    if (logic.updateMoneyTowerAction("Buy",towerBuildCost)){
                        // Transaction was successful. Create and store tower. 
                        std::shared_ptr<Tower> tower = Tower::createTower(currentOptions[j], location, view);
                        if (tower) {
                            location.occupied = true;
                            location.towerType = currentOptions[j];
                            location.tower = tower;
                            isErrorFreeTransaction = true;
                        }
                    }else{
                        // Transaction failed. 
                        isErrorFreeTransaction = false;
                    }
                } else {
                    // If the location is occupied, handle tower action
                    isErrorFreeTransaction = handleTowerAction(currentOptions[j], logic);
                }
                clickGUI = true;
                hide();
                break;
            }
        }
    }
    // Check if click occurred within tower area
    if (!clickGUI) {
        for (const auto& location : TowerLocationManager::getTowerLocations()) {
            if (mouseX >= location.x && mouseX <= location.x + location.size &&
                mouseY >= location.y && mouseY <= location.y + location.size) {
                clickGUI = true;
                break;
            }
        }
    }
    // Otherwise, hide GUI if click occured outside of it
    if (!clickGUI) {
        hide();
    }
    return isErrorFreeTransaction;  
}

bool TOWERGUI::handleTowerAction(const std::string& action, Logic& logic) {
    int actionCost = 0;

    if (action == "Upgrade") {
        actionCost = location.tower->getUpgradeCost();
    } else if (action == "Repair") {
        actionCost = location.tower->getRepairCost();
    } else if (action == "Sell") {
        actionCost = location.tower->getSellEarnings();
    }

    // Perform action only if player has enough money
    if (logic.updateMoneyTowerAction(action, actionCost)) {
        if (action == "Upgrade") {
            location.tower->upgrade();
        } else if (action == "Repair") {
            location.tower->repair();
        } else if (action == "Sell") {
            TowerLocationManager::removeTower(*location.tower);
        }

        return true; // Successful transaction
    } else {
        return false; // Failed transaction (not enough money)
    }
}