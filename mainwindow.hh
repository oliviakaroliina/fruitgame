/*
#############################################################################
# File: mainwindow.hh                                                       #
# Description: Mainwindow-classes methods                                   #
# Modified: 7.12.-14.12.2020                                                #
#                                                                           #
# Program author:                                                           #
# Name: Olivia Saukonoja                                                    #
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

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <vector>
#include <deque>
#include <random>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Changes the places of 2 fruits
    // Calls for find method when the swapping is done
    // Returns nothing
    void swap_fruits();

    // Drops the fruits that have empty spaces under them
    // Calls for find method when the dropping is done
    // Returns nothing
    void drop_fruits();

    // Restarts the program
    // Returns nothing
    void on_restartButton_clicked();

    // Saves the player's name and points to the file
    // Returns nothing
    void on_quitButton_clicked();

private:
    Ui::MainWindow *ui;

    // CONSTANTS:

    // Margins for the drawing area (the graphicsView object)
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 100;
    // Size of a square containing a fruit
    const int SQUARE_SIDE = 45;
    // Number of vertical cells (places for fruits)
    const int ROWS = 8;
    // Number of horizontal cells (places for fruits)
    const int COLUMNS = 11;
    // Constants describing the drawing area coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = SQUARE_SIDE * ROWS;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = SQUARE_SIDE * COLUMNS;

    // Constant describing the usage of coordinates in attributes
    const int NOTHING_SAVED = -1;
    // Constant describing the empty place in the data structure fileboard_
    const std::string EMPTY = "";

    // Vector of fruit filenames without affixes
    const std::vector<std::string>
            IMAGES = {"strawberry", "pear", "bananas", "grapes"};
    const int NUMBER_OF_FRUITS = 4;

    // Constants describing the filenames' affixes
    const std::string PREFIX = ":/";
    const std::string SUFFIX = ".png";

    // Constant describing the time between deleting and dropping
    const int DELAY = 200;

    // Constans for giving points to the player
    // Deleted represents the amount of neighbouring fruits
    // and points the points that the player gets from removing fruits
    struct Point_info {
        int deleted;
        int points;
    };

    const std::vector<Point_info> POINTS = {
        {COLUMNS-8, COLUMNS-8},
        {COLUMNS-7, COLUMNS-6},
        {COLUMNS-6, COLUMNS-4},
        {COLUMNS-5, COLUMNS-2},
        {COLUMNS-4, COLUMNS},
        {COLUMNS-3, COLUMNS+2},
        {COLUMNS-2, COLUMNS+4},
        {COLUMNS-1, COLUMNS+6},
        {COLUMNS, COLUMNS+8}
    };

    const std::string FILE_OF_POINTS = "points.txt";


    // DATA STRUCTURES:

    // Stores the pointers of the push buttons
    std::vector<std::vector<QPushButton*>> fruitboard_;

    // Describes the icons of the push buttons
    // by storing the names of the fruit files
    std::vector<std::vector<std::string>> fileboard_;


    // ATTRIBUTES:

    // For randomly selecting fruits for the grid
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // Coordinates for swapping two fruits
    int x1_;
    int y1_;
    int x2_;
    int y2_;

    // Push button pointers for swapping two fruits
    QPushButton* swap_button_1_;
    QPushButton* swap_button_2_;

    // For saving the points of the player
    int points_;

    // For saving all of the player's points
    std::map<std::string, int> all_points;


    // METHODS:

    // Makes the push buttons for the grid,
    // adds fruit to the data structures
    // and connects it to the slot
    // Returns nothing
    void create_push_buttons();

    // Parameters:
    // - filename: name of the image file without affixes (compatible
    //             for comparing with constant vector IMAGES)
    // - row: where the possible new fruit would be
    // - column: where the possible new fruit would be
    // Checks if there are three same fruits on the trot in one row or column
    // Returns true if the new fruit would be the third on the trot
    // and false if wouldn't
    bool is_same_as_two_previous(const std::string& filename, const int& row,
                                 const int& column) const;

    // Parameters:
    // - filename: name of the image file without affixes
    // - row: where the push button locates
    // - column: where the push button locates
    // - push_button: the button where to draw the fruit
    // Draws one fruit to the icon of the push button
    // Returns nothing
    void draw_fruit(const std::string& filename, const int& row,
                    const int& column, QPushButton*& push_button) const;

    // Finds and returns the button that has been clicked
    QPushButton* get_button() const;

    // Parameters:
    // - button: the pointer to the button that has been clicked
    // - get_row: if true, returns the row where the button is,
    //            if false, returns the column
    // Returns the row where the button is
    int get_location(QPushButton*& button, const bool& get_row) const;

    // Looks for three or more fruits in a row
    // Calls for delete_neighbouring method
    // if there's something to delete
    // Returns nothing
    void find_neighbouring_horizontally();

    // Looks for three or more fruits in a column
    // Calls for delete_neighbouring method
    // if there's something to delete
    // Returns nothing
    void find_neighbouring_vertically();

    // Parameters:
    // - row: where the first fruit to be deleted locates
    // - column: where the first fruit to be deleted locates
    // - amount: how many fruits has to be deleted
    // - delete_from_row: if true, deletes the fruits horizontally,
    //                    if false, deletes vertically
    // Deletes neighbouring fruits, calls for add_points and uses timer
    // to wait between deleting and calling the drop_fruits method
    // Returns nothing
    void delete_neighbouring(const int& row, const int& column,
                             const int& amount, const bool& delete_from_row);

    // Parameters:
    // - amount: how many fruits has been deleted
    // Adds points to the attribute points_
    // and shows it to the user
    // Returns nothing
    void add_points(const int& amount);

    // Returns true if there's fruits to drop
    // and false if isn't
    bool is_there_something_to_drop() const;

    // Parameters:
    // - s: the string to be splitted
    // - separator: the separator that splits
    // Splits a row into smaller strings
    // and adds them to a vector of strings
    std::vector<std::string> split(const std::string& s, const char& separator,
                                   bool ignore_empty);

    // Gets high scores from a file and shows them to the user
    // Returns nothing
    void show_high_scores();

};
#endif // MAINWINDOW_HH
