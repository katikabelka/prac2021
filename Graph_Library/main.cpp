#include "gtest/gtest.h"

#include "find_path.h"

TEST(TestCreation, AllTypes)
{
    std::vector<std::string> v1{{"AB"}};
    ASSERT_THROW(GraphFactory().Create("graph", std::make_unique<Simple_Opts>(v1)), std::invalid_argument);

    std::vector<std::string> v2 = {{"AB"}, {"BC"}, {"AC"}};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Simple_Opts>(v2)), std::invalid_argument);
    ASSERT_THROW (GraphFactory().Create("weighted", std::make_unique<Simple_Opts>(v2)), std::invalid_argument);

    std::vector<std::string> v3{{"AB"}, {"BC"}, {"CD"}, {"AE"}};
    std::vector<int> weights {1, 2, 3, 4};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Weighted_Opts>(v3, weights)), std::invalid_argument);
    ASSERT_THROW (GraphFactory().Create("bipartite", std::make_unique<Weighted_Opts>(v3, weights)), std::invalid_argument);

    std::vector<char> v4_part1 {'A', 'B', 'C', 'D'};
    std::vector<char> v4_part2 {'E', 'F'};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Bipartite_Opts> (v4_part1, v4_part2)), std::invalid_argument);
}

TEST(TestBipartite, AllIncluded) {
    std::vector<char> b1_part1 {'A', 'B', 'C', 'M'};
    std::vector<char> b1_part2 {'D', 'E'};
    std::unique_ptr<TGraph> b1 = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b1_part1, b1_part2));
    
    ASSERT_EQ ("WeightedGraph {AD:13, AE:13, BD:13, BE:13, CD:13, CE:13, MD:13, ME:13}", b1->AsWeighted(13)->ToString());
    ASSERT_EQ ("BipartiteGraph {{A, B, C, M}, {D, E}}", b1->ToString());
    ASSERT_EQ (std::vector<char>({'A', 'B', 'C', 'M', 'D', 'E'}), b1->GetVertices());
    std::vector<std::pair<char, char>> edg1 {{'A', 'D'}, {'A', 'E'}, {'B', 'D'}, {'B', 'E'}, {'C', 'D'}, {'C', 'E'}, {'M', 'D'}, {'M', 'E'}};
    ASSERT_EQ (edg1, b1->GetEdges());
    std::vector<char> b2_part1 {'B', 'K', 'M', 'N'};
    std::vector<char> b2_part2 {'E', 'F'};
    std::unique_ptr<TGraph> b2 = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b2_part1, b2_part2));
    std::vector<char> b3_part1 {'A', 'B', 'M'};
    std::vector<char> b3_part2 {'K', 'F'};
    std::unique_ptr<TGraph> b3 = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b3_part1, b3_part2));
    std::vector<char> v {'A','B','M', 'N', 'D'};
    std::unique_ptr<TGraph> complete = GraphFactory().Create("complete", std::make_unique<Complete_Opts> (v));
    std::vector<std::string> edges{{"AB"}, {"BF"}, {"FD"}, {"AE"}, {"ED"}};
    std::vector<int> weights {10, 12, 3, 15, 20};
    std::unique_ptr<TGraph> weighted = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(edges, weights));
    std::vector<std::string> vec{"EF", "FA", "AB", "EC"};
    std::unique_ptr<TGraph> simple = GraphFactory().Create("simple", std::make_unique<Simple_Opts>(vec));
    std::vector<std::string> v1{{"AB"}, {"BC"}};
    ASSERT_THROW(GraphFactory().Create("graph", std::make_unique<Simple_Opts>(v1)), std::invalid_argument);
    std::vector<std::string> v2 = {{"AB"}, {"BC"}, {"AC"}};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Simple_Opts>(v2)), std::invalid_argument);

    std::vector<std::string> v3{{"AB"}, {"BC"}, {"CD"}, {"AE"}, {"ED"}};
    std::vector<int> weights2 {10, 12, 3, 15, 20};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Weighted_Opts>(v3, weights2)), std::invalid_argument);

    std::vector<char> v4_part1 {'A', 'B', 'C', 'D'};
    std::vector<char> v4_part2 {'E', 'F'};
    ASSERT_THROW (GraphFactory().Create("complete", std::make_unique<Bipartite_Opts> (v4_part1, v4_part2)), std::invalid_argument);

    auto b4 = dynamic_cast<Bipartite*>(b1.get());
    auto b5 = dynamic_cast<Bipartite*>(b2.get());
    auto b6 = dynamic_cast<Bipartite*>(b3.get());
    ASSERT_THROW(*b5 + *b6, std::logic_error);
    auto complete2 = dynamic_cast<Complete*>(complete.get());
    auto weight2 = dynamic_cast<Weighted*>(weighted.get());
    ASSERT_THROW(*b5 + *weight2, std::logic_error);
    auto simple2 = dynamic_cast<Simple*>(simple.get());
    ASSERT_EQ ("SimpleGraph {AD, AE, BD, BE, CD, CE, MD, ME, EF, FA, AB}", (*b4 + *simple2).ToString());
    ASSERT_EQ ("BipartiteGraph {{A, B, C, M, K, N}, {D, E, F}}", (*b4 + *b5).ToString());
    *b4 += *b5;
    ASSERT_EQ ("BipartiteGraph {{A, B, C, M, K, N}, {D, E, F}}", b4->ToString());
    ASSERT_EQ ("BipartiteGraph {{A, C}, {D}}", (*b4 - *b5).ToString());
    ASSERT_EQ ("SimpleGraph {AK, AF, BK, BF, MK, MF}", (*b6 - *complete2).ToString());
    ASSERT_EQ ("SimpleGraph {AD, AE, BD, BE, BF, CD, CF, MD, ME, MF, KD, KE, KF, ND, NE, NF}", (*b4 - *simple2).ToString());
    ASSERT_EQ ("SimpleGraph {AK, AF, BK, MK, MF}", (*b6 - *weight2).ToString());
}

