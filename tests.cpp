#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "newick.h"
#include "util.h"
#include "cherrypick_semitemporal.h"

TEST_CASE( "Removal + reconstruction works", "[reconstruction]" ) {
    std::string trees("((10,(13,(((15,47),18),32))),11);\n"
                "(((10,(13,((15,18),32))),11),47);\n"
                "(((10,(13,((15,18),32))),11),47);\n"
                "((10,(13,(((15,47),18),32))),11);\n");

    auto treeParsed = parseFile(trees);
    auto numeric = labelNumeric(treeParsed, true);
    auto indexed = indexTrees(numeric);
    reco_sequence<tree_leaf> rec;

    indexed[0]->remove(15, NULL, &rec);
    indexed[0]->remove(15, NULL, &rec);
    indexed[2]->remove(15, NULL, &rec);
    REQUIRE(trees != toString(indexed));
    assertTrees(indexed);
    reconstruct<tree_leaf>(indexed, NULL, rec);
    assertTrees(indexed);
    REQUIRE(trees == toString(indexed));
}

TEST_CASE( "Cherry picking + reconstruction works", "[reconstruction]" ) {
    std::string trees("((10,(13,(((15,47),18),32))),11);\n"
                      "(((10,(13,((15,18),32))),11),47);\n"
                      "(((10,(13,((15,18),32))),11),47);\n"
                      "((10,(13,(((15,47),18),32))),11);\n");

    auto treeParsed = parseFile(trees);
    auto numeric = labelNumeric(treeParsed, true);
    auto indexed = indexTrees(numeric);
    reco_sequence<tree_leaf> rec;

    indexed[0]->remove({15, 47}, NULL, &rec);
    indexed[1]->remove({15, 48}, NULL, &rec);
    assertTrees(indexed);
    REQUIRE(trees != toString(indexed));
    reconstruct<tree_leaf>(indexed, NULL, rec);
    assertTrees(indexed);
    REQUIRE(trees == toString(indexed));
}