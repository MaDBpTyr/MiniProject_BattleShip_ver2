# Mini Project C++ OOP: BattleShip Strategy Benchmark

## 1. Giới Thiệu Project

Đây là mini project C++ console mô phỏng trò chơi BattleShip để phân tích chiến thuật bắn tàu và chiến thuật đặt tàu bằng lập trình hướng đối tượng OOP.

Project tập trung trả lời 2 câu hỏi chính:

1. Nếu là người bắn tàu, chiến thuật nào tối ưu nhất?
2. Nếu là người đặt tàu, nên đặt tàu như thế nào để đối thủ khó bắn trúng?

## 2. Luật Game BattleShip

- Bàn chơi có kích thước 10x10.
- Mỗi người chơi có 5 tàu:
  - Destroyer: 2 ô
  - Submarine: 3 ô
  - Cruiser: 3 ô
  - Battleship: 4 ô
  - Carrier: 5 ô
- Tàu được đặt ngẫu nhiên vào các vị trí hợp lệ.
- Tàu có thể nằm ngang hoặc dọc.
- Tàu không được chồng lên nhau.
- Mỗi lượt bắn vào một tọa độ.
- Nếu bắn trúng tàu thì kết quả là HIT.
- Nếu bắn trúng thì người chơi được bắn tiếp.
- Nếu bắn trượt thì kết quả là MISS.
- Nếu bắn trượt thì người chơi mất lượt.
- Nếu toàn bộ ô của một tàu bị bắn trúng thì kết quả là SUNK.
- Người chơi thắng khi bắn chìm toàn bộ 5 tàu của đối thủ.

## 3. Mục Tiêu Mô Phỏng

Project chạy mô phỏng tự động nhiều trận giữa các bot để thu thập số liệu thống kê. Mục tiêu không phải là hiển thị từng lượt chơi, mà là dùng dữ liệu để so sánh hiệu quả chiến thuật.

Các số liệu được thu thập gồm:

- Tổng số trận.
- Số trận thắng của từng bot.
- Tỉ lệ thắng.
- Tổng số phát bắn.
- Số HIT.
- Số MISS.
- Độ chính xác.
- Số lượt bắn trung bình để thắng.
- Ma trận HIT 10x10.

## 4. Các Chiến Thuật Bắn

### 4.1 Center Bias Strategy

Bot ưu tiên bắn các ô gần trung tâm bàn cờ trước. Các ô trung tâm được bắn theo thứ tự gần tâm nhất. Khi vùng trung tâm đã bị bắn hết, bot chuyển sang bắn random các ô còn lại.

Ưu điểm:

- Ý tưởng đơn giản, dễ hiểu.
- Hợp lý nếu giả định tàu có khả năng xuất hiện nhiều ở vùng trung tâm.

Nhược điểm:

- Không có Target mode.
- Sau khi bắn HIT, bot không biết truy đuổi để đánh chìm tàu nhanh.

### 4.2 Checkerboard Strategy

Bot ưu tiên bắn theo mẫu caro. Các ô được ưu tiên là những ô thỏa điều kiện:

```cpp
(row + col) % 2 == 0
```

Khi các ô caro đã bị bắn hết, bot chuyển sang bắn random các ô còn lại.

Ưu điểm:

- Giảm số ô cần dò trong giai đoạn đầu.
- Phù hợp với BattleShip vì tàu nhỏ nhất có kích thước 2, nên bắn theo caro vẫn có khả năng chạm tàu.

Nhược điểm:

- Không có Target mode.
- Sau khi bắn HIT, bot vẫn không truy đuổi tàu.

### 4.3 Hunt & Target Strategy

Chiến thuật này có 2 trạng thái:

- Hunt mode: bot bắn random có kiểm soát để tìm tàu.
- Target mode: khi bắn HIT, bot ưu tiên bắn các ô xung quanh vị trí HIT.

Nếu tiếp tục HIT, bot cố gắng xác định hướng của tàu rồi bắn tiếp theo hướng đó. Nếu hướng hiện tại không hợp lệ hoặc bắn MISS, bot thử hướng ngược lại hoặc quay về Hunt mode khi không còn mục tiêu hợp lệ.

Ưu điểm:

- Sau khi tìm thấy tàu, bot có khả năng đánh chìm nhanh hơn.
- Mạnh hơn các chiến thuật chỉ bắn theo mẫu đơn giản.

Nhược điểm:

- Trong Hunt mode, bot vẫn dùng random nên chưa tối ưu hoàn toàn vùng tìm kiếm.

### 4.4 Hybrid Adaptive Strategy

Đây là chiến thuật kết hợp:

- Center Bias
- Checkerboard
- Hunt & Target

