# Mini Project C++ OOP: BattleShip Strategy Benchmark

## 1. Gioi Thieu Project

Day la mini project C++ console mo phong game BattleShip de phan tich chien thuat ban tau va chien thuat dat tau bang lap trinh huong doi tuong OOP.

Project tap trung vao 2 cau hoi:

1. Neu la nguoi ban tau, chien thuat nao toi uu nhat?
2. Neu la nguoi dat tau, nen dat tau nhu the nao de doi thu kho ban trung?

Project duoc viet theo yeu cau hoc tap cho sinh vien nam nhat/nam hai, uu tien code ro rang, de doc, co comment tieng Viet va chi dung OOP co ban den trung binh.

## 2. File Chinh

Code hoan chinh nam trong 1 file:

```text
C:\Users\admin\Documents\New project\BattleshipOOP_OneFile.cpp
```

File nay co the compile truc tiep bang `g++`, khong can GUI, khong can thu vien phuc tap.

## 3. Cach Compile Va Chay

Mo terminal tai thu muc project:

```bash
cd "C:\Users\admin\Documents\New project"
```

Compile:

```bash
g++ BattleshipOOP_OneFile.cpp -O2 -std=c++11 -o battleship
```

Chay benchmark mac dinh, moi matchup 100000 tran:

```bash
./battleship
```

Chay nhanh de demo, vi du moi matchup 1000 tran:

```bash
./battleship 1000
```

Sau khi chay, chuong trinh se tao file CSV:

```text
battleship_results.csv
```

## 4. Luat Game BattleShip

- Ban choi kich thuoc 10x10.
- Moi nguoi choi co 5 tau:
  - Destroyer: 2 o
  - Submarine: 3 o
  - Cruiser: 3 o
  - Battleship: 4 o
  - Carrier: 5 o
- Tau duoc dat ngau nhien hop le.
- Tau nam ngang hoac doc.
- Tau khong duoc chong len nhau.
- Moi luot ban vao mot toa do.
- Ban trung tau la HIT.
- Ban truot la MISS.
- Ban trung het cac o cua mot tau la SUNK.
- Bot thang khi ban chim toan bo 5 tau cua doi thu.
- Chuong trinh co xu ly tranh ban trung lap mot o.

## 5. Cac Chien Thuat Ban

### 5.1 Center Bias Strategy

Bot uu tien ban cac o gan trung tam ban co truoc. Khi cac o trung tam da ban het, bot chuyen sang ban random cac o con lai.

Uu diem:

- Don gian, de hieu.
- Hop ly khi gia dinh tau thuong xuat hien gan trung tam.

Nhuoc diem:

- Khong co Target mode.
- Ban trung tau nhung khong truy duoi de danh chim tau nhanh.

### 5.2 Checkerboard Strategy

Bot uu tien ban theo mau caro, chi ban cac o thoa:

```cpp
(row + col) % 2 == 0
```

Khi ban het cac o caro, bot chuyen sang random cac o con lai.

Uu diem:

- Giam so o can tim trong giai doan dau.
- Phu hop vi tau nho nhat co kich thuoc 2, nen mau caro giup tang kha nang cham tau.

Nhuoc diem:

- Khong co Target mode.
- Sau khi HIT van khong biet truy duoi tau.

### 5.3 Hunt & Target Strategy

Chien thuat co 2 trang thai:

- Hunt mode: ban random co kiem soat de tim tau.
- Target mode: sau khi HIT, ban cac o xung quanh diem HIT.

Neu co nhieu HIT lien tiep, bot co gang xac dinh huong tau va ban tiep theo huong do.

Uu diem:

- Sau khi tim thay tau, co kha nang danh chim nhanh.
- Manh hon Center Bias va Checkerboard co ban.

Nhuoc diem:

- Hunt mode van random, chua toi uu vung tim kiem.

### 5.4 Hybrid Adaptive Strategy

Day la chien thuat ket hop:

- Center Bias
- Checkerboard
- Hunt & Target

Trong Hunt mode, bot uu tien cac o caro gan trung tam truoc. Khi HIT, bot chuyen sang Target mode nhu Hunt & Target.

Uu diem:

- Tim tau tot hon nho checkerboard.
- Uu tien vung trung tam de tang toc do tim kiem.
- Co Target mode de danh chim tau nhanh sau khi HIT.

