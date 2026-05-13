/*
    Mini Project C++ OOP: BattleShip Strategy Benchmark
    ----------------------------------------------------
    - Mo phong 5 chien thuat ban tau va 1 chien thuat dat tau ngau nhien.
    - Chay 25 matchup, moi matchup mac dinh 100000 tran.

*/

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

using namespace std;

const int BOARD_SIZE = 10;
const int SHIP_TYPE_COUNT = 5;

struct ShipInfo {
    string name;
    int size;
};

const ShipInfo DEFAULT_SHIPS[SHIP_TYPE_COUNT] = {
    {"Destroyer", 2},
    {"Submarine", 3},
    {"Cruiser", 3},
    {"Battleship", 4},
    {"Carrier", 5}
};

// Luu toa do tren ban co.
struct Position {
    int row;
    int col;

    Position(int r = -1, int c = -1) {
        row = r;
        col = c;
    }

    bool isValid() const {
        return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
    }

    bool equals(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

enum class ShotResultType {
    MISS,
    HIT,
    SUNK
};

struct ShotResult {
    ShotResultType type;
    string shipName;

    ShotResult(ShotResultType t = ShotResultType::MISS, string name = "") {
        type = t;
        shipName = name;
    }

    bool isHit() const {
        return type == ShotResultType::HIT || type == ShotResultType::SUNK;
    }
};

// Ship dong goi thong tin cua tung tau: ten, kich thuoc, vi tri, so lan bi ban trung.
class Ship {
private:
    string name;
    int size;
    vector<Position> positions;
    int hitCount;

public:
    Ship() {
        name = "";
        size = 0;
        hitCount = 0;
    }

    Ship(string shipName, int shipSize) {
        name = shipName;
        size = shipSize;
        hitCount = 0;
    }

    string getName() const {
        return name;
    }

    int getSize() const {
        return size;
    }

    int getHitCount() const {
        return hitCount;
    }

    vector<Position> getPositions() const {
        return positions;
    }

    void addPosition(Position p) {
        positions.push_back(p);
    }

    bool occupies(Position p) const {
        for (size_t i = 0; i < positions.size(); i++) {
            if (positions[i].equals(p)) {
                return true;
            }
        }
        return false;
    }

    void receiveHit() {
        if (hitCount < size) {
            hitCount++;
        }
    }

    bool isSunk() const {
        return hitCount >= size;
    }
};

// Board quan ly mang 2 chieu 10x10, danh sach tau va ket qua ban.
class Board {
private:
    int shipIndex[BOARD_SIZE][BOARD_SIZE];   // -1 la khong co tau, >=0 la index trong vector ships
    bool wasShot[BOARD_SIZE][BOARD_SIZE];    // true neu o nay da bi ban
    vector<Ship> ships;

public:
    Board() {
        reset();
    }

    void reset() {
        ships.clear();
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                shipIndex[r][c] = -1;
                wasShot[r][c] = false;
            }
        }
    }

    bool canPlaceShip(Position start, int size, bool horizontal) const {
        for (int i = 0; i < size; i++) {
            int r = start.row + (horizontal ? 0 : i);
            int c = start.col + (horizontal ? i : 0);

            Position p(r, c);
            if (!p.isValid()) {
                return false;
            }
            if (shipIndex[r][c] != -1) {
                return false;
            }
        }
        return true;
    }

    bool placeShip(string name, int size, Position start, bool horizontal) {
        if (!canPlaceShip(start, size, horizontal)) {
            return false;
        }

        Ship ship(name, size);
        int index = (int)ships.size();

        for (int i = 0; i < size; i++) {
            int r = start.row + (horizontal ? 0 : i);
            int c = start.col + (horizontal ? i : 0);
            ship.addPosition(Position(r, c));
            shipIndex[r][c] = index;
        }

        ships.push_back(ship);
        return true;
    }

    ShotResult receiveShot(Position p) {
        if (!p.isValid()) {
            return ShotResult(ShotResultType::MISS);
        }

        // Neu chien thuat bi loi va ban trung o cu, khong tinh hit lan nua.
        if (wasShot[p.row][p.col]) {
            return ShotResult(ShotResultType::MISS);
        }

        wasShot[p.row][p.col] = true;
        int index = shipIndex[p.row][p.col];

        if (index == -1) {
            return ShotResult(ShotResultType::MISS);
        }

        ships[index].receiveHit();

        if (ships[index].isSunk()) {
            return ShotResult(ShotResultType::SUNK, ships[index].getName());
        }

        return ShotResult(ShotResultType::HIT, ships[index].getName());
    }

