#ifndef MONEYMANAGER_H
#define MONEYMANAGER_H

class MoneyManager {
private:
    int money;
    double timeSinceLastSlowGain;

public:
    MoneyManager();

    // Decreases money by amount.
    // Returns true if transaction is successful (sufficient funds), false otherwise.
    bool spendMoney(int amount);
    void gainMoney(int amount);

    bool isReadyToSlowGain(double elapsedTime); // Checks if enough time has passed to slowly gain money
    void slowGain(); // Increments money by constant amount

    // Reset method
    void reset();

    // Getters 
    int getMoney() const;
};

#endif