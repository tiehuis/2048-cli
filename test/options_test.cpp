#include <gtest/gtest.h>
#include "options.h"

// Test for default game options initialization
TEST(OptionsTest, DefaultInitialization) {
    struct gameoptions* opts = gameoptions_default();
    ASSERT_NE(opts, nullptr);

    // Check if the default values are set correctly
    EXPECT_EQ(opts->grid_height, DEFAULT_GRID_HEIGHT);
    EXPECT_EQ(opts->grid_width, DEFAULT_GRID_WIDTH);
    EXPECT_EQ(opts->spawn_value, DEFAULT_SPAWN_VALUE);
    EXPECT_EQ(opts->enable_color, DEFAULT_COLOR_FLAG);
    EXPECT_EQ(opts->animate, DEFAULT_ANIMATE_FLAG);
    EXPECT_EQ(opts->ai, DEFAULT_AI_FLAG);
    EXPECT_EQ(opts->interactive, DEFAULT_INTERACTIVE_FLAG);

    gameoptions_destroy(opts);
}

// Test for parsing command-line options
TEST(OptionsTest, ParseOptions) {
    struct gameoptions opts;
    int argc = 3;
    char* argv[] = {"program", "-h", "4", "-w", "4", nullptr};

    struct gameoptions* parsed_opts = parse_options(&opts, argc, argv);
    ASSERT_NE(parsed_opts, nullptr);

    // Check if the parsed values are correct
    EXPECT_EQ(parsed_opts->grid_height, 4);
    EXPECT_EQ(parsed_opts->grid_width, 4);

    // Additional checks can be added based on the actual implementation
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