TEST(TestComplete, AllIncluded)
{	
    std::vector<char> v1 {'A','B','M', 'N', 'D'};
    std::unique_ptr<TGraph> complete1 = GraphFactory().Create("complete", std::make_unique<Complete_Opts> (v1));
    ASSERT_EQ (std::vector<char>({'A','B','M', 'N', 'D'}), complete1->GetVertices());
    std::vector<std::pair<char, char>> res1 {{'A', 'B'}, {'A', 'M'}, {'A', 'N'}, {'A', 'D'}, {'B', 'M'}, 
                                        {'B', 'N'}, {'B', 'D'}, {'M', 'N'}, {'M', 'D'}, {'N', 'D'}};                            
    ASSERT_EQ (res1, complete1->GetEdges());
    std::vector<char> v2 {'A', 'N', 'F'};
    std::unique_ptr<TGraph> complete2 = GraphFactory().Create("complete", std::make_unique<Complete_Opts> (v2));

    std::vector<std::string> edges1{{"AB"}, {"BF"}, {"FD"}, {"AE"}, {"ED"}};
    std::vector<int> weights1 {10, 12, 3, 15, 20};
    std::unique_ptr<TGraph> weighted = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(edges1, weights1));

    std::vector<std::string> v{{"EF"}, {"FA"}, {"AB"}, {"EC"}};
    std::unique_ptr<TGraph> simple = GraphFactory().Create("simple", std::make_unique<Simple_Opts>(v));

    std::vector<char> b1_part1 {'A', 'B', 'C', 'N'};
    std::vector<char> b1_part2 {'D', 'E'};
    std::unique_ptr<TGraph> bipart = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b1_part1, b1_part2));

    std::vector<std::pair<char, char>> res2 {{'A', 'N'}, {'A', 'F'}, {'N', 'F'}};
    ASSERT_EQ (res2, complete2->GetEdges());
    ASSERT_EQ ("WeightedGraph {AB:1, AM:1, AN:1, AD:1, BM:1, BN:1, BD:1, MN:1, MD:1, ND:1}", complete1->AsWeighted(1)->ToString());

    auto c1 = dynamic_cast<Complete*>(complete1.get());
    auto c2 = dynamic_cast<Complete*>(complete2.get());
    ASSERT_EQ ("CompleteGraph {A, B, M, N, D, F}", (*c1 + *c2).ToString());

    auto b = dynamic_cast<Bipartite*>(bipart.get());
    ASSERT_EQ ("SimpleGraph {AB, AM, AN, AD, BM, BN, BD, MN, MD, ND, AE, BE, CD, CE, NE}", (*c1 + *b).ToString());

    auto s = dynamic_cast<Simple*>(simple.get());
    ASSERT_EQ ("SimpleGraph {AN, AF, NF, EF, AB, EC}", (*c2 + *s).ToString());

    auto w = dynamic_cast<Weighted*>(weighted.get());
    ASSERT_THROW (*c2 + *w, std::logic_error);
    *c1 += *c2;
    ASSERT_EQ ("CompleteGraph {A, B, M, N, D, F}", c1->ToString());
    *c1 -= *c2;
    ASSERT_EQ ("CompleteGraph {B, M, D}", c1->ToString());
}

