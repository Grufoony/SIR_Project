#include "board.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <random>
#include <stdexcept>
#include <thread>

sir::Board::Board(std::string c, int n, double b, double y, int inf,
    Mode adv_opt, double q_prob, Quarantine_parameters quarantine)
    : grid_(n + 2, std::vector<Cell>(n + 2)) {
    dimension_ = n + 2;
    q_prob_ = q_prob;
    beta_ = b;
    gamma_ = y;
    day_ = 0;
    name_ = c;
    advanced_opt_ = adv_opt;
    if (n < 149 || n > 601) {
        throw std::runtime_error{
            "Error: the dimension is too tiny to see any simulation effect."
            "The minimum dimension is 150.The maximum is 600" };
    }
    if ((int)advanced_opt_ > 3)
        assert(quarantine.first_day != 0 && quarantine.last_day != 0);
    if ((int)advanced_opt_ == 3 || (int)advanced_opt_ == 5)
        assert(static_cast<int>(q_prob_ * 1000) != 0);
    assert(b > 0 && b < 1);
    assert(y > 0 && y < 1);
    assert(q_prob >= 0 && q_prob <= 1);
    assert(inf > 0);
    for (int i = 0; i < inf; ++i) {
        int ran1 = (std::rand() + time(nullptr)) % dimension_;
        int ran2 = (std::rand() + time(nullptr)) % dimension_;
        grid_[ran1][ran2].state = Sir::i;
    }
    quarantin_.len_line = dimension_ / 2;
    quarantin_.first_day = quarantine.first_day;
    quarantin_.last_day = quarantine.last_day;
    counter_.num_i = inf;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sir& sir::Board::operator()(int riga, int column) {
    return (grid_[riga + 1][column + 1].state);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Evolution of the Grid
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::evolve_() {
    counter_.num_i = 0;
    counter_.num_s = 0;
    counter_.num_r = 0;
    counter_.num_q = 0;
    std::vector<std::vector<Sir>> temp(dimension_ - 2,
        std::vector<Sir>(dimension_ - 2));
    int theoretical_ill = static_cast<int>(dimension_ * beta_);
    int ill_found = static_cast<int>(theoretical_ill * q_prob_);
    int n;
    for (int l = 1; l < dimension_ - 1; ++l) {
        for (int c = 1; c < dimension_ - 1; ++c) {
            switch (grid_[l][c].state) {
            case Sir::s:
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        if (grid_[l + j][c + i].state == Sir::i &&
                            grid_[l + j][c + i].state != Sir::s) {
                            grid_[l][c].inf_prob += beta_;
                        }
                        else {
                            grid_[l][c].inf_prob += 0;
                        }
                    }
                }
                if (grid_[l][c].inf_prob >= 1) {
                    ++temp[l - 1][c - 1];
                    grid_[l][c].inf_prob = 0;
                }
                break;
            case Sir::i:
                n = gen_unif_rand_number(theoretical_ill);
                if (n < ill_found && day_ > 10 &&
                    ((int)advanced_opt_ == 3 || (int)advanced_opt_ == 5)) {
                    temp[l - 1][c - 1] = Sir::q;
                }
                else {
                    grid_[l][c].clock += 1;
                    if (n < theoretical_ill * gamma_ && day_>14) {
                        ++(++temp[l - 1][c - 1]);
                        grid_[l][c].clock = 0;
                    }
                    else {
                        ++temp[l - 1][c - 1];
                    }
                }
                break;
            case Sir::r:
                ++(++temp[l - 1][c - 1]);
                break;
            case Sir::q:

                if (grid_[l][c].clock > 40) {
                    temp[l - 1][c - 1] = Sir::r;
                    grid_[l][c].clock = 0;
                }
                else {
                    ++grid_[l][c].clock;
                    temp[l - 1][c - 1] = Sir::q;
                }
                break;
            case Sir::q_edge:
                if (day_ < quarantin_.last_day) {
                    temp[l - 1][c - 1] = Sir::q_edge;
                }
                break;
            default:
                throw std::runtime_error("ERROR: undefined Sir state of a cell.\n");
            }
        }
        counter_.num_s +=
            std::count(temp[l - 1].begin(), temp[l - 1].end(), Sir::s);
        counter_.num_i +=
            std::count(temp[l - 1].begin(), temp[l - 1].end(), Sir::i);
        counter_.num_r +=
            std::count(temp[l - 1].begin(), temp[l - 1].end(), Sir::r);
        counter_.num_q +=
            std::count(temp[l - 1].begin(), temp[l - 1].end(), Sir::q);
    }
    counter_quarantine();
    ++day_;
    copy_(temp);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int sir::Board::gen_unif_rand_number(int num) const {
    std::random_device dev{};
    std::mt19937 gen{ dev() };
   // gen.seed(rand() + time(nullptr));
    std::uniform_int_distribution<int> uniform(0, num);
    return uniform(gen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::copy_(std::vector<std::vector<Sir>>& copy) {
    for (int l = 0; l <= dimension_ - 1; ++l) {
        for (int c = 0; c <= dimension_ - 1; ++c) {
            if (l == 0 || c == 0 || l == dimension_ - 1 || c == dimension_ - 1) {
                grid_[l][c].state = Sir::s;
            }
            else {
                grid_[l][c].state = copy[l - 1][c - 1];
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::counter_quarantine() {
    q_counter_.num_s = 0;
    q_counter_.num_i = 0;
    q_counter_.num_r = 0;
    for (int line = quarantin_.len_line; line < quarantin_.len_line * 2; ++line) {
        for (int column = quarantin_.len_line; column < quarantin_.len_line * 2;
            ++column) {
            if (grid_[line][column].state == Sir::s) {
                ++q_counter_.num_s;
            }
            else if (grid_[line][column].state == Sir::r) {
                ++q_counter_.num_r;
            }
            else {
                ++q_counter_.num_i;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This member function activates the quarantene of Grid's quarter.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::quarantine_() {
    for (int l = 1; l < dimension_; ++l) {
        for (int c = 1; c < dimension_; ++c) {
            if ((((l == quarantin_.len_line || l == quarantin_.len_line * 2) ||
                (c == quarantin_.len_line * 2 || c == quarantin_.len_line)) &&
                (l >= quarantin_.len_line && l <= quarantin_.len_line * 2) &&
                (c >= quarantin_.len_line && c <= quarantin_.len_line * 2)))
                grid_[l][c].state = Sir::q_edge;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Short range movement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::move_() {
    for (int l = 2; l < dimension_ - 2; ++l) {
        for (int c = 2; c < dimension_ - 2; ++c) {
            if (grid_[l][c].state != Sir::q && grid_[l][c].state != Sir::q_edge) {
                int swap;
                int newc = 0;
                int newl = 0;
                swap = (rand() + time(nullptr)) % 13;
                switch (swap) {
                case 0:
                    newc = c - 1;
                    newl = l - 1;
                    break;
                case 1:
                    newc = c;
                    newl = l - 1;
                    break;
                case 2:
                    newc = c + 1;
                    newl = l - 1;
                    break;
                case 3:
                    newc = c - 1;
                    newl = l;
                    break;
                case 4:
                    newc = c + 1;
                    newl = l;
                    break;
                case 5:
                    newc = c - 1;
                    newl = l + 1;
                    break;
                case 6:
                    newc = c;
                    newl = l + 1;
                    break;
                case 7:
                    newc = c + 1;
                    newl = l + 1;
                    break;
                default:
                    break;
                }
                if (swap < 8 && grid_[newl][newc].state != Sir::q &&
                    grid_[l][c].state != Sir::q &&
                    grid_[newl][newc].state != Sir::q_edge &&
                    grid_[l][c].state != Sir::q_edge) {
                    std::swap(grid_[l][c], grid_[newl][newc]);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Long range movement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sir::Board::airplane_() {
    if (day_ > quarantin_.first_day && day_ < quarantin_.last_day &&
        (int)advanced_opt_ >= 4) {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                if (((l < quarantin_.len_line || l > quarantin_.len_line * 2) ||
                    (c < quarantin_.len_line || c > quarantin_.len_line * 2))) {
                    int column = (rand() + time(nullptr)) % (dimension_);
                    int line = (rand() + time(nullptr)) % (dimension_);
                    if (((grid_[l][c].state != Sir::q_edge &&
                        grid_[column][line].state != Sir::q_edge) ||
                        (grid_[l][c].state != Sir::q &&
                            grid_[column][line].state != Sir::q)) &&
                        ((line < quarantin_.len_line || line > quarantin_.len_line * 2) ||
                            (column < quarantin_.len_line ||
                                column > quarantin_.len_line * 2))) {
                        if (line == 1) {
                            std::swap(grid_[l][c], grid_[line][column]);
                        }
                    }
                }
            }
        }
    }
    else {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                int column = (rand() + time(nullptr)) % (dimension_);
                int line = (rand() + time(nullptr)) % (dimension_);
                if (grid_[l][c].state != Sir::q_edge &&
                    grid_[column][line].state != Sir::q_edge) {
                    if (line == 1) {
                        std::swap(grid_[l][c], grid_[line][column]);
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  This member function activates SFML graphics.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void sir::Board::draw(int& milliseconds) {
    float bit_size = 1.;
    if (dimension_ < 350) {
        bit_size = 2.;
    }
    int win_size = static_cast<int>(bit_size * dimension_);
    assert(win_size > 99 && win_size < 1001);
    sf::RenderWindow window(sf::VideoMode(win_size * 2, win_size), name_,
        sf::Style::Close | sf::Style::Resize);
    window.setVerticalSyncEnabled(true);
    sf::RectangleShape sus_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape inf_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape rec_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape board(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape q_bit(sf::Vector2f(bit_size, bit_size));

    sus_bit.setFillColor(sf::Color::Green);
    inf_bit.setFillColor(sf::Color::Red);
    rec_bit.setFillColor(sf::Color::Blue);
    board.setFillColor(sf::Color::Magenta);
    q_bit.setFillColor(sf::Color::Black);

    Counter q_graph_points;
    Counter graph_points;
    int proportion =
        dimension_ - ((quarantin_.len_line * quarantin_.len_line) / dimension_);

    std::cout << "day---------"
        << "number_infected" << '\n';
    bool flag = true;
    while (window.isOpen()) {
        if (day_ >= quarantin_.first_day && flag && (int)advanced_opt_ >= 4) {
            quarantine_();
            flag = false;
        }
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            if (evnt.type == sf::Event::Closed) {
                window.close();
            }

        }
        if (day_ >= 300) {
            window.close();
        }
        std::cout << "" << day_ + 1 << "            "
            << counter_.num_i + q_counter_.num_i << '\n';
        evolve_();
        if ((int)advanced_opt_ >= 1) {
            move_();
        }
        if ((int)advanced_opt_ >= 2) {
            airplane_();
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Here I print the Grid.
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        for (int l = 1; l < dimension_ - 1; ++l) {
            for (int c = 1; c < dimension_ - 1; ++c) {
                switch (grid_[l][c].state) {
                case Sir::s:
                    sus_bit.setPosition(bit_size * c, bit_size * l);
                    window.draw(sus_bit);
                    break;
                case Sir::i:
                    inf_bit.setPosition(bit_size * c, bit_size * l);
                    window.draw(inf_bit);
                    break;
                case Sir::r:
                    rec_bit.setPosition(bit_size * c, bit_size * l);
                    window.draw(rec_bit);
                    break;
                case Sir::q:
                    q_bit.setPosition(bit_size * c, bit_size * l);
                    window.draw(q_bit);
                    break;
                case Sir::q_edge:
                    board.setPosition(bit_size * c, bit_size * l);
                    window.draw(board);
                    break;
                default:
                    throw std::runtime_error("WARNING: invalid Sir type in a bit.\n");
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Here I print the graphs.
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if ((int)advanced_opt_ >= 4) {
            q_graph_points.num_s =
                static_cast<int>(q_counter_.num_s / (quarantin_.len_line));
            q_graph_points.num_i =
                static_cast<int>(q_counter_.num_i / (quarantin_.len_line));
            q_graph_points.num_r =
                static_cast<int>(q_counter_.num_r / (quarantin_.len_line));
            graph_points.num_s = static_cast<int>(
                (static_cast<double>(counter_.num_s) - q_counter_.num_s) * 0.5 /
                (proportion));
            graph_points.num_i =
                static_cast<int>((static_cast<double>(counter_.num_i) +
                    counter_.num_q - q_counter_.num_i) *
                    0.5 / (proportion));
            graph_points.num_r = static_cast<int>(
                (static_cast<double>(counter_.num_r) - q_counter_.num_r) * 0.5 /
                (proportion));
            graph_in_quarantine_.push_back({ q_graph_points });
            graph_out_quarantine_.push_back({ graph_points });
            for (int i = dimension_ + 1; i < 2 * dimension_; ++i) {
                board.setPosition(i * bit_size,
                    bit_size * static_cast<float>(dimension_ / 2));
                window.draw(board);
                for (int counter = 0; counter != (int)graph_out_quarantine_.size();
                    ++counter) {
                    if (graph_out_quarantine_[counter].num_i <= 1) {
                        inf_bit.setPosition(
                            static_cast<float>(dimension_ + counter) * bit_size,
                            static_cast<float>(dimension_ / 2) * bit_size - 2);
                        window.draw(inf_bit);
                    }
                    else {
                        inf_bit.setPosition(
                            static_cast<float>(dimension_ + counter + 1) * bit_size,
                            bit_size * static_cast<float>(
                                dimension_ / 2 -
                                graph_out_quarantine_[counter].num_i) -
                            2);
                        window.draw(inf_bit);
                    }
                    if (graph_out_quarantine_[counter].num_s <= 1) {
                        sus_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter),
                            bit_size * static_cast<float>(dimension_ / 2) - 1);
                        window.draw(sus_bit);
                    }
                    else {
                        sus_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter + 1),
                            bit_size * static_cast<float>(
                                dimension_ / 2 -
                                graph_out_quarantine_[counter].num_s) +
                            1);
                        window.draw(sus_bit);
                    }
                    if (graph_out_quarantine_[counter].num_r <= 1) {
                        rec_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter),
                            bit_size * static_cast<float>(dimension_ / 2) - 2);
                        window.draw(rec_bit);
                    }
                    else {
                        rec_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter + 1),
                            bit_size * static_cast<float>(
                                dimension_ / 2 -
                                graph_out_quarantine_[counter].num_r) -
                            2);
                        window.draw(rec_bit);
                    }
                    if (graph_in_quarantine_[counter].num_i <= 1) {
                        inf_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter),
                            bit_size * static_cast<float>(dimension_) - 2);
                        window.draw(inf_bit);
                    }
                    else {
                        inf_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter + 1),
                            bit_size * static_cast<float>(dimension_) -
                            graph_in_quarantine_[counter].num_i + 1);
                        window.draw(inf_bit);
                    }
                    if (graph_in_quarantine_[counter].num_s <= 1) {
                        sus_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter),
                            bit_size * static_cast<float>(dimension_) - 1);
                        window.draw(sus_bit);
                    }
                    else {
                        sus_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter + 1),
                            bit_size *
                            static_cast<float>(
                                dimension_ - graph_in_quarantine_[counter].num_s + 2));
                        window.draw(sus_bit);
                    }
                    if (graph_in_quarantine_[counter].num_r <= 1) {
                        rec_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter),
                            bit_size * static_cast<float>(dimension_) - 1);
                        window.draw(rec_bit);
                    }
                    else {
                        rec_bit.setPosition(
                            bit_size * static_cast<float>(dimension_ + counter + 1),
                            bit_size *
                            static_cast<float>(
                                dimension_ - graph_in_quarantine_[counter].num_r) +
                            1);
                        window.draw(rec_bit);
                    }
                }
            }
        }
        else {
            graph_points.num_i =
                static_cast<int>((counter_.num_i + counter_.num_q) / dimension_);
            graph_points.num_s = static_cast<int>(counter_.num_s / dimension_);
            graph_points.num_r = static_cast<int>(counter_.num_r / dimension_);
            graph_out_quarantine_.push_back({ graph_points });
            for (int counter = 0; counter != (int)graph_out_quarantine_.size();
                ++counter) {
                if (graph_out_quarantine_[counter].num_i <= 1) {
                    inf_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter),
                        bit_size * static_cast<float>(dimension_) - 2);
                    window.draw(inf_bit);
                }
                else {
                    inf_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter + 1),
                        bit_size *
                        static_cast<float>(dimension_ -
                            graph_out_quarantine_[counter].num_i) +
                        1);
                    window.draw(inf_bit);
                }
                if (graph_out_quarantine_[counter].num_s <= 1) {
                    sus_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter),
                        bit_size * static_cast<float>(dimension_) - 1);
                    window.draw(sus_bit);
                }
                else {
                    sus_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter + 1),
                        bit_size *
                        static_cast<float>(dimension_ -
                            graph_out_quarantine_[counter].num_s) +
                        1);
                    window.draw(sus_bit);
                }
                if (graph_out_quarantine_[counter].num_r <= 1) {
                    rec_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter),
                        bit_size * static_cast<float>(dimension_) - 1);
                    window.draw(rec_bit);
                }
                else {
                    rec_bit.setPosition(
                        bit_size * static_cast<float>(dimension_ + counter + 1),
                        bit_size *
                        static_cast<float>(dimension_ -
                            graph_out_quarantine_[counter].num_r) +
                        1);
                    window.draw(rec_bit);
                }
            }
        }
        window.display();
        window.clear(sf::Color::Black);
    }
}
