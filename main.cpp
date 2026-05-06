#include <iostream>
using namespace std;

int absolute(int n)
{
    return n < 0 ? -n : n;
}

class piece // base class
{
protected:
    char color;
    char symbol;

public:
    piece(char c, char symbol) // Init
    {
        color = c;
        this->symbol = symbol;
    }
    virtual ~piece() {} // Virtual destructor for safe polymorphic deletion
    char getcolor()     // Get color
    {
        return color;
    }
    char getsymbol() // Get symbol
    {
        return symbol;
    }
    virtual bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *board[8][8]) // Validate move
    {
        return false;
    }
};

class pawn : public piece
{
public:
    pawn(char c) : piece(c, 'P') {} // Init

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        int direction = (color == 'W') ? -1 : 1;

        // forward direction
        if (startingcolumn == endingcolumn && b[endingrow][endingcolumn] == NULL) // Forward move
        {
            if (endingrow == startingrow + direction) // 1 step
                return true;

            // double move
            if ((startingrow == 6 && color == 'W') || (startingrow == 1 && color == 'B')) // Start pos
            {
                if (endingrow == startingrow + 2 * direction && b[startingrow + direction][startingcolumn] == NULL) // 2 steps
                    return true;
            }
        }

        // capture
        if (absolute(startingcolumn - endingcolumn) == 1 && endingrow == startingrow + direction && b[endingrow][endingcolumn] != NULL) // Diagonal capture
        {
            if (b[endingrow][endingcolumn]->getcolor() != color) // Enemy?
                return true;
        }

        return false;
    }
};

class rook : public piece
{
public:
    bool moved;

    rook(char c) : piece(c, 'R') // Init
    {
        moved = false;
    }

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        if (startingrow != endingrow && startingcolumn != endingcolumn) // Not straight
            return false;

        int row_change_step = (endingrow > startingrow) ? 1 : (endingrow < startingrow ? -1 : 0);
        int column_change_step = (endingcolumn > startingcolumn) ? 1 : (endingcolumn < startingcolumn ? -1 : 0);

        int row = startingrow + row_change_step, column = startingcolumn + column_change_step;

        while (row != endingrow || column != endingcolumn) // Walk path
        {
            if (b[row][column] != NULL) // Blocked?
                return false;

            row += row_change_step;
            column += column_change_step;
        }
        return true;
    }
};

class knight : public piece
{
public:
    knight(char c) : piece(c, 'N') {} // Init

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        int row = absolute(startingrow - endingrow), column = absolute(startingcolumn - endingcolumn);
        return (row == 2 && column == 1) || (row == 1 && column == 2);
    }
};

class bishop : public piece
{
public:
    bishop(char c) : piece(c, 'B') {} // Init

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        if (absolute(startingrow - endingrow) != absolute(startingcolumn - endingcolumn)) // Not diagonal
            return false;

        int row_change_step = (endingrow > startingrow) ? 1 : -1;
        int column_change_step = (endingcolumn > startingcolumn) ? 1 : -1;

        int row = startingrow + row_change_step, column = startingcolumn + column_change_step;
        while (row != endingrow) // Walk path
        {
            if (b[row][column] != NULL) // Blocked?
                return false;

            row += row_change_step;
            column += column_change_step;
        }
        return true;
    }
};

class queen : public piece
{
public:
    queen(char c) : piece(c, 'Q') {} // Init

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        rook r(color);
        bishop bi(color);
        return r.isvalidmove(startingrow, startingcolumn, endingrow, endingcolumn, b) ||
               bi.isvalidmove(startingrow, startingcolumn, endingrow, endingcolumn, b);
    }
};

class king : public piece
{
public:
    bool moved;

    king(char c) : piece(c, 'K') // Init
    {
        moved = false;
    }

    bool isvalidmove(int startingrow, int startingcolumn, int endingrow, int endingcolumn, piece *b[8][8]) // Validate move
    {
        return absolute(startingrow - endingrow) <= 1 && absolute(startingcolumn - endingcolumn) <= 1;
    }
};

class board
{
public:
    piece *matrix[8][8];

    board() // Init
    {
        for (int i = 0; i < 8; i++)     // Rows
            for (int j = 0; j < 8; j++) // Cols
                matrix[i][j] = NULL;
    }

    ~board() // Cleanup dynamically allocated pieces to prevent memory leaks
    {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (matrix[i][j])
                    delete matrix[i][j];
    }