Trong Hunt mode, bot ưu tiên các ô caro ở gần trung tâm. Khi bắn HIT, bot chuyển sang Target mode giống Hunt & Target để truy đuổi và đánh chìm tàu.

Ưu điểm:

- Tìm tàu hiệu quả hơn nhờ checkerboard.
- Ưu tiên trung tâm để tăng khả năng phát hiện tàu sớm.
- Có Target mode để đánh chìm tàu nhanh sau khi HIT.

Nhược điểm:

- Code phức tạp hơn các chiến thuật còn lại.
- Cần lưu trạng thái nhiều hơn.

## 5. Chiến Thuật Đặt Tàu

Project hiện tại cài đặt chiến thuật:

### Random Placement Strategy

Tàu được đặt ngẫu nhiên vào vị trí hợp lệ, không chồng lên nhau. Mỗi tàu có thể nằm ngang hoặc dọc.

Chiến thuật này giúp tạo dữ liệu mô phỏng khách quan, vì vị trí tàu thay đổi liên tục qua từng trận.

## 6. Thiết Kế OOP

### 6.1 Position

Lưu tọa độ trên bàn cờ gồm `row` và `col`.

Nhiệm vụ chính:

- Kiểm tra tọa độ có hợp lệ không.
- So sánh hai vị trí có giống nhau không.

### 6.2 Ship

Quản lý thông tin của một tàu.

Thuộc tính chính:

- Tên tàu.
- Kích thước tàu.
- Danh sách vị trí tàu chiếm.
- Số lần bị bắn trúng.

Nhiệm vụ chính:

- Kiểm tra tàu có chiếm một vị trí hay không.
- Nhận HIT.
- Kiểm tra tàu đã chìm chưa.

### 6.3 Board

Quản lý bàn chơi 10x10.

Nhiệm vụ chính:

- Reset bàn chơi.
- Kiểm tra vị trí đặt tàu hợp lệ.
- Đặt tàu.
- Nhận phát bắn từ đối thủ.
- Trả về kết quả MISS, HIT hoặc SUNK.
- Kiểm tra tất cả tàu đã chìm chưa.

### 6.4 Strategy

`Strategy` là lớp cha trừu tượng cho các chiến thuật bắn.

Các chiến thuật cụ thể kế thừa từ `Strategy` và override các hàm ảo như:

```cpp
virtual Position chooseShot(mt19937& rng) = 0;
virtual void processShotResult(Position p, ShotResultType result) = 0;
virtual Strategy* clone() const = 0;
```

### 6.5 PlacementStrategy

`PlacementStrategy` là lớp cha trừu tượng cho các chiến thuật đặt tàu.

Hàm quan trọng:

```cpp
virtual void placeShips(Board& board, mt19937& rng) = 0;
```

Nhờ lớp này nên sau này nếu muốn mở rộng thêm nhiều cách đặt tàu khác thì dễ dàng hơn vì không cần sửa logic của `Player` hoặc `Game`.

### 6.6 Player

Mỗi `Player` có:

- Một `Board` riêng.
- Một con trỏ `Strategy*` để bắn.
- Một con trỏ `PlacementStrategy*` để đặt tàu.

`Player` không cần biết chiến thuật cụ thể là gì. Player chỉ gọi hàm ảo thông qua con trỏ lớp cha.

### 6.7 Game

`Game` mô phỏng một trận đấu giữa 2 player.

Nhiệm vụ chính:

- Reset game mới.
- Cho hai bot lần lượt bắn.
- Ghi nhận HIT, MISS, SUNK.
- Xác định bot thắng.
- Trả về kết quả trận đấu.

### 6.8 Simulation

`Simulation` chạy toàn bộ benchmark.

Benchmark là quá trình kiểm tra và so sánh hiệu quả bằng số liệu. Trong project này, benchmark có nghĩa là:

- Cho các bot đấu nhau hàng loạt.
- Thu thập số liệu sau nhiều trận.
- So sánh xem chiến thuật nào mạnh hơn, ổn định hơn.

Nhiệm vụ chính của `Simulation`:

- Tạo danh sách 4 chiến thuật bắn.
- Chạy toàn bộ 16 matchup.
- Gom thống kê từng matchup.
- Xếp hạng chiến thuật.
- In báo cáo cuối.
- Xuất file CSV.

### 6.9 Statistics

`Statistics` lưu số liệu thống kê.

Các thông tin chính:

- Số trận.
- Số trận thắng.
- Tổng số phát bắn.
- Tổng HIT.
- Tổng MISS.
- Accuracy.
- Average shots/game.
- Average turns to win.
- Hit matrix 10x10.

## 7. Class Diagram

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

## 8. OOP Trong Project

