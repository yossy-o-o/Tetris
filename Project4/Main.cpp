#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <ctime>
#include <cstdlib>

const int WIDTH = 10;   //横幅

const int HEIGHT = 20;  //縦幅

const int CELL_SIZE = 30; //1マスサイズ(ピクセル数)

const int WINDOW_WIDTH = WIDTH * CELL_SIZE; //ウィンドウの幅

const int WINDOW_HEIGHT = HEIGHT * CELL_SIZE;   //ウィンドウの高さ


// テトリミノ（7種）の定義：7個の4×4行列
const std::array<std::array<std::array<int, 4>, 4>, 7> tetrominoes = { {

        // I型
        {{ {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} }},

        // O型
        {{ {0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0} }},

        // T型
        {{ {0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0} }},

        // S型
        {{ {0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0} }},

        // Z型
        {{ {0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0} }},

        // J型
        {{ {0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0} }},

        // L型
        {{ {0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0} }}
    } };


//ミノの色を管理　
// ここでのvectorは、可変長配列を表す
std::vector<sf::Color> colors = {

    sf::Color::Cyan, 
    sf::Color::Yellow, 
    sf::Color::Magenta,
    sf::Color::Green, 
    sf::Color::Red, 
    sf::Color::Blue, 
    sf::Color(255, 140, 0)

};

// ミノの構造体（形・色・座標x, y）
//ミノの中身
struct Tetromino {

    std::array<std::array<int, 4>, 4> shape; //4*4のミノ

    sf::Color color; //色

    int x = 3, y = 0; //落ちてくる時の最初の座標
};

class Game 
{

public:
    Game() 
    {
        field.fill({});
        spawnTetromino();
    }

    void update() 
    {
        if (!move(0, 1)) {
            merge();
            clearLines();
            spawnTetromino();
            if (!isValidPosition(current, current.x, current.y))
                gameOver = true;
        }
    }

    //90度回転の実装
    void rotate() 
    {
        Tetromino rotated = current;

        //4 * 4の配列の回転を実装
        for (int y = 0; y < 4; y++) 
        {
            for (int x = 0; x < 4; x++)
            {
                rotated.shape[y][x] = current.shape[3 - x][y]; //配列の中でxの位置を反転させる、3 - x を使うことで「0→3、1→2、2→1、3→0」というように反転を行う
            }
        } 

        if (isValidPosition(rotated, current.x, current.y))
        {
            current = rotated;
        }
    }

    bool move(int dx, int dy) 
    {
        if (isValidPosition(current, current.x + dx, current.y + dy)) {
            current.x += dx;
            current.y += dy;
            return true;
        }
        return false;
    }

    void hardDrop() 
    {
        while (move(0, 1));
        update();
    }

    void draw(sf::RenderWindow& window) 
    {
        window.clear();
        // フィールド描画
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (field[y][x] == sf::Color::Black) continue;
                drawCell(window, x, y, field[y][x]);
            }
        }
        // 現在のミノ描画
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (current.shape[y][x]) {
                    drawCell(window, current.x + x, current.y + y, current.color);
                }
            }
        }
        window.display();
    }

    bool isGameOver() const { return gameOver; }

private:

    std::array<std::array<sf::Color, WIDTH>, HEIGHT> field;
    Tetromino current;
    bool gameOver = false;

    void spawnTetromino() 
    {
        int id = rand() % 7;
        current.shape = tetrominoes[id];
        current.color = colors[id];
        current.x = 3;
        current.y = 0;
    }

    //指定した位置に、ミノを配置できるかの判断
    //t = 確認したいミノ nx, ny = 置く位置の左上

    bool isValidPosition(const Tetromino& t, int nx, int ny) 
    {
        for (int y = 0; y < 4; y++) 
        {
            for (int x = 0; x < 4; x++) 
            {
                if (!t.shape[y][x]) continue;

                int fx = nx + x, fy = ny + y; //nx,nyで左上、x,yでその相対位置

                if (fx < 0 || fx >= WIDTH || fy < 0 || fy >= HEIGHT) return false; //範囲外を削除

                if (field[fy][fx] != sf::Color::Black) return false; //すでにブロックを置かれていると、重なるためfalse
            }
        }
        return true;
    }

    void merge() 
    {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (current.shape[y][x]) {
                    field[current.y + y][current.x + x] = current.color;
                }
            }
        }
    }

    void clearLines() 
    {
        for (int y = HEIGHT - 1; y >= 0; y--) {
            bool full = true;
            for (int x = 0; x < WIDTH; x++) {
                if (field[y][x] == sf::Color::Black) {
                    full = false;
                    break;
                }
            }
            if (full) {
                for (int ty = y; ty > 0; ty--)
                    field[ty] = field[ty - 1];
                field[0].fill(sf::Color::Black);
                y++; // 消したらその行を再チェック
            }
        }
    }

    void drawCell(sf::RenderWindow& window, int x, int y, sf::Color color) 
    {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
        cell.setPosition(x * CELL_SIZE + 1, y * CELL_SIZE + 1);
        cell.setFillColor(color);
        window.draw(cell);
    }
};

int main() 
{
    srand(static_cast<unsigned>(time(nullptr)));
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetris");
    Game game;
    sf::Clock clock;
    float timer = 0.0f, delay = 0.5f;

    while (window.isOpen()) 
    {
        float time = clock.restart().asSeconds();
        timer += time;

        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) 
            {
                if (event.key.code == sf::Keyboard::Up) game.rotate();
                else if (event.key.code == sf::Keyboard::Left) game.move(-1, 0);
                else if (event.key.code == sf::Keyboard::Right) game.move(1, 0);
                else if (event.key.code == sf::Keyboard::Down) game.move(0, 1);
                else if (event.key.code == sf::Keyboard::Space) game.hardDrop();
            }
        }

        if (timer > delay) 
        {
            game.update();
            timer = 0;
        }

        game.draw(window);

        if (game.isGameOver()) 
        {
            window.setTitle("Game Over");
        }
    }

    return 0;
}