    bool allShipsSunk() const {
        if (ships.empty()) {
            return false;
        }

        for (size_t i = 0; i < ships.size(); i++) {
            if (!ships[i].isSunk()) {
                return false;
            }
        }
        return true;
    }
};

// PlacementStrategy la interface cho cac cach dat tau.
class PlacementStrategy {
public:
    virtual ~PlacementStrategy() {}
    virtual void placeShips(Board& board, mt19937& rng) = 0;
    virtual string getName() const = 0;
    virtual PlacementStrategy* clone() const = 0;
};

// Dat 5 tau vao vi tri ngau nhien hop le, khong chong len nhau.
class RandomPlacementStrategy : public PlacementStrategy {
public:
    void placeShips(Board& board, mt19937& rng) {
        uniform_int_distribution<int> posDist(0, BOARD_SIZE - 1);
        uniform_int_distribution<int> dirDist(0, 1);

        for (int i = 0; i < SHIP_TYPE_COUNT; i++) {
            bool placed = false;

            while (!placed) {
                int row = posDist(rng);
                int col = posDist(rng);
                bool horizontal = (dirDist(rng) == 1);

                placed = board.placeShip(
                    DEFAULT_SHIPS[i].name,
                    DEFAULT_SHIPS[i].size,
                    Position(row, col),
                    horizontal
                );
            }
        }
    }

    string getName() const {
        return "Random Placement";
    }

    PlacementStrategy* clone() const {
        return new RandomPlacementStrategy(*this);
    }
};

// Strategy la lop cha truu tuong cho cac chien thuat ban.
// Player chi goi Strategy* -> chooseShot(), nen ham that su chay se duoc quyet dinh luc runtime.
class Strategy {
protected:
    bool fired[BOARD_SIZE][BOARD_SIZE];  // fired[r][c] = da ban o nay chua

    void resetFiredMatrix() {
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                fired[r][c] = false;
            }
        }
    }

    bool canShoot(Position p) const {
        return p.isValid() && !fired[p.row][p.col];
    }

    void markFired(Position p) {
        if (p.isValid()) {
            fired[p.row][p.col] = true;
        }
    }

    Position chooseRandomUnfired(mt19937& rng) {
        vector<Position> candidates;

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (!fired[r][c]) {
                    candidates.push_back(Position(r, c));
                }
            }
        }

        if (candidates.empty()) {
            return Position(-1, -1);
        }

        uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
        return candidates[dist(rng)];
    }

    Position chooseRandomFromVector(const vector<Position>& candidates, mt19937& rng) {
        if (candidates.empty()) {
            return Position(-1, -1);
        }

        uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
        return candidates[dist(rng)];
    }

    int centerDistanceScore(Position p) const {
        // Dung toa do nhan 2 de tranh so thuc: tam ban co 10x10 nam giua 4.5 va 4.5.
        int dr = 2 * p.row - 9;
        int dc = 2 * p.col - 9;
        return dr * dr + dc * dc;
    }

public:
    Strategy() {
        resetFiredMatrix();
    }

    virtual ~Strategy() {}
    virtual string getName() const = 0;
    virtual Position chooseShot(mt19937& rng) = 0;
    virtual void processShotResult(Position p, ShotResultType result) = 0;
    virtual Strategy* clone() const = 0;

    virtual void reset() {
        resetFiredMatrix();
    }
};

// Bot ban ngau nhien vao mot o chua tung ban.
class RandomStrategy : public Strategy {
public:
    string getName() const {
        return "Random";
    }

    Position chooseShot(mt19937& rng) {
        Position p = chooseRandomUnfired(rng);
        markFired(p);
        return p;
    }

    void processShotResult(Position, ShotResultType) {
        // Random Strategy khong can ghi nho ket qua HIT/MISS.
    }

    Strategy* clone() const {
        return new RandomStrategy(*this);
    }
};

