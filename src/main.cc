#include <console.hh>
#include <random>
#include <list>
#include <queue>

struct block {
    std::size_t x, y;
};

struct fruit {
    block coord;
    void draw(std::vector<console::Pixel> & pixels, std::size_t X) {
        console::grid::at_2D(pixels, coord.x, coord.y, X) = console::Pixel(console::col::FG::RED, console::col::BG::BLACK, '&');
    }
};

fruit current_fruit;

std::mt19937 mersenne;

enum class DIRECTION : std::uint8_t {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE,
};

std::queue<DIRECTION> directions;

struct snake {
    bool doesnt_want_to_quit = true;

    block last_known;
    std::size_t score = 0;
    std::list<block> segments;
    bool dead = false;
    DIRECTION current = DIRECTION::NONE;

    void update_direction(DIRECTION direction) {
        if(!dead)
            switch(direction) {
                case DIRECTION::UP:
                    if(current != DIRECTION::DOWN ) current = DIRECTION::UP;
                break;
                case DIRECTION::DOWN:
                    if(current != DIRECTION::UP   ) current = DIRECTION::DOWN;
                break;
                case DIRECTION::LEFT:
                    if(current != DIRECTION::RIGHT) current = DIRECTION::LEFT;
                break;
                case DIRECTION::RIGHT:
                    if(current != DIRECTION::LEFT ) current = DIRECTION::RIGHT;
                break;
                default: break;
            }
    }

    void draw(std::vector<console::Pixel> & pixels, std::size_t X) {
        for(const auto & segment : segments)
            console::grid::at_2D(pixels, segment.x, segment.y, X) = console::Pixel(
                console::col::FG::WHITE,
                console::col::BG::BLACK,
                '%'
            );
    }

    void move(std::size_t X, std::size_t Y) {
        last_known.x = X;
        last_known.y = Y;

        if(current == DIRECTION::NONE)
            return;

        block b = *segments.rbegin();
        segments.push_back(b);

        switch(current) {
            case DIRECTION::UP   : --segments.rbegin()->y; break;
            case DIRECTION::DOWN : ++segments.rbegin()->y; break;
            case DIRECTION::LEFT : --segments.rbegin()->x; break;
            case DIRECTION::RIGHT: ++segments.rbegin()->x; break;
            default: break;
        }

        if(!(0 < segments.rbegin()->x && segments.rbegin()->x < X - 1 && 0 < segments.rbegin()->y && segments.rbegin()->y < Y - 1)) {
            dead = true;
            current = DIRECTION::NONE;
        }

        if(segments.rbegin()->x == current_fruit.coord.x && segments.rbegin()->y == current_fruit.coord.y) {
            current_fruit.coord.x = mersenne() % (X - 1);
            current_fruit.coord.y = mersenne() % (Y - 1);

            if(current_fruit.coord.x == 0) current_fruit.coord.x = 1;
            if(current_fruit.coord.y == 0) current_fruit.coord.y = 1;

            score += 500;
        } else {
            segments.erase(segments.begin());
        }

        for(auto it = ++segments.rbegin(); it != segments.rend(); ++it) {
            if(it->x == segments.rbegin()->x && it->y == segments.rbegin()->y) {
                dead = true;
                current = DIRECTION::NONE;
            }
        }
    }
};

snake player;

void keys(char key) {
    if(key == 'q') {
        player.doesnt_want_to_quit = false;
    } else if(!player.dead) {
        switch(key) {
            case 'w': directions.push(DIRECTION::UP);    break;
            case 'a': directions.push(DIRECTION::LEFT);  break;
            case 's': directions.push(DIRECTION::DOWN);  break;
            case 'd': directions.push(DIRECTION::RIGHT); break;
        }
    } else {
        if(key == 'r') {
            player.segments.clear();
            player.segments.push_back(block{player.last_known.x / 2, player.last_known.y / 2});

            player.score = 0;
            player.dead = false;

            current_fruit.coord.x = mersenne() % (player.last_known.x - 1);
            current_fruit.coord.y = mersenne() % (player.last_known.y - 1);

            if(current_fruit.coord.x == 0) current_fruit.coord.x = 1;
            if(current_fruit.coord.y == 0) current_fruit.coord.y = 1;
        }
    }
}

