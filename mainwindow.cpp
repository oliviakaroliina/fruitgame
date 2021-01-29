/*
#############################################################################
# File: mainwindow.cpp                                                      #
# Description: Mainwindow-classes methods.                                  #
# Modified: 7.12.-14.12.2020                                                #
# Program author:                                                           #
# Name: Olivia Saukonoja                                                    #
#                                                                           #
# ATTENTION! The program works in this order:                               #
# 1. Making the game board                                                  #
# 2. When the user clicks on the push button in the game board, the program #
#    runs the swap method (the if-clause).                                  #
# 3. When the user clicks on the push button second time, the program runs  #
#    the swap method (the else-clause).                                     #
# 4. The swap method calls for find methods from the else-clause.           #
# 5. The find methods calls for delete method, if there's something to      #
#    delete.                                                                #
# 6. The delete method calls for drop slot.                                 #
# 7. The drop slot calls for the find methods again.                        #
# 8. Then the program continues the points 5-7 as long as there's something #
#    to delete.                                                             #
# 9. Then the user can click on the button again, so the program runs again #
#    all points from 2->.                                                   #
# Also, the delete method calls for add points method, that doesn't return  #
# anything and that doesn't call any new method.                            #
#############################################################################
*/

#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPixmap>
#include <QCursor>
#include <QGraphicsView>
#include <cmath>
#include <QApplication>
#include <QProcess>
#include <fstream>
#include <QString>
#include <iostream>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);

    // Takes the seed value from computer's clock
    int seed = time(0);

    // Wipes out the first random number (which is almost always 0)
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_);

    // Initializes the attributes
    x1_ = NOTHING_SAVED;
    y1_ = NOTHING_SAVED;
    x2_ = NOTHING_SAVED;
    y2_ = NOTHING_SAVED;
    swap_button_1_ = nullptr;
    swap_button_2_ = nullptr;
    points_ = 0;

    ui->scoreTextBrowser->setText(QString::number(points_));

    create_push_buttons();
    show_high_scores();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::create_push_buttons()
{
    for ( int row = 0; row < ROWS; ++row ) {
        // Adds a new vector for every row
        fruitboard_.push_back({});
        fileboard_.push_back({});

        for ( int column = 0; column < COLUMNS; ++column ) {
            // Generates a random fruit
            int i = distr_(randomEng_);
            std::string new_filename = IMAGES.at(i);

            // Checks there aren't too many same fruits on the trot
            while ( is_same_as_two_previous(new_filename, row, column) ) {
                int i = distr_(randomEng_);
                new_filename = IMAGES.at(i);
            }
            QPushButton* push_button = new QPushButton(this);

            draw_fruit(new_filename, row, column, push_button);

            connect(push_button, &QPushButton::clicked, this,
                    &MainWindow::swap_fruits);

            // Adds the new fruits to the data structures
            fruitboard_.at(row).push_back(push_button);
            fileboard_.at(row).push_back(new_filename);
        }
    }
}

bool MainWindow::is_same_as_two_previous(const std::string& filename, const
                                         int& row, const int& column) const
{
    // If two previous fruits exist in the row
    if ( column >= 2 ) {

        // Checks horizontally two previous fruits
        if ( fileboard_.at(row).at(column-1) == filename and
             fileboard_.at(row).at(column-2) == filename ) {
            return true;
        }
    }
    // If two previous fruits exist in the column
    if ( row >= 2 ) {

        // Checks vertically two previous fruits
        if ( fileboard_.at(row-1).at(column) == filename and
             fileboard_.at(row-2).at(column) == filename ) {
            return true;
        }
    }
    return false;
}