// Bot uu tien cac o gan bien/goc truoc, sau do ban random cac o con lai.
class EdgeBiasStrategy : public Strategy {
private:
    vector<Position> edgeOrder;

    int edgeDistanceScore(Position p) const {
        int top = p.row;
        int left = p.col;
        int bottom = BOARD_SIZE - 1 - p.row;
        int right = BOARD_SIZE - 1 - p.col;
        return min(min(top, bottom), min(left, right));
    }

    void buildEdgeOrder() {
        edgeOrder.clear();

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                edgeOrder.push_back(Position(r, c));
            }
        }

        sort(edgeOrder.begin(), edgeOrder.end(), [this](const Position& a, const Position& b) {
            int edgeA = edgeDistanceScore(a);
            int edgeB = edgeDistanceScore(b);

            if (edgeA != edgeB) {
                return edgeA < edgeB;
            }

            // Neu cung khoang cach toi bien, uu tien o xa trung tam hon.
            return centerDistanceScore(a) > centerDistanceScore(b);
        });
    }

public:
    EdgeBiasStrategy() {
        buildEdgeOrder();
    }

    string getName() const {
        return "EdgeBias";
    }

    Position chooseShot(mt19937& rng) {
        for (size_t i = 0; i < edgeOrder.size(); i++) {
            if (canShoot(edgeOrder[i])) {
                markFired(edgeOrder[i]);
                return edgeOrder[i];
            }
        }

        Position p = chooseRandomUnfired(rng);
        markFired(p);
        return p;
    }

    void processShotResult(Position, ShotResultType) {
        // Edge Bias khong co Target mode, chi uu tien vi tri gan bien.
    }

    Strategy* clone() const {
        return new EdgeBiasStrategy(*this);
    }
};
// Bot uu tien vung trung tam 6x6, sau do ban random cac o con lai.
class CenterBiasStrategy : public Strategy {
private:
    vector<Position> centerOrder;

    void buildCenterOrder() {
        centerOrder.clear();

        for (int r = 2; r <= 7; r++) {
            for (int c = 2; c <= 7; c++) {
                centerOrder.push_back(Position(r, c));
            }
        }

        sort(centerOrder.begin(), centerOrder.end(), [this](const Position& a, const Position& b) {
            return centerDistanceScore(a) < centerDistanceScore(b);
        });
    }

public:
    CenterBiasStrategy() {
        buildCenterOrder();
    }

    string getName() const {
        return "CenterBias";
    }

    Position chooseShot(mt19937& rng) {
        for (size_t i = 0; i < centerOrder.size(); i++) {
            if (canShoot(centerOrder[i])) {
                markFired(centerOrder[i]);
                return centerOrder[i];
            }
        }

        Position p = chooseRandomUnfired(rng);
        markFired(p);
        return p;
    }

    void processShotResult(Position, ShotResultType) {
        // Center Bias khong can ghi nho ket qua HIT/MISS.
    }

    Strategy* clone() const {
        return new CenterBiasStrategy(*this);
    }
};

// Bot ban theo mau caro: chi uu tien o co (row + col) % 2 == 0.
class CheckerboardStrategy : public Strategy {
public:
    string getName() const {
        return "Checkerboard";
    }

    Position chooseShot(mt19937& rng) {
        vector<Position> checkerCells;

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (!fired[r][c] && (r + c) % 2 == 0) {
                    checkerCells.push_back(Position(r, c));
                }
            }
        }

        Position p;
        if (!checkerCells.empty()) {
            p = chooseRandomFromVector(checkerCells, rng);
        } else {
            p = chooseRandomUnfired(rng);
        }

        markFired(p);
        return p;
    }

    void processShotResult(Position, ShotResultType) {
        // Checkerboard don gian: khong Target, chi giu ma tran da ban trong lop cha.
    }

    Strategy* clone() const {
        return new CheckerboardStrategy(*this);
    }
};

// Bo nho phu cho Target mode. Dung cho HuntTarget.
struct TargetMemory {
    bool targetMode;
    vector<Position> hits;

    TargetMemory() {
        targetMode = false;
    }

    void reset() {
        targetMode = false;
        hits.clear();
    }

    void addHit(Position p) {
        for (size_t i = 0; i < hits.size(); i++) {
            if (hits[i].equals(p)) {
                return;
            }
        }

        targetMode = true;
        hits.push_back(p);
    }

