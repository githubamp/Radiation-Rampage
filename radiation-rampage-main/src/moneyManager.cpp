#include "moneyManager.h"
#include "constants.h"

MoneyManager::MoneyManager(){
    using namespace moneyManager;
    money = moneyManager::startAmount;
    timeSinceLastSlowGain = 0.0;
}

bool MoneyManager::spendMoney(int amount){
    if ((money - amount) >= 0){
        money-=amount;
        return true;
    }else{
        return false;
    }
}

void MoneyManager::gainMoney(int amount){
    money+=amount;
}

int MoneyManager::getMoney() const{
    return money;
}

void MoneyManager::slowGain(){
    using namespace moneyManager;
    gainMoney(moneyManager::slowGainAmount);
}

bool MoneyManager::isReadyToSlowGain(double elapsedTime){
    using namespace moneyManager;
    timeSinceLastSlowGain += elapsedTime;    
    if (timeSinceLastSlowGain < moneyManager::secondsBetweenSlowGain){
        return false;
    }
    // Enough time has passed. Can slow gain now
    timeSinceLastSlowGain = 0.0;
    return true;

}

void MoneyManager::reset(){
    money = moneyManager::startAmount;
    timeSinceLastSlowGain = 0.0;
}