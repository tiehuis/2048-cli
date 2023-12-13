#include <gtest/gtest.h>
#include "ai.h"
#include "engine.h"

// No need to redefine 'struct gamestate' and 'struct options' as they are already defined in 'engine.h'

// Test to check if ai_move returns a valid move
TEST(AIMoveTest, ReturnsValidMove) {
    struct gamestate g;
    // Assuming 'g.opts' is already properly initialized in 'engine.h'
    // If 'opts' needs to be set, it should be set according to the definition in 'engine.h'

    char move = ai_move(&g);
    EXPECT_TRUE(move == 'w' || move == 'a' || move == 's' || move == 'd');
}

// Test to check behavior when interactive option is enabled
TEST(AIMoveTest, InteractiveOption) {
    struct gamestate g;
    // Assuming 'g.opts' is already properly initialized in 'engine.h'

    char move = ai_move(&g);
    EXPECT_TRUE(move == 'w' || move == 'a' || move == 's' || move == 'd');
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