void MainWindow::draw_fruit(const std::string& filename, const int& row,
                            const int& column, QPushButton*& push_button) const
{
    // Converts image (png) to a pixmap
    std::string filename_with_affixes = PREFIX + filename + SUFFIX;
    QPixmap image(QString::fromStdString(filename_with_affixes));

    // Scales the pixmap
    image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

    // Sets the pixmap for the new label
    push_button->setIcon(image);

    // Sets the push button to the current row and column
    push_button->setGeometry(LEFT_MARGIN + column * SQUARE_SIDE,
                             TOP_MARGIN + row * SQUARE_SIDE,
                             SQUARE_SIDE, SQUARE_SIDE);
}

void MainWindow::swap_fruits()
{
    // Saves the fruits that has been pressed
    if ( swap_button_1_ == nullptr ) {
        swap_button_1_ = get_button();
        x1_ = get_location(swap_button_1_, true);
        y1_ = get_location(swap_button_1_, false);
    } else {
        // The first fruit has already been saved, so the swap can be done
        swap_button_2_ = get_button();
        x2_ = get_location(swap_button_2_, true);
        y2_ = get_location(swap_button_2_, false);

        // Checks that the fruits are neighbours to each other
        if ( ( abs(x1_ - x2_) == 1 and abs(y1_ - y2_) == 0 ) or
             ( abs(x1_ - x2_) == 0 and abs(y1_ - y2_) == 1 ) ) {

            // Swaps the fruits in the file data structure
            std::string first_filename = fileboard_.at(x1_).at(y1_);
            std::string second_filename = fileboard_.at(x2_).at(y2_);
            fileboard_.at(x1_).at(y1_) = second_filename;
            fileboard_.at(x2_).at(y2_) = first_filename;

            // Draws the new icons that the buttons get from each other
            draw_fruit(second_filename, x1_, y1_, swap_button_1_);
            draw_fruit(first_filename, x2_, y2_, swap_button_2_);
        }
        // Initializes again for the next swap
        x1_ = NOTHING_SAVED; y1_ = NOTHING_SAVED;
        x2_ = NOTHING_SAVED; y2_ = NOTHING_SAVED;
        swap_button_1_ = nullptr; swap_button_2_ = nullptr;

        find_neighbouring_horizontally();
        find_neighbouring_vertically();
    }
}

QPushButton* MainWindow::get_button() const
{
    for ( int row = 0; row < ROWS; ++row ) {

        for ( int column = 0; column < COLUMNS; ++column ) {

            // If the button exists
            if ( fruitboard_.at(row).at(column) != nullptr ) {

                // Gets the push button's coordinates
                QRect widgetRect = fruitboard_.at(row).at(column)->geometry();
                // Finds the mouse's coordinates
                QPoint mousePos = mapFromGlobal(QCursor::pos());

                if (widgetRect.contains(mousePos)) {
                    // The mouse's position is in the push button
                    return fruitboard_.at(row).at(column);
                }
            }
        }
    }
    return nullptr;
}

int MainWindow::get_location(QPushButton*& button, const bool& get_row ) const
{
    for ( int row = 0; row < ROWS; ++row ) {

        for ( int column = 0; column < COLUMNS; ++column ) {

            if ( fruitboard_.at(row).at(column) == button ) {
                // The fruit's row and column has found
                if ( get_row ) {
                   return row;
                } else {
                    return column;
                }

            }
        }
    }
    return NOTHING_SAVED;
}

void MainWindow::find_neighbouring_horizontally()
{
    int amount_of_neighbouring = 0;
    for ( int row = 0; row < ROWS; ++row ) {
        for ( int column = 0; column < COLUMNS; ++column ) {

            // If the column isn't the last or the second last one
            // and the place is not empty
            if ( (column != COLUMNS-1) and (column != COLUMNS-2) and
                 (fileboard_.at(row).at(column)) != EMPTY ) {

                // Checks if horizontally next three or more fruits are same
                for ( int n = 0; n < (COLUMNS-column); ++n ) {
                    if ( fileboard_.at(row).at(column+n) ==
                         fileboard_.at(row).at(column) ) {
                        // Increases the amount of neighbouring fruits
                        // Including also the fruit itself
                        ++amount_of_neighbouring;
                    } else {
                        // If there's even one different, stops looking for
                        // new similar fruits
                        break;
                    }
                }
                if ( amount_of_neighbouring >= 3 ) {
                    delete_neighbouring(row, column, amount_of_neighbouring,
                                        true);
                }
            }
            // Initializes the amount always before handling the next fruit
            amount_of_neighbouring = 0;
        }
    }
}

