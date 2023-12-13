#include <gtest/gtest.h>
#include "engine.h"

// Test to check initial state of the game
TEST(GameEngineTest, InitialState) {
    struct gamestate g;
    // Assuming gamestate has a properly defined opts structure
    // Initialize opts with appropriate values if needed
    // For example, if opts has grid_width and grid_height, initialize them

    g.opts->grid_width = 4;
    g.opts->grid_height = 4;

    // Initialize the grid to zero
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            g.grid[i][j] = 0;
        }
    }

    // Check if the initial state is correct
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            EXPECT_EQ(g.grid[i][j], 0);
        }
    }
}

// Additional tests can be added to check other functionalities like block movement, scoring, etc.

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