Nhuoc diem:

- Code phuc tap hon cac chien thuat con lai.

## 6. Chien Thuat Dat Tau

Project hien tai cai dat:

### Random Placement Strategy

Moi tau duoc dat ngau nhien vao vi tri hop le, khong chong len tau khac.

Ket luan khi doc ma tran HIT:

- Cac o co HIT cao la vung nguy hiem.
- Cac o co HIT thap la vung nen can nhac dat tau.
- Voi cac bot uu tien trung tam, vung bien va goc thuong an toan hon.

## 7. Thiet Ke OOP

### Position

Luu toa do `row`, `col` tren ban co.

### Ship

Quan ly thong tin cua mot tau:

- Ten tau
- Kich thuoc tau
- Danh sach vi tri tau chiem
- So lan bi ban trung
- Kiem tra tau da chim chua

### Board

Quan ly ban co 10x10:

- Dat tau
- Kiem tra vi tri dat tau hop le
- Nhan phat ban
- Tra ve MISS, HIT hoac SUNK
- Kiem tra tat ca tau da chim chua

### Strategy

Lop truu tuong cho chien thuat ban.

Cac ham quan trong:

```cpp
virtual Position chooseShot(mt19937& rng) = 0;
virtual void processShotResult(Position p, ShotResultType result) = 0;
virtual Strategy* clone() const = 0;
```

### PlacementStrategy

Lop truu tuong cho chien thuat dat tau.

```cpp
virtual void placeShips(Board& board, mt19937& rng) = 0;
```

### Player

Moi player co:

- Mot `Board`
- Mot `Strategy*`
- Mot `PlacementStrategy*`

Player khong can biet dang dung chien thuat nao. Player chi goi ham ao `chooseShot()`.

### Game

Mo phong mot tran dau giua 2 Player.

### Simulation

Chay toan bo 16 matchup, gom thong ke, in report va xuat CSV.

### Statistics

Luu thong ke:

- So tran
- So tran thang
- So phat ban
- HIT
- MISS
- Accuracy
- Average shots/game
- Average turns to win
- Hit matrix 10x10

## 8. Class Diagram

```mermaid
classDiagram
    class Position {
        +int row
        +int col
        +isValid() bool
        +equals(Position) bool
    }

    class Ship {
        -string name
        -int size
        -vector~Position~ positions
        -int hitCount
        +addPosition(Position) void
        +occupies(Position) bool
        +receiveHit() void
        +isSunk() bool
    }

    class Board {
        -int shipIndex[10][10]
        -bool wasShot[10][10]
        -vector~Ship~ ships
        +reset() void
        +canPlaceShip(Position, int, bool) bool
        +placeShip(string, int, Position, bool) bool
        +receiveShot(Position) ShotResult
        +allShipsSunk() bool
    }

    class Strategy {
        <<abstract>>
        #bool fired[10][10]
        +chooseShot(mt19937) Position
        +processShotResult(Position, ShotResultType) void
        +clone() Strategy*
        +reset() void
    }

    class CenterBiasStrategy
    class CheckerboardStrategy
    class HuntTargetStrategy
    class HybridAdaptiveStrategy

    Strategy <|-- CenterBiasStrategy
    Strategy <|-- CheckerboardStrategy
    Strategy <|-- HuntTargetStrategy
    Strategy <|-- HybridAdaptiveStrategy

    class PlacementStrategy {
        <<abstract>>
        +placeShips(Board, mt19937) void
        +clone() PlacementStrategy*
    }

    class RandomPlacementStrategy
    PlacementStrategy <|-- RandomPlacementStrategy

    class Player {
        -Board board
        -Strategy* shootingStrategy
        -PlacementStrategy* placementStrategy
        +resetForNewGame(mt19937) void
        +shoot(Player, mt19937) ShotRecord
        +hasLost() bool
    }

    class Game {
        -Player player1
        -Player player2
        +play(mt19937, bool, long long[10][10]) GameResult
    }

    class Simulation {
        -vector~Strategy*~ strategies
        -vector~Statistics~ matchupStats
        +runMatchup(Strategy*, Strategy*, int) void
        +runAllMatchups() void
        +collectStatistics() void
        +printFinalReport() void
        +exportCSV() void
    }

    Board *-- Ship
    Ship *-- Position
    Player *-- Board
    Player o-- Strategy
    Player o-- PlacementStrategy
    Game *-- Player
    Simulation --> Game
    Simulation --> Statistics
```