    bool isValidUnfired(Position p, bool fired[BOARD_SIZE][BOARD_SIZE]) const {
        return p.isValid() && !fired[p.row][p.col];
    }

    bool alreadyInVector(const vector<Position>& list, Position p) const {
        for (size_t i = 0; i < list.size(); i++) {
            if (list[i].equals(p)) {
                return true;
            }
        }
        return false;
    }

    void addCandidate(vector<Position>& candidates, Position p, bool fired[BOARD_SIZE][BOARD_SIZE]) const {
        if (isValidUnfired(p, fired) && !alreadyInVector(candidates, p)) {
            candidates.push_back(p);
        }
    }

    Position chooseTargetShot(bool fired[BOARD_SIZE][BOARD_SIZE], mt19937& rng) const {
        if (!targetMode || hits.empty()) {
            return Position(-1, -1);
        }

        vector<Position> candidates;

        // Neu co tu 2 HIT thang hang, uu tien ban tiep o hai dau cua doan tau.
        if (hits.size() >= 2) {
            bool sameRow = true;
            bool sameCol = true;

            for (size_t i = 1; i < hits.size(); i++) {
                if (hits[i].row != hits[0].row) {
                    sameRow = false;
                }
                if (hits[i].col != hits[0].col) {
                    sameCol = false;
                }
            }

            if (sameRow) {
                int row = hits[0].row;
                int minCol = hits[0].col;
                int maxCol = hits[0].col;

                for (size_t i = 1; i < hits.size(); i++) {
                    minCol = min(minCol, hits[i].col);
                    maxCol = max(maxCol, hits[i].col);
                }

                addCandidate(candidates, Position(row, minCol - 1), fired);
                addCandidate(candidates, Position(row, maxCol + 1), fired);
            }

            if (sameCol) {
                int col = hits[0].col;
                int minRow = hits[0].row;
                int maxRow = hits[0].row;

                for (size_t i = 1; i < hits.size(); i++) {
                    minRow = min(minRow, hits[i].row);
                    maxRow = max(maxRow, hits[i].row);
                }

                addCandidate(candidates, Position(minRow - 1, col), fired);
                addCandidate(candidates, Position(maxRow + 1, col), fired);
            }

            if (!candidates.empty()) {
                uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
                return candidates[dist(rng)];
            }
        }

        // Neu moi co 1 HIT, ban 4 o xung quanh: tren, duoi, trai, phai.
        int dr[4] = {-1, 1, 0, 0};
        int dc[4] = {0, 0, -1, 1};

        for (size_t i = 0; i < hits.size(); i++) {
            for (int k = 0; k < 4; k++) {
                Position next(hits[i].row + dr[k], hits[i].col + dc[k]);
                addCandidate(candidates, next, fired);
            }
        }

        if (candidates.empty()) {
            return Position(-1, -1);
        }

        uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
        return candidates[dist(rng)];
    }
};

// Hunt & Target:
// - Hunt mode: ban random o chua ban.
// - Target mode: sau HIT, ban quanh diem HIT va mo rong theo huong tau.
class HuntTargetStrategy : public Strategy {
private:
    TargetMemory targetMemory;

public:
    string getName() const {
        return "HuntTarget";
    }

    Position chooseShot(mt19937& rng) {
        Position targetShot = targetMemory.chooseTargetShot(fired, rng);

        if (targetShot.isValid()) {
            markFired(targetShot);
            return targetShot;
        }

        // Neu khong con o target hop le, quay lai Hunt mode.
        targetMemory.reset();

        Position p = chooseRandomUnfired(rng);
        markFired(p);
        return p;
    }

    void processShotResult(Position p, ShotResultType result) {
        if (result == ShotResultType::HIT) {
            targetMemory.addHit(p);
        } else if (result == ShotResultType::SUNK) {
            targetMemory.reset();
        }
    }

    void reset() {
        Strategy::reset();
        targetMemory.reset();
    }

    Strategy* clone() const {
        return new HuntTargetStrategy(*this);
    }
};


struct ShotRecord {
    Position position;
    ShotResult result;
};