    void init() // Setup board
    {
        for (int i = 0; i < 8; i++) // Place pawns
        {
            matrix[1][i] = new pawn('B');
            matrix[6][i] = new pawn('W');
        }

        matrix[0][0] = new rook('B');
        matrix[0][7] = new rook('B'); // rook
        matrix[7][0] = new rook('W');
        matrix[7][7] = new rook('W');

        matrix[0][1] = new knight('B');
        matrix[0][6] = new knight('B'); // knight
        matrix[7][1] = new knight('W');
        matrix[7][6] = new knight('W');

        matrix[0][2] = new bishop('B');
        matrix[0][5] = new bishop('B'); // bishop
        matrix[7][2] = new bishop('W');
        matrix[7][5] = new bishop('W');

        matrix[0][3] = new queen('B');
        matrix[7][3] = new queen('W'); // queen

        matrix[0][4] = new king('B');
        matrix[7][4] = new king('W');
    }

    void display() // Print board
    {
        cout << "\n  a b c d e f g h\n";
        for (int i = 0; i < 8; i++) // Rows
        {
            cout << 8 - i << " ";
            for (int j = 0; j < 8; j++) // Cols
            {
                if (matrix[i][j] == NULL) // Empty
                    cout << ". ";
                else // Occupied
                {
                    char symbol = matrix[i][j]->getsymbol();

                    if (matrix[i][j]->getcolor() == 'B') // Lowercase Black
                    {
                        if (symbol >= 'A' && symbol <= 'Z')
                            symbol = symbol + 32;
                    }

                    cout << symbol << " ";
                }
            }
            cout << 8 - i;
            cout << endl;
        }

        cout << "  a b c d e f g h\n";
    }

    bool ischeck(char color) // Is check?
    {
        int kingrow = -1, kingcolumn = -1;                                                                 // Initialize to prevent garbage values
        for (int i = 0; i < 8; i++)                                                                        // Rows
            for (int j = 0; j < 8; j++)                                                                    // Cols
                if (matrix[i][j] && matrix[i][j]->getsymbol() == 'K' && matrix[i][j]->getcolor() == color) // Find king
                {
                    kingrow = i;
                    kingcolumn = j;
                }

        if (kingrow == -1 || kingcolumn == -1)
            return false; // King not found failsafe

        for (int i = 0; i < 8; i++)                                                   // Rows
            for (int j = 0; j < 8; j++)                                               // Cols
                if (matrix[i][j] && matrix[i][j]->getcolor() != color)                // Find enemy
                    if (matrix[i][j]->isvalidmove(i, j, kingrow, kingcolumn, matrix)) // Attacks king?
                        return true;

        return false;
    }

    bool hasmove(char color) // Has move?
    {
        for (int i = 0; i < 8; i++)                                    // Rows
            for (int j = 0; j < 8; j++)                                // Cols
                if (matrix[i][j] && matrix[i][j]->getcolor() == color) // Friendly piece
                    for (int row = 0; row < 8; row++)                  // Target rows
                        for (int column = 0; column < 8; column++)     // Target cols
                        {
                            if (matrix[i][j]->isvalidmove(i, j, row, column, matrix)) // Legal move?
                            {
                                if (matrix[row][column] && matrix[row][column]->getcolor() == color) // Friendly capture check
                                    continue;

                                piece *movedPiece = matrix[i][j];
                                piece *temp = matrix[row][column];

                                matrix[row][column] = movedPiece;
                                matrix[i][j] = NULL;

                                bool check = ischeck(color);

                                matrix[i][j] = movedPiece;
                                matrix[row][column] = temp;

                                if (!check) // Escapes check?
                                    return true;
                            }
                        }
        return false;
    }

