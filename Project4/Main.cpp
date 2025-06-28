#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <ctime>
#include <cstdlib>

const int WIDTH = 10;   //����

const int HEIGHT = 20;  //�c��

const int CELL_SIZE = 30; //1�}�X�T�C�Y(�s�N�Z����)

const int WINDOW_WIDTH = WIDTH * CELL_SIZE; //�E�B���h�E�̕�

const int WINDOW_HEIGHT = HEIGHT * CELL_SIZE;   //�E�B���h�E�̍���


// �e�g���~�m�i7��j�̒�`�F7��4�~4�s��
const std::array<std::array<std::array<int, 4>, 4>, 7> tetrominoes = { {

        // I�^
        {{ {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} }},

        // O�^
        {{ {0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0} }},

        // T�^
        {{ {0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0} }},

        // S�^
        {{ {0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0} }},

        // Z�^
        {{ {0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0} }},

        // J�^
        {{ {0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0} }},

        // L�^
        {{ {0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0} }}
    } };


//�~�m�̐F���Ǘ��@
// �����ł�vector�́A�ϒ��z���\��
std::vector<sf::Color> colors = {

    sf::Color::Cyan, 
    sf::Color::Yellow, 
    sf::Color::Magenta,
    sf::Color::Green, 
    sf::Color::Red, 
    sf::Color::Blue, 
    sf::Color(255, 140, 0)

};

// �~�m�̍\���́i�`�E�F�E���Wx, y�j
//�~�m�̒��g
struct Tetromino {

    std::array<std::array<int, 4>, 4> shape; //4*4�̃~�m

    sf::Color color; //�F

    int x = 3, y = 0; //�����Ă��鎞�̍ŏ��̍��W
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

    //90�x��]�̎���
    void rotate() 
    {
        Tetromino rotated = current;

        //4 * 4�̔z��̉�]������
        for (int y = 0; y < 4; y++) 
        {
            for (int x = 0; x < 4; x++)
            {
                rotated.shape[y][x] = current.shape[3 - x][y]; //�z��̒���x�̈ʒu�𔽓]������A3 - x ���g�����ƂŁu0��3�A1��2�A2��1�A3��0�v�Ƃ����悤�ɔ��]���s��
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
        // �t�B�[���h�`��
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (field[y][x] == sf::Color::Black) continue;
                drawCell(window, x, y, field[y][x]);
            }
        }
        // ���݂̃~�m�`��
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

    //�w�肵���ʒu�ɁA�~�m��z�u�ł��邩�̔��f
    //t = �m�F�������~�m nx, ny = �u���ʒu�̍���

    bool isValidPosition(const Tetromino& t, int nx, int ny) 
    {
        for (int y = 0; y < 4; y++) 
        {
            for (int x = 0; x < 4; x++) 
            {
                if (!t.shape[y][x]) continue;

                int fx = nx + x, fy = ny + y; //nx,ny�ō���Ax,y�ł��̑��Έʒu

                if (fx < 0 || fx >= WIDTH || fy < 0 || fy >= HEIGHT) return false; //�͈͊O���폜

                if (field[fy][fx] != sf::Color::Black) return false; //���łɃu���b�N��u����Ă���ƁA�d�Ȃ邽��false
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
                y++; // �������炻�̍s���ă`�F�b�N
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