### 8.1 Tính Đóng Gói

Tính đóng gói thể hiện ở các class như `Ship`, `Board`, `Player`.

Ví dụ:

- `Ship` che giấu `name`, `size`, `positions`, `hitCount`.
- `Board` che giấu mảng `shipIndex`, `wasShot` và danh sách `ships`.
- `Player` che giấu board và chiến thuật bên trong.

Bên ngoài chỉ có thể thao tác thông qua các hàm public.

### 8.2 Tính Kế Thừa

Tính kế thừa thể hiện ở:

- `CenterBiasStrategy` kế thừa `Strategy`.
- `CheckerboardStrategy` kế thừa `Strategy`.
- `HuntTargetStrategy` kế thừa `Strategy`.
- `HybridAdaptiveStrategy` kế thừa `Strategy`.
- `RandomPlacementStrategy` kế thừa `PlacementStrategy`.

### 8.3 Tính Đa Hình

Tính đa hình thể hiện qua việc `Player` giữ con trỏ lớp cha:

```cpp
Strategy* shootingStrategy;
PlacementStrategy* placementStrategy;
```

Khi gọi:

```cpp
shootingStrategy->chooseShot(rng);
```

C++ sẽ tự động gọi đúng hàm `chooseShot()` của object thật ở runtime. Đây là runtime polymorphism.

## 9. Ma Trận HIT 10x10

Ma trận HIT lưu số lần mỗi ô trên bàn cờ bị bắn trúng sau toàn bộ simulation.

Ma trận này dùng để:

- Xem ô nào thường bị bắn trúng nhiều nhất.
- Phân tích vùng nguy hiểm trên bàn cờ.
- So sánh hiệu quả của chiến thuật đặt tàu.
- Tìm vùng ít nguy hiểm để đặt tàu.

Cách đọc:

- Số càng lớn: ô đó càng hay bị HIT.
- Số càng nhỏ: ô đó ít bị HIT hơn.
- Vùng có nhiều số lớn là vùng nguy hiểm.
- Vùng có nhiều số nhỏ là vùng nên cân nhắc đặt tàu.

Dựa trên ma trận HIT, người chơi có thể rút ra gợi ý đặt tàu:

- Tránh đặt quá nhiều tàu ở vùng trung tâm nếu đối thủ dùng Center Bias hoặc Hybrid Adaptive.
- Ưu tiên rải tàu về biên và góc nếu ma trận HIT cho thấy các vùng này ít bị bắn trúng hơn.
- Không đặt các tàu quá sát nhau để tránh bị đối thủ truy đuổi liên tục sau khi HIT.
- Thay đổi hướng ngang/dọc để giảm khả năng bị đoán hướng.

## 10. File CSV Kết Quả

File `battleship_results.csv` gồm 3 phần chính:

1. `MATCHUP STATISTICS`: thống kê từng cặp chiến thuật.
2. `STRATEGY RANKING`: bảng xếp hạng tổng hợp theo chiến thuật.
3. `HIT MATRIX 10x10`: ma trận tần suất HIT của từng ô.

## 11. Kết Luận Chiến Thuật

Theo logic mô phỏng, chiến thuật bắn tối ưu nhất thường là:

```text
Hybrid Adaptive Strategy
```

Lý do:

- Dùng checkerboard để giảm số ô cần dò.
- Ưu tiên các ô gần trung tâm trong giai đoạn Hunt.
- Sau khi HIT, chuyển sang Target mode để đánh chìm tàu nhanh.
- Kết hợp được ưu điểm của nhiều chiến thuật.

Chiến thuật ổn định thứ hai thường là:

```text
Hunt & Target Strategy
```

Hai chiến thuật `Center Bias` và `Checkerboard` yếu hơn vì không có khả năng truy đuổi tàu sau khi HIT.

## 12. Gợi Ý Cho Người Chơi

Nếu là người bắn tàu:

- Nên dùng tư duy Hybrid Adaptive.
- Giai đoạn đầu bắn theo caro để tìm tàu.
- Ưu tiên vùng trung tâm trước, sau đó mở rộng ra ngoài.
- Khi bắn HIT, phải chuyển sang truy đuổi ngay.
- Nếu xác định được hướng tàu, tiếp tục bắn theo hướng đó đến khi tàu chìm.

Nếu là người đặt tàu:

- Không nên gom nhiều tàu ở trung tâm.
- Nên rải tàu về biên và góc nếu ma trận HIT cho thấy những vùng đó ít nguy hiểm hơn.
- Không đặt các tàu quá sát nhau.
- Thay đổi hướng ngang và dọc linh hoạt.
- Dựa vào ma trận HIT để tránh các vùng hay bị bắn trúng.