    bool move(int startingrow, int startingcolumn, int endingrow, int endingcolumn, bool turn) // Move piece
    {
        if (!matrix[startingrow][startingcolumn]) // Empty start
            return false;

        if (turn && matrix[startingrow][startingcolumn]->getcolor() != 'W') // Wrong color
            return false;
        if (!turn && matrix[startingrow][startingcolumn]->getcolor() != 'B') // Wrong color
            return false;

        if (matrix[startingrow][startingcolumn]->getsymbol() == 'K' && absolute(endingcolumn - startingcolumn) == 2) // Castling
        {
            if (ischeck(turn ? 'W' : 'B')) // Cannot castle out of check
                return false;

            king *k = (king *)matrix[startingrow][startingcolumn];
            if (k->moved) // King moved
                return false;

            int rookCol = (endingcolumn > startingcolumn) ? 7 : 0;
            piece *cornerPiece = matrix[startingrow][rookCol];

            if (!cornerPiece || cornerPiece->getsymbol() != 'R') // Must be a rook
                return false;

            rook *r = (rook *)cornerPiece;
            if (r->moved) // Rook moved
                return false;

            int step = (endingcolumn > startingcolumn) ? 1 : -1;

            for (int column = startingcolumn + step; column != rookCol; column += step) // Path clear?
                if (matrix[startingrow][column] != NULL)                                // Blocked
                    return false;

            for (int column = startingcolumn + step; column != endingcolumn + step; column += step) // Squares safe?
            {
                piece *temp = matrix[startingrow][column];

                matrix[startingrow][column] = matrix[startingrow][startingcolumn];
                matrix[startingrow][startingcolumn] = NULL;

                if (ischeck(turn ? 'W' : 'B')) // Passes check
                {
                    matrix[startingrow][startingcolumn] = matrix[startingrow][column];
                    matrix[startingrow][column] = temp;
                    return false;
                }

                matrix[startingrow][startingcolumn] = matrix[startingrow][column];
                matrix[startingrow][column] = temp;
            }

            matrix[startingrow][endingcolumn] = matrix[startingrow][startingcolumn];
            matrix[startingrow][startingcolumn] = NULL;

            matrix[startingrow][startingcolumn + step] = matrix[startingrow][rookCol];
            matrix[startingrow][rookCol] = NULL;

            k->moved = true; // Mark king
            r->moved = true; // Mark rook

            return true;
        }

        if (matrix[endingrow][endingcolumn] && matrix[endingrow][endingcolumn]->getcolor() == matrix[startingrow][startingcolumn]->getcolor()) // Friendly capture
            return false;

        if (!matrix[startingrow][startingcolumn]->isvalidmove(startingrow, startingcolumn, endingrow, endingcolumn, matrix)) // Invalid move
            return false;

        piece *temp = matrix[endingrow][endingcolumn];

        matrix[endingrow][endingcolumn] = matrix[startingrow][startingcolumn];
        matrix[startingrow][startingcolumn] = NULL;

        if (ischeck(turn ? 'W' : 'B')) // Leaves check
        {
            matrix[startingrow][startingcolumn] = matrix[endingrow][endingcolumn];
            matrix[endingrow][endingcolumn] = temp;
            return false;
        }

        if (matrix[endingrow][endingcolumn]->getsymbol() == 'K') // Mark king
            ((king *)matrix[endingrow][endingcolumn])->moved = true;

        if (matrix[endingrow][endingcolumn]->getsymbol() == 'R') // Mark rook
            ((rook *)matrix[endingrow][endingcolumn])->moved = true;

        if (matrix[endingrow][endingcolumn]->getsymbol() == 'P' && (endingrow == 0 || endingrow == 7)) // Promote pawn
        {
            char col = matrix[endingrow][endingcolumn]->getcolor();
            delete matrix[endingrow][endingcolumn];
            matrix[endingrow][endingcolumn] = new queen(col);
        }

        if (temp != NULL) // Delete the captured piece from memory
            delete temp;

        return true;
    }
};

int main() // Main
{
    board b;
    b.init();
    bool turn = true;

    while (true) // Game loop
    {
        b.display();

        if (b.ischeck(turn ? 'W' : 'B') && !b.hasmove(turn ? 'W' : 'B')) // Checkmate
        {
            cout << "CHECKMATE!\n";
            cout << (turn ? "Black Wins!\n" : "White Wins!\n");
            break;
        }

        if (!b.ischeck(turn ? 'W' : 'B') && !b.hasmove(turn ? 'W' : 'B')) // Stalemate
        {
            cout << "STALEMATE!\n";
            break;
        }

        cout << (turn ? "White" : "Black") << " turn:\n";

        char column1, column2;
        int row1, row2;

        cout << "Current position: ";
        cin >> column1 >> row1;
        cout << "Moving position: ";
        cin >> column2 >> row2;

        if (column1 < 'a' || column1 > 'h' || column2 < 'a' || column2 > 'h') // Bounds check
        {
            cout << "Invalid column!\n";
            continue;
        }

        if (row1 < 1 || row1 > 8 || row2 < 1 || row2 > 8) // Bounds check
        {
            cout << "Invalid row!\n";
            continue;
        }

        int startingcolumn = column1 - 'a';
        int startingrow = 8 - row1;
        int endingcolumn = column2 - 'a';
        int endingrow = 8 - row2;

        if (b.move(startingrow, startingcolumn, endingrow, endingcolumn, turn)) // Try move
            turn = !turn;
        else // Invalid
            cout << "Invalid move!\n";
    }
}