bool init(std::vector<console::Pixel> &, std::size_t X, std::size_t Y) {
    console::toggle_title();

    mersenne = std::mt19937{std::random_device{}()};

    player.segments.push_back(block{X / 2, Y / 2});

    current_fruit.coord.x = mersenne() % (X - 1);
    current_fruit.coord.y = mersenne() % (Y - 1);

    if(current_fruit.coord.x == 0) current_fruit.coord.x = 1;
    if(current_fruit.coord.y == 0) current_fruit.coord.y = 1;

    return true;
}

void draw_border(std::vector<console::Pixel> & pixels, std::size_t X, std::size_t Y) {
    for(std::size_t x = 0; x < X; ++x) {
        console::grid::at_2D(pixels, x, 0, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '-');
        console::grid::at_2D(pixels, x, Y - 1, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '-');
    }

    for(std::size_t y = 0; y < Y; ++y) {
        console::grid::at_2D(pixels, 0, y, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '|');
        console::grid::at_2D(pixels, X - 1, y, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '|');
    }

    console::grid::at_2D(pixels, 0    , 0    , X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '#');
    console::grid::at_2D(pixels, X - 1, 0    , X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '#');
    console::grid::at_2D(pixels, 0    , Y - 1, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '#');
    console::grid::at_2D(pixels, X - 1, Y - 1, X) = console::Pixel(console::col::FG::WHITE, console::col::BG::BLACK, '#');
}

void draw_score(std::vector<console::Pixel> & pixels) {
    console::grid::set_string(
        pixels,
        std::string_view{" SCORE: " + std::to_string(player.score) + " - Press 'q' to quit "},
        console::col::FG::WHITE,
        console::col::BG::BLACK,
        console::col::INVERT::NO,
        console::col::BOLD::NO,
        console::col::ITALIC::NO,
        console::col::UNDERLINE::NO,
        console::col::STRIKETHROUGH::NO,
        2
    );
}

void draw_death_screen(std::vector<console::Pixel> & pixels, std::size_t X, std::size_t Y) {
    std::string_view line1 = "YOU ARE DEAD";

    console::grid::set_string(
        pixels,
        line1,
        console::col::FG::RED,
        console::col::BG::BLACK,
        console::col::INVERT::NO,
        console::col::BOLD::NO,
        console::col::ITALIC::NO,
        console::col::UNDERLINE::NO,
        console::col::STRIKETHROUGH::NO,
        X / 2 - line1.length() / 2,
        Y / 2,
        X
    );

    std::string_view line2 = "Press 'r' to restart.";

    console::grid::set_string(
        pixels,
        line2,
        console::col::FG::RED,
        console::col::BG::BLACK,
        console::col::INVERT::NO,
        console::col::BOLD::NO,
        console::col::ITALIC::NO,
        console::col::UNDERLINE::NO,
        console::col::STRIKETHROUGH::NO,
        X / 2 - line2.length() / 2,
        Y / 2 + 1,
        X
    );
}

constexpr float tickrate = 1.0f / 15.0f;
float accumulator = 0.0;

bool update(std::vector<console::Pixel> & pixels, std::size_t X, std::size_t Y, float deltaTime) {
    console::grid::for_each_0(pixels, X, Y, [](console::Pixel & pixel) {
        pixel = console::Pixel();
    });

    draw_border(pixels, X, Y);
    current_fruit.draw(pixels, X);
    player.draw(pixels, X);

    if(player.dead) draw_death_screen(pixels, X, Y);

    draw_score(pixels);

    accumulator += deltaTime;

    while(accumulator >= tickrate) {
        if(!directions.empty()) {
            player.update_direction(directions.front());
            directions.pop();
        }

        player.move(X, Y);
        accumulator -= tickrate;
    }

    return player.doesnt_want_to_quit;
}

int main() {
    if(!console::init())
        return 0;

    console::set_init_callback(init);
    console::set_key_callback(keys);
    console::set_update_callback(update);

    console::run();

    return console::exit();
}