TEST(TestSimple, AllIncluded)
{
    std::vector<std::string> vec1 {{"EF"}, {"FA"}, {"AB"}, {"EC"}};
    std::unique_ptr<TGraph> simple1 = GraphFactory().Create("simple", std::make_unique<Simple_Opts>(vec1));
    ASSERT_EQ ("WeightedGraph {EF:7, FA:7, AB:7, EC:7}", simple1->AsWeighted(7)->ToString());
    ASSERT_EQ ("SimpleGraph {EF, FA, AB, EC}", simple1->ToString());
    ASSERT_EQ (std::vector<char>({'A','B','C', 'E', 'F'}), simple1->GetVertices());
    ASSERT_EQ ("SimpleGraph {EF, FA, AB, EC}", simple1->ToString());
    std::vector<std::pair<char, char>> res1 {{'E', 'F'}, {'F', 'A'}, {'A', 'B'}, {'E', 'C'}};
    ASSERT_EQ (res1, simple1->GetEdges());
    std::vector<std::string> vec2 = {{"EF"}, {"FC"}, {"AE"}};
    std::unique_ptr<TGraph> simple2 = GraphFactory().Create("simple", std::make_unique<Simple_Opts>(vec2));

    std::vector<char> v {'A', 'N', 'F'};
    std::unique_ptr<TGraph> complete = GraphFactory().Create("complete", std::make_unique<Complete_Opts> (v));

    std::vector<std::string> edges1 {{"AB"}, {"BF"}, {"FD"}, {"AE"}, {"ED"}};
    std::vector<int> weights1 {10, 12, 3, 15, 20};
    std::unique_ptr<TGraph> weighted = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(edges1, weights1));

    std::vector<char> b1_part1 {'A', 'B', 'C', 'N'};
    std::vector<char> b1_part2 {'D', 'E'};
    std::unique_ptr<TGraph> bipart = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b1_part1, b1_part2));

    auto s1 = dynamic_cast<Simple*>(simple1.get());
    auto s2 = dynamic_cast<Simple*>(simple2.get());
    ASSERT_EQ ("SimpleGraph {EF, FA, AB, EC, FC, AE}", (*s1 + *s2).ToString());

    auto b = dynamic_cast<Bipartite*>(bipart.get());
    ASSERT_EQ ("SimpleGraph {EF, FA, AB, EC, AD, AE, BD, BE, CD, ND, NE}", (*s1 + *b).ToString());

    auto c = dynamic_cast<Complete*>(complete.get());
    ASSERT_EQ ("SimpleGraph {EF, FC, AE, AN, AF, NF}", (*s2 + *c).ToString());

    auto w = dynamic_cast<Weighted*>(weighted.get());
    ASSERT_THROW (*s1 + *w, std::logic_error);
    *s2 += *s1;

    ASSERT_EQ ("SimpleGraph {EF, FC, AE, FA, AB, EC}", s2->ToString());
    ASSERT_EQ ("SimpleGraph {}", (*s1 - *s2).ToString());
    ASSERT_EQ ("SimpleGraph {EF, FC, FA, AB}", (*s2 - *b).ToString());
    ASSERT_EQ ("SimpleGraph {EF, AB, EC}", (*s1 - *c).ToString());
    ASSERT_EQ ("SimpleGraph {EF, FA, EC}", (*s1 - *w).ToString());
    *s1 -= *s2;
    ASSERT_EQ ("SimpleGraph {}", s1->ToString());
}