void MainWindow::find_neighbouring_vertically()
{
    int amount_of_neighbouring = 0;
    for ( int row = 0; row < ROWS; ++row ) {
        for ( int column = 0; column < COLUMNS; ++column ) {

            // If the row isn't the last or the second last one
            // and the place is not empty
            if ( (row != ROWS-1) and (row != ROWS-2) and
                 (fileboard_.at(row).at(column) != EMPTY) ) {

                // Checks if vertically next three or more fruits are same
                for ( int n = 0; n < (ROWS-row); ++n ) {
                    if ( fileboard_.at(row+n).at(column) ==
                         fileboard_.at(row).at(column) ) {
                        // Increases the amount of neighbouring fruits
                        // Including also the fruit itself
                        ++amount_of_neighbouring;
                    } else {
                        // If there's even one different, stops looking for
                        // new similar fruits
                        break;
                    }
                }
                if ( amount_of_neighbouring >= 3 ) {
                    delete_neighbouring(row, column, amount_of_neighbouring,
                                        false);
                }
            }
            // Initializes the amount always before handling the next fruit
            amount_of_neighbouring = 0;
        }
    }
}

void MainWindow::delete_neighbouring(const int& row, const int& column, const
                                     int& amount, const bool& delete_from_row)
{
    if ( delete_from_row ) {
        // Deletes all buttons from this row until the amount
        // Starting from the first neighbouring fruit
        for ( int i = 0; i < amount; ++i ) {
            fileboard_.at(row).at(column+i) = EMPTY;
            delete fruitboard_.at(row).at(column+i);
            fruitboard_.at(row).at(column+i) = nullptr;
        }
    } else {
        // Deletes all buttons from this column until the amount
        // Starting from the first neighbouring fruit
        for ( int i = 0; i < amount; ++i ) {
            fileboard_.at(row+i).at(column) = EMPTY;
            delete fruitboard_.at(row+i).at(column);
            fruitboard_.at(row+i).at(column) = nullptr;
        }
    }
    add_points(amount);
    QTimer::singleShot(DELAY, this, SLOT(drop_fruits()));
}

void MainWindow::add_points(const int& amount)
{
    // Updates points
    for ( auto p : POINTS ) {
        if ( amount == p.deleted ) {
            points_ += p.points;
        }
    }
    // Shows points to the player
    ui->scoreTextBrowser->setText(QString::number(points_));
}

void MainWindow::drop_fruits()
{
    while ( is_there_something_to_drop() ) {
        // Goes through every row except the first one
        // because nothing can be dropped to the first row
        for ( int row = 1; row < ROWS; ++row ) {
            for ( int column = 0; column < COLUMNS; ++column ) {

                // If the place is empty and if there's something above it
                if ( (fruitboard_.at(row).at(column) == nullptr) and
                     (fruitboard_.at(row-1).at(column) != nullptr)) {

                    // Move the fruit to the empty place
                    QPushButton* btn_to_be_moved = fruitboard_.at(row-1).
                                                   at(column);
                    fruitboard_.at(row).at(column) = btn_to_be_moved;
                    std::string name_to_be_moved = fileboard_.at(row-1).
                                                   at(column);
                    fileboard_.at(row).at(column) = name_to_be_moved;
                    btn_to_be_moved->setGeometry(LEFT_MARGIN + column *
                                                 SQUARE_SIDE, TOP_MARGIN + row
                                                 * SQUARE_SIDE, SQUARE_SIDE,
                                                 SQUARE_SIDE);

                    // Make the previous place empty
                    fruitboard_.at(row-1).at(column) = nullptr;
                    fileboard_.at(row-1).at(column) = EMPTY;
                }
            }
        }
    }
    find_neighbouring_vertically();
    find_neighbouring_horizontally();
}