// Player co Board rieng, Strategy ban va PlacementStrategy dat tau.
// Player so huu hai con tro nay, nen destructor phai delete de tranh ro ri bo nho.
class Player {
private:
    string name;
    Board board;
    Strategy* shootingStrategy;
    PlacementStrategy* placementStrategy;

public:
    Player(string playerName, Strategy* strategy, PlacementStrategy* placement) {
        name = playerName;
        shootingStrategy = strategy;
        placementStrategy = placement;
    }

    ~Player() {
        delete shootingStrategy;
        delete placementStrategy;
    }

    void resetForNewGame(mt19937& rng) {
        board.reset();
        shootingStrategy->reset();
        placementStrategy->placeShips(board, rng);
    }

    ShotRecord shoot(Player& enemy, mt19937& rng) {
        ShotRecord record;
        record.position = shootingStrategy->chooseShot(rng);
        record.result = enemy.board.receiveShot(record.position);
        shootingStrategy->processShotResult(record.position, record.result.type);
        return record;
    }

    bool hasLost() const {
        return board.allShipsSunk();
    }

    string getName() const {
        return name;
    }

    string getStrategyName() const {
        return shootingStrategy->getName();
    }
};

struct GameResult {
    int winner;         // 1 hoac 2
    int p1Shots;
    int p2Shots;
    int p1Hits;
    int p2Hits;
    int p1Misses;
    int p2Misses;
    int totalShots;
    int totalHits;
    int totalMisses;
    int winnerShots;    // So phat ban cua nguoi thang

    GameResult() {
        winner = 0;
        p1Shots = p2Shots = 0;
        p1Hits = p2Hits = 0;
        p1Misses = p2Misses = 0;
        totalShots = totalHits = totalMisses = 0;
        winnerShots = 0;
    }
};

// Game mo phong mot tran dau tu dong giua 2 Player.
class Game {
private:
    Player player1;
    Player player2;

    void recordShot(GameResult& result, const ShotRecord& shot, bool fromPlayer1,
                    long long hitMatrix[BOARD_SIZE][BOARD_SIZE]) {
        if (fromPlayer1) {
            result.p1Shots++;
        } else {
            result.p2Shots++;
        }

        result.totalShots++;

        if (shot.result.isHit()) {
            if (fromPlayer1) {
                result.p1Hits++;
            } else {
                result.p2Hits++;
            }

            result.totalHits++;
            hitMatrix[shot.position.row][shot.position.col]++;
        } else {
            if (fromPlayer1) {
                result.p1Misses++;
            } else {
                result.p2Misses++;
            }

            result.totalMisses++;
        }
    }

public:
    Game(Strategy* s1, Strategy* s2, PlacementStrategy* p1, PlacementStrategy* p2)
        : player1("Bot 1", s1, p1), player2("Bot 2", s2, p2) {
    }

    GameResult play(mt19937& rng, bool player1Starts,
                    long long hitMatrix[BOARD_SIZE][BOARD_SIZE]) {
        GameResult result;

        player1.resetForNewGame(rng);
        player2.resetForNewGame(rng);

        bool p1Turn = player1Starts;

        // Toi da moi bot ban 100 o, nen 200 phat la gioi han an toan.
        while (result.winner == 0 && result.totalShots < BOARD_SIZE * BOARD_SIZE * 2) {
            bool currentShotHit = false;

            if (p1Turn) {
                ShotRecord shot = player1.shoot(player2, rng);
                recordShot(result, shot, true, hitMatrix);
                currentShotHit = shot.result.isHit();

                if (player2.hasLost()) {
                    result.winner = 1;
                    result.winnerShots = result.p1Shots;
                    break;
                }
            } else {
                ShotRecord shot = player2.shoot(player1, rng);
                recordShot(result, shot, false, hitMatrix);
                currentShotHit = shot.result.isHit();

                if (player1.hasLost()) {
                    result.winner = 2;
                    result.winnerShots = result.p2Shots;
                    break;
                }
            }

            // Theo luat: ban trung thi duoc ban tiep, ban truot moi mat luot.
            if (!currentShotHit) {
                p1Turn = !p1Turn;
            }
        }

        return result;
    }
};

struct Statistics {
    string bot1Name;
    string bot2Name;
    long long totalGames;
    long long bot1Wins;
    long long bot2Wins;
    long long totalShots;
    long long totalHits;
    long long totalMisses;
    long long winnerShotsSum;
    long long hitMatrix[BOARD_SIZE][BOARD_SIZE];