TEST(TestWeighted, AllIncluded)
{
    std::vector<std::string> edges1 {{"AB"}, {"BF"}, {"FD"}, {"AE"}, {"ED"}};
    std::vector<int> weights1 {10, 12, 3, 15, 20};
    std::unique_ptr<TGraph> weighted1 = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(edges1, weights1));

    std::vector<std::string> edges2 {{"AB"}, {"AD"}, {"BF"}};
    std::vector<int> weights2 {10,12, 15};
    std::unique_ptr<TGraph> weighted2 = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(edges2, weights2));

    std::vector<std::string> vec2{{"EF"}, {"FC"}, {"AE"}};
    std::unique_ptr<TGraph> simple = GraphFactory().Create("simple", std::make_unique<Simple_Opts>(vec2));

    std::vector<char> v {'A', 'N', 'F'};
    std::unique_ptr<TGraph> complete = GraphFactory().Create("complete", std::make_unique<Complete_Opts> (v));

    std::vector<char> b1_part1 {'A', 'B', 'C', 'N'};
    std::vector<char> b1_part2 {'D', 'E'};
    std::unique_ptr<TGraph> bipart = GraphFactory().Create(std::string("bipartite"), std::make_unique<Bipartite_Opts> (b1_part1, b1_part2));

    ASSERT_EQ (std::vector<char>({'A','B', 'D', 'E', 'F'}), weighted1->GetVertices());
    ASSERT_EQ (std::vector<char>({'A', 'B', 'D', 'F'}), weighted2->GetVertices());
    std::vector<std::pair<char, char>> res1 {{'A', 'B'}, {'B', 'F'}, {'F', 'D'}, {'A', 'E'}, {'E', 'D'}};
    ASSERT_EQ (res1, weighted1->GetEdges());
    std::vector<std::pair<char, char>> res2 {{'A', 'B'}, {'A', 'D'}, {'B', 'F'}};
    ASSERT_EQ (res2, weighted2->GetEdges());
    ASSERT_EQ ("WeightedGraph {AB:10, BF:12, FD:3, AE:15, ED:20}", weighted1->ToString());
    ASSERT_EQ ("WeightedGraph {AB:10, AD:12, BF:15}", weighted2->ToString());

    auto s = dynamic_cast<Simple*>(simple.get());
    auto b = dynamic_cast<Bipartite*>(bipart.get());
    auto c = dynamic_cast<Complete*>(complete.get());
    auto w1 = dynamic_cast<Weighted*>(weighted1.get());
    auto w2 = dynamic_cast<Weighted*>(weighted2.get());
    ASSERT_EQ ("WeightedGraph {AB:10, BF:12, FD:3, AE:15, ED:20, AD:12}", (*w1 + *w2).ToString());
    ASSERT_THROW (*w1 + *s, std::logic_error);
    ASSERT_THROW (*w1 + *b, std::logic_error);
    ASSERT_THROW (*w2 + *c, std::logic_error);
    *w1 += *w2;
    ASSERT_EQ ("WeightedGraph {AB:10, BF:12, FD:3, AE:15, ED:20, AD:12}", w1->ToString());
    ASSERT_EQ ("WeightedGraph {FD:3, AE:15, ED:20}", (*w1 - *w2).ToString());
    ASSERT_EQ ("WeightedGraph {AB:10, BF:15}", (*w2 - *b).ToString());
    ASSERT_EQ ("WeightedGraph {AB:10, BF:12, FD:3, AE:15, ED:20, AD:12}", (*w1 - *c).ToString());
    ASSERT_EQ ("WeightedGraph {AB:10, BF:12, FD:3, ED:20, AD:12}", (*w1 - *s).ToString());
    *w1 -= *w2;
    ASSERT_EQ ("WeightedGraph {FD:3, AE:15, ED:20}", w1->ToString());
}

TEST(PathFunction, AllCases)
{
    std::vector<std::string> v1 {{"AB"}, {"AC"}, {"AF"}, {"BC"}, {"CF"}, {"FE"}, {"BD"}, {"ED"}};
    std::vector<int> w1 {7, 9, 14, 10, 2, 9, 11, 15};
    auto weight1 = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(v1, w1));
    auto weighted1 = dynamic_cast<Weighted*>(weight1.get());
    std::vector<std::pair<char, char>> res1 {{'A', 'C'}, {'C', 'F'}, {'F', 'E'}};
    ASSERT_EQ (res1, find_min_path(*weighted1, 'A', 'E'));

    std::vector<std::string> v2 {"AB", "AC", "BC", "CE", "BD", "BE", "CD", "DE", "DG", "EG"};
    std::vector<int> w2 {2, 1, 2, 6, 4, 5, 4, 2, 6, 3};
    auto weight2 = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(v2, w2));
    auto weighted2 = dynamic_cast<Weighted*>(weight2.get());
    std::vector<std::pair<char, char>> res2 {{'B', 'E'}};
    ASSERT_EQ (res2, find_min_path(*weighted2, 'B', 'E'));

    std::vector<std::string> v3 {{"AB"}, {"AC"}, {"AF"}, {"BC"}, {"CF"}, {"XY"}};
    std::vector<int> w3 {1, 2, 3, 4, 5, 6};
    auto weight3 = GraphFactory().Create("weighted", std::make_unique<Weighted_Opts>(v3, w3));
    auto weighted3 = dynamic_cast<Weighted*>(weight3.get());
    ASSERT_THROW(find_min_path(*weighted3, 'A', 'X'), std::logic_error);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}