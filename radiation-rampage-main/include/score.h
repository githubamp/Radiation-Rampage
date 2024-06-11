#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <vector>

class Score {
private:
    int score;
    std::string date;

public:
    Score(std::string n, int s) : score(s), date(n) {}
    std::string getString() const {std::string str = " " + date + "          " +  std::to_string(score); return str; };
    int getScore() const { return score; };
    std::string getDate() const { return date; };
};



#endif