    Statistics(string b1 = "", string b2 = "", long long games = 0) {
        bot1Name = b1;
        bot2Name = b2;
        totalGames = games;
        bot1Wins = bot2Wins = 0;
        totalShots = totalHits = totalMisses = 0;
        winnerShotsSum = 0;

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                hitMatrix[r][c] = 0;
            }
        }
    }

    double bot1Winrate() const {
        return totalGames == 0 ? 0.0 : 100.0 * bot1Wins / totalGames;
    }

    double bot2Winrate() const {
        return totalGames == 0 ? 0.0 : 100.0 * bot2Wins / totalGames;
    }

    double accuracy() const {
        return totalShots == 0 ? 0.0 : 100.0 * totalHits / totalShots;
    }

    double averageShotsPerGame() const {
        return totalGames == 0 ? 0.0 : (double)totalShots / totalGames;
    }

    double averageTurnsToWin() const {
        return totalGames == 0 ? 0.0 : (double)winnerShotsSum / totalGames;
    }
};

struct StrategyStatistics {
    string name;
    long long games;
    long long wins;
    long long losses;
    long long shots;
    long long hits;
    long long misses;
    long long winningShotsSum;

    StrategyStatistics(string strategyName = "") {
        name = strategyName;
        games = wins = losses = 0;
        shots = hits = misses = 0;
        winningShotsSum = 0;
    }

    double winrate() const {
        return games == 0 ? 0.0 : 100.0 * wins / games;
    }

    double accuracy() const {
        return shots == 0 ? 0.0 : 100.0 * hits / shots;
    }

    double averageShotsPerGame() const {
        return games == 0 ? 0.0 : (double)shots / games;
    }

    double averageTurnsToWin() const {
        return wins == 0 ? 0.0 : (double)winningShotsSum / wins;
    }
};

// Simulation chay tat ca matchup, gom thong ke, in report va xuat CSV.
class Simulation {
private:
    vector<Strategy*> strategies;
    PlacementStrategy* placementPrototype;
    vector<Statistics> matchupStats;
    map<string, StrategyStatistics> strategyStats;
    long long globalHitMatrix[BOARD_SIZE][BOARD_SIZE];
    int gamesPerMatchup;
    double runtimeSeconds;
    mt19937 rng;

    void resetAllStatistics() {
        matchupStats.clear();
        strategyStats.clear();

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                globalHitMatrix[r][c] = 0;
            }
        }

        for (size_t i = 0; i < strategies.size(); i++) {
            strategyStats[strategies[i]->getName()] = StrategyStatistics(strategies[i]->getName());
        }
    }

    void updateStrategyStatistics(const string& name, bool isBot1, const GameResult& result) {
        StrategyStatistics& stat = strategyStats[name];
        stat.games++;

        bool won = (isBot1 && result.winner == 1) || (!isBot1 && result.winner == 2);

        if (won) {
            stat.wins++;
            stat.winningShotsSum += isBot1 ? result.p1Shots : result.p2Shots;
        } else {
            stat.losses++;
        }

        if (isBot1) {
            stat.shots += result.p1Shots;
            stat.hits += result.p1Hits;
            stat.misses += result.p1Misses;
        } else {
            stat.shots += result.p2Shots;
            stat.hits += result.p2Hits;
            stat.misses += result.p2Misses;
        }
    }

    vector<StrategyStatistics> getRanking() const {
        vector<StrategyStatistics> ranking;

        for (map<string, StrategyStatistics>::const_iterator it = strategyStats.begin();
             it != strategyStats.end(); ++it) {
            ranking.push_back(it->second);
        }

        sort(ranking.begin(), ranking.end(), [](const StrategyStatistics& a, const StrategyStatistics& b) {
            if (a.winrate() != b.winrate()) {
                return a.winrate() > b.winrate();
            }
            if (a.accuracy() != b.accuracy()) {
                return a.accuracy() > b.accuracy();
            }
            return a.averageTurnsToWin() < b.averageTurnsToWin();
        });

        return ranking;
    }

    void printLowRiskCells() const {
        long long minHit = globalHitMatrix[0][0];
        long long maxHit = globalHitMatrix[0][0];

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                minHit = min(minHit, globalHitMatrix[r][c]);
                maxHit = max(maxHit, globalHitMatrix[r][c]);
            }
        }

        cout << "\nLowest HIT cells (goi y vung dat tau kho bi trung hon): ";
        int printed = 0;
        for (int r = 0; r < BOARD_SIZE && printed < 10; r++) {
            for (int c = 0; c < BOARD_SIZE && printed < 10; c++) {
                if (globalHitMatrix[r][c] == minHit) {
                    cout << "(" << r << "," << c << ") ";
                    printed++;
                }
            }
        }
        cout << "\nHighest HIT count: " << maxHit << " | Lowest HIT count: " << minHit << "\n";
    }