bool MainWindow::is_there_something_to_drop() const
{
    // Goes through every row except the first one
    // because nothing can be dropped to the first row
    for ( int row = 1; row < ROWS; ++row ) {
        for ( int column = 0; column < COLUMNS; ++column ) {

            // If the place is empty and if there's something above it
            if ( (fruitboard_.at(row).at(column) == nullptr) and
                 (fruitboard_.at(row-1).at(column) != nullptr)) {
                return true;
            }
        }
    }
    return false;
}

void MainWindow::on_restartButton_clicked()
{
    // Saves the player's name and points
    on_quitButton_clicked();
    // Quits the previous program
    qApp->quit();
    // Starts a new one
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::on_quitButton_clicked()
{
    // Saves the player's name and points to the file
    QString player_name = ui->nameEdit->text();

    if ( player_name != "" and points_ != 0 ) {

        // Opens the file for reading
        std::ifstream file_object(FILE_OF_POINTS);
        if ( file_object ) {
            // Saves the info to attribute all_points
            std::string row;
            while ( getline(file_object, row) ) {
                if ( row != "" ) {
                    std::vector<std::string> parts = split(row, ':', true);
                    all_points.insert({ parts.at(1), stoi(parts.at(0)) });
                }
            }
            file_object.close();

            // Saves the player's info to attribute all_points
            if ( all_points.find(player_name.toStdString()) !=
                 all_points.end() ) {
                // The player has been played before
                if ( all_points.at(player_name.toStdString()) < points_ ) {
                    // The new points are better than the previous ones
                    all_points.at(player_name.toStdString()) = points_;
                }
            } else {
                // It's a new player
                all_points.insert({ player_name.toStdString(), points_ });
            }

            // Opens the file for writing
            std::ofstream output_file_object(FILE_OF_POINTS);

            // Declares a vector of pairs
            std::vector<std::pair<int, std::string>> new_points;

            // Copies the info from the map to the vector of pairs
            for ( auto it : all_points ) {
                new_points.push_back({it.second, it.first});
            }
            // Sorts the vector of pairs to the order of magnitude
            // i.e. points are in ascending order
            sort(new_points.begin(), new_points.end());
            reverse(new_points.begin(), new_points.end());

            // Writes the info (including the new info) in order of magnitude
            // to the file again
            for ( auto pair : new_points ) {
                output_file_object << pair.first << ":" << pair.second
                                   << std::endl;
            }
            output_file_object.close();
        }
    }
}

std::vector<std::string> MainWindow::split(const std::string& s, const char&
                                           separator, bool ignore_empty)
{
    std::vector<std::string> result;
    // For saving the rest of the string after splitting
    std::string tmp = s;

    while ( tmp.find(separator) != std::string::npos ) {

        // Saves the part to a variable
        std::string new_part = tmp.substr(0, tmp.find(separator));

        // Saves rest of the string without the part to the variable
        tmp = tmp.substr(tmp.find(separator)+1, tmp.size());

        // If the part is empty and the empty parts are wanted too
        // or vice versa, adds the part to the result
        if ( not ( ignore_empty and new_part.empty() ) ) {
            result.push_back(new_part);
        }
    }
    // Adds the last part to the result
    if ( not ( ignore_empty and tmp.empty() ) ) {
        result.push_back(tmp);
    }
    return result;
}

void MainWindow::show_high_scores()
{
    // Gets all players' high scores
    std::ifstream file_object(FILE_OF_POINTS);
    if ( file_object ) {
        std::string row;
        QString high_score_text = "";
        while ( getline(file_object, row) ) {
            high_score_text.append(QString::fromStdString(row));
            high_score_text.append("\n");
        }
        file_object.close();

        // Shows the scores to the player
        ui->highScoreTextBrowser->setText(high_score_text);
    }
}