## 9. Da Hinh Trong Project

`Strategy` la lop cha truu tuong. Bon chien thuat ban ke thua tu `Strategy` va override `chooseShot()`.

`Player` chi giu con tro:

```cpp
Strategy* shootingStrategy;
```

Khi goi:

```cpp
shootingStrategy->chooseShot(rng);
```

C++ se tu chon dung ham cua doi tuong that o runtime. Day la runtime polymorphism.

Tuong tu, `PlacementStrategy` cung la lop cha truu tuong. Hien tai co `RandomPlacementStrategy`, va co the mo rong them cac lop dat tau khac ma khong can sua `Player` hay `Game`.

## 10. Bao Cao Output

Chuong trinh in:

- Tong so strategy
- Tong so matchup
- So tran moi matchup
- Ket qua tung matchup
- Winrate bot1/bot2
- Average shots/game
- HIT, MISS
- Accuracy
- Average turns to win
- Bang xep hang strategy
- Hit matrix 10x10
- Vung co HIT thap nhat

## 11. Cach Doc File CSV

File `battleship_results.csv` gom 3 phan:

1. `MATCHUP STATISTICS`: thong ke tung cap dau.
2. `STRATEGY RANKING`: xep hang tong hop theo strategy.
3. `HIT MATRIX 10x10`: ma tran tan suat HIT cua tung o.

Co the mo file bang Excel, Google Sheets hoac LibreOffice Calc.

## 12. Ket Luan Chien Thuat

Theo logic simulation, chien thuat manh nhat thuong la:

```text
Hybrid Adaptive Strategy
```

Ly do:

- Trong giai doan tim tau, no dung checkerboard de giam so o can ban.
- No uu tien trung tam, noi tau co kha nang bi cham som hon trong nhieu cach dat random.
- Sau khi HIT, no chuyen sang Target mode de danh chim tau nhanh.

Chien thuat on dinh thu hai thuong la:

```text
Hunt & Target Strategy
```

Center Bias va Checkerboard don le yeu hon vi khong co Target mode.

## 13. Goi Y Dat Tau

Neu la nguoi dat tau, nen:

- Khong gom nhieu tau o trung tam.
- Uu tien rai tau ve bien va goc.
- Doi huong ngang/doc linh hoat.
- Khong dat tau qua sat nhau.
- Doc `HIT MATRIX 10x10` de tim cac vung co tan suat HIT thap.

Luu y: project hien tai moi co Random Placement. Co the nang cap them Edge Placement, Corner Placement hoac Anti-Hybrid Placement.

## 14. Han Che

- Chua co GUI.
- Chua co che do nguoi choi nhap tay.
- Placement moi chi co random.
- Chua dung probability heatmap nang cao.
- Hunt & Target duoc cai dat theo muc de hieu, chua phai AI toi uu tuyet doi.

## 15. Huong Nang Cap

Co the mo rong project bang:

- Them `EdgePlacementStrategy`.
- Them `CornerBiasPlacementStrategy`.
- Them `SpreadPlacementStrategy` de dat tau rai deu.
- Them che do nguoi choi vs bot.
- Them doc config tu file.
- Them nhieu lan benchmark de so sanh placement strategy.
- Them output JSON hoac chart CSV rieng cho hit matrix.
- Them xac suat don gian dua tren kich thuoc tau con lai.

## 16. Tom Tat Cho Bao Cao

Project the hien ro cac tinh chat OOP:

- Dong goi: `Ship`, `Board`, `Player` che giau du lieu noi bo.
- Ke thua: cac strategy ke thua `Strategy`, placement ke thua `PlacementStrategy`.
- Da hinh: `Player` goi ham ao qua `Strategy*` va `PlacementStrategy*`.
- Mo rong tot: muon them chien thuat moi chi can tao class moi ke thua interface san co.

Ket luan tong quat:

- Chien thuat ban toi uu: Hybrid Adaptive Strategy.
- Chien thuat on dinh: Hybrid Adaptive va Hunt & Target.
- Chien thuat dat tau nen uu tien: tranh trung tam, rai tau ve bien/goc, dua tren HIT MATRIX de tranh vung nguy hiem.