public:
    Simulation(int games = 100000) {
        gamesPerMatchup = games;
        runtimeSeconds = 0.0;

        unsigned seed = (unsigned)chrono::system_clock::now().time_since_epoch().count();
        rng.seed(seed);

        strategies.push_back(new RandomStrategy());
        strategies.push_back(new EdgeBiasStrategy());
        strategies.push_back(new CenterBiasStrategy());
        strategies.push_back(new CheckerboardStrategy());
        strategies.push_back(new HuntTargetStrategy());

        placementPrototype = new RandomPlacementStrategy();
        resetAllStatistics();
    }

    ~Simulation() {
        for (size_t i = 0; i < strategies.size(); i++) {
            delete strategies[i];
        }
        delete placementPrototype;
    }

    // Chay 1 matchup giua 2 chien thuat.
    void runMatchup(Strategy* s1, Strategy* s2, int totalGames) {
        Statistics stats(s1->getName(), s2->getName(), totalGames);

        Game game(
            s1->clone(),
            s2->clone(),
            placementPrototype->clone(),
            placementPrototype->clone()
        );

        for (int gameIndex = 0; gameIndex < totalGames; gameIndex++) {
            // Doi nguoi di truoc theo tung tran de giam loi the bot 1.
            bool player1Starts = (gameIndex % 2 == 0);
            GameResult result = game.play(rng, player1Starts, stats.hitMatrix);

            if (result.winner == 1) {
                stats.bot1Wins++;
            } else if (result.winner == 2) {
                stats.bot2Wins++;
            }

            stats.totalShots += result.totalShots;
            stats.totalHits += result.totalHits;
            stats.totalMisses += result.totalMisses;
            stats.winnerShotsSum += result.winnerShots;

            updateStrategyStatistics(s1->getName(), true, result);
            updateStrategyStatistics(s2->getName(), false, result);
        }

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                globalHitMatrix[r][c] += stats.hitMatrix[r][c];
            }
        }

        matchupStats.push_back(stats);
    }

    void runAllMatchups() {
        resetAllStatistics();

        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        for (size_t i = 0; i < strategies.size(); i++) {
            for (size_t j = 0; j < strategies.size(); j++) {
                runMatchup(strategies[i], strategies[j], gamesPerMatchup);
            }
        }

        chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        runtimeSeconds = chrono::duration<double>(end - start).count();

        collectStatistics();
        printFinalReport();
        exportCSV();
    }

    void collectStatistics() {
        // Thong ke da duoc cap nhat truc tiep trong runMatchup().
        // Ham nay duoc giu de dung voi thiet ke yeu cau cua de bai.
    }

    void printFinalReport() {
        cout << fixed << setprecision(2);
        cout << "=== BATTLESHIP STRATEGY BENCHMARK REPORT ===\n";
        cout << "Total strategies: " << strategies.size() << "\n";
        cout << "Total matchups: " << matchupStats.size() << "\n";
        cout << "Games per matchup: " << gamesPerMatchup << "\n";
        cout << "Placement strategy: " << placementPrototype->getName() << "\n";
        cout << "Runtime: " << runtimeSeconds << " seconds\n\n";

        for (size_t i = 0; i < matchupStats.size(); i++) {
            const Statistics& s = matchupStats[i];

            cout << "Matchup: " << s.bot1Name << " vs " << s.bot2Name << "\n";
            cout << "Total games: " << s.totalGames << "\n";
            cout << "Bot1 (" << s.bot1Name << ") wins: " << s.bot1Wins << "\n";
            cout << "Bot2 (" << s.bot2Name << ") wins: " << s.bot2Wins << "\n";
            cout << "Bot1 (" << s.bot1Name << ") winrate: " << s.bot1Winrate() << "%\n";
            cout << "Bot2 (" << s.bot2Name << ") winrate: " << s.bot2Winrate() << "%\n";
            cout << "Total shots: " << s.totalShots << "\n";
            cout << "Average shots/game: " << s.averageShotsPerGame() << "\n";
            cout << "Total HIT: " << s.totalHits << "\n";
            cout << "Total MISS: " << s.totalMisses << "\n";
            cout << "Accuracy: " << s.accuracy() << "%\n";
            cout << "Average turns to win: " << s.averageTurnsToWin() << "\n\n";
        }

        vector<StrategyStatistics> ranking = getRanking();

        cout << "=== STRATEGY RANKING ===\n";
        for (size_t i = 0; i < ranking.size(); i++) {
            cout << (i + 1) << ". " << ranking[i].name
                 << " | Winrate: " << ranking[i].winrate() << "%"
                 << " | Accuracy: " << ranking[i].accuracy() << "%"
                 << " | Avg winner shots: " << ranking[i].averageTurnsToWin()
                 << " | Avg shots/game: " << ranking[i].averageShotsPerGame()
                 << "\n";
        }

        cout << "\n=== HIT MATRIX 10x10 ===\n";
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                cout << setw(9) << globalHitMatrix[r][c] << " ";
            }
            cout << "\n";
        }

        printLowRiskCells();

        if (!ranking.empty()) {
            cout << "\nBest shooting strategy by benchmark: " << ranking[0].name << "\n";
        }

        cout << "CSV exported to: battleship_results.csv\n";
    }

    void exportCSV() {
        ofstream file("battleship_results.csv");

        if (!file.is_open()) {
            cout << "Cannot write battleship_results.csv\n";
            return;
        }

        file << fixed << setprecision(4);

        file << "MATCHUP STATISTICS\n";
        file << "Bot1,Bot2,TotalGames,Bot1Wins,Bot2Wins,Bot1Winrate,Bot2Winrate,"
             << "TotalShots,AverageShotsPerGame,TotalHits,TotalMisses,Accuracy,AverageTurnsToWin\n";

        for (size_t i = 0; i < matchupStats.size(); i++) {
            const Statistics& s = matchupStats[i];
            file << s.bot1Name << ","
                 << s.bot2Name << ","
                 << s.totalGames << ","
                 << s.bot1Wins << ","
                 << s.bot2Wins << ","
                 << s.bot1Winrate() << ","
                 << s.bot2Winrate() << ","
                 << s.totalShots << ","
                 << s.averageShotsPerGame() << ","
                 << s.totalHits << ","
                 << s.totalMisses << ","
                 << s.accuracy() << ","
                 << s.averageTurnsToWin() << "\n";
        }

        file << "\nSTRATEGY RANKING\n";
        file << "Rank,Strategy,Games,Wins,Losses,Winrate,Shots,Hits,Misses,Accuracy,AverageWinnerShots,AverageShotsPerGame\n";

        vector<StrategyStatistics> ranking = getRanking();
        for (size_t i = 0; i < ranking.size(); i++) {
            const StrategyStatistics& s = ranking[i];
            file << (i + 1) << ","
                 << s.name << ","
                 << s.games << ","
                 << s.wins << ","
                 << s.losses << ","
                 << s.winrate() << ","
                 << s.shots << ","
                 << s.hits << ","
                 << s.misses << ","
                 << s.accuracy() << ","
                 << s.averageTurnsToWin() << ","
                 << s.averageShotsPerGame() << "\n";
        }

        file << "\nHIT MATRIX 10x10\n";
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                file << globalHitMatrix[r][c];
                if (c < BOARD_SIZE - 1) {
                    file << ",";
                }
            }
            file << "\n";
        }

        file.close();
    }
};

int main(int argc, char* argv[]) {
    int gamesPerMatchup = 100000;

    if (argc >= 2) {
        int userValue = atoi(argv[1]);
        if (userValue > 0) {
            gamesPerMatchup = userValue;
        }
    }

    Simulation simulation(gamesPerMatchup);
    simulation.runAllMatchups();

    return 0;
}



