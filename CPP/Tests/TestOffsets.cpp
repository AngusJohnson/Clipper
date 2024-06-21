#include <gtest/gtest.h>
#include "clipper2/clipper.offset.h"
#include "ClipFileLoad.h"

using namespace Clipper2Lib;

TEST(Clipper2Tests, TestOffsets) {
  std::ifstream ifs("Offsets.txt");
  ASSERT_TRUE(ifs.good());
  for (int test_number = 1; test_number <= 2; ++test_number)
  {
    ClipperOffset co;
    PathsI subject, subject_open, clip;
    PathsI solution, solution_open;
    ClipType ct = ClipType::None;
    FillRule fr = FillRule::NonZero;
    Integer stored_area = 0, stored_count = 0;
    ASSERT_TRUE(LoadTestNum(ifs, test_number, subject, subject_open, clip, stored_area, stored_count, ct, fr));
    co.AddPaths(subject, JoinType::Round, EndType::Polygon);
    PathsI outputs;
    co.Execute(1, outputs);
    // is the sum total area of the solution is positive
    const auto outer_is_positive = Area(outputs) > 0;
    // there should be exactly one exterior path
    const auto is_positive_func = IsPositive<Integer>;
    const auto is_positive_count = std::count_if(
      outputs.begin(), outputs.end(), is_positive_func);
    const auto is_negative_count =
      outputs.size() - is_positive_count;
    if (outer_is_positive)
      EXPECT_EQ(is_positive_count, 1);
    else
      EXPECT_EQ(is_negative_count, 1);
  }
  ifs.close();
}
static PointI MidPoint(const PointI& p1, const PointI& p2)
{
  PointI result;
  result.x = (p1.x + p2.x) / 2;
  result.y = (p1.y + p2.y) / 2;
  return result;
}
TEST(Clipper2Tests, TestOffsets2) { // see #448 & #456
  Scalar scale = 10, delta = 10 * scale, arc_tol = 0.25 * scale;
  PathsI subject, solution;
  ClipperOffset c;
  subject.push_back(MakePath({ 50,50, 100,50, 100,150, 50,150, 0,100 }));
  int err;
  subject = ScalePaths<Integer, Integer>(subject, scale, err);
  c.AddPaths(subject, JoinType::Round, EndType::Polygon);
  c.ArcTolerance(arc_tol);
  c.Execute(delta, solution);
  Scalar min_dist = delta * 2, max_dist = 0;
  for (const PointI& subjPt : subject[0])
  {
    PointI prevPt = solution[0][solution[0].size() - 1];
    for (const PointI& pt : solution[0])
    {
      PointI mp = MidPoint(prevPt, pt);
      Scalar d = Distance(mp, subjPt);
      if (d < delta * 2)
      {
        if (d < min_dist) min_dist = d;
        if (d> max_dist) max_dist = d;
      }
      prevPt = pt;
    }
  }
  EXPECT_GE(min_dist + 1, delta - arc_tol); // +1 for rounding errors
  EXPECT_LE(solution[0].size(), 21);
}

TEST(Clipper2Tests, TestOffsets3) // see #424
{
  PathsI subjects = {{
   {1525311078, 1352369439}, {1526632284, 1366692987}, {1519397110, 1367437476},
   {1520246456, 1380177674}, {1520613458, 1385913385}, {1517383844, 1386238444},
   {1517771817, 1392099983}, {1518233190, 1398758441}, {1518421934, 1401883197},
   {1518694564, 1406612275}, {1520267428, 1430289121}, {1520770744, 1438027612},
   {1521148232, 1443438264}, {1521441833, 1448964260}, {1521683005, 1452518932},
   {1521819320, 1454374912}, {1527943004, 1454154711}, {1527649403, 1448523858},
   {1535901696, 1447989084}, {1535524209, 1442788147}, {1538953052, 1442463089},
   {1541553521, 1442242888}, {1541459149, 1438855987}, {1538764308, 1439076188},
   {1538575565, 1436832236}, {1538764308, 1436832236}, {1536509870, 1405374956},
   {1550497874, 1404347351}, {1550214758, 1402428457}, {1543818445, 1402868859},
   {1543734559, 1402124370}, {1540672717, 1402344571}, {1540473487, 1399995761},
   {1524996506, 1400981422}, {1524807762, 1398223667}, {1530092585, 1397898609},
   {1531675935, 1397783265}, {1531392819, 1394920653}, {1529809469, 1395025510},
   {1529348096, 1388880855}, {1531099218, 1388660654}, {1530826588, 1385158410},
   {1532955197, 1384938209}, {1532661596, 1379003269}, {1532472852, 1376235028},
   {1531277476, 1376350372}, {1530050642, 1361806623}, {1599487345, 1352704983},
   {1602758902, 1378489467}, {1618990858, 1376350372}, {1615058698, 1344085688},
   {1603230761, 1345700495}, {1598648484, 1346329641}, {1598931599, 1348667965},
   {1596698132, 1348993024}, {1595775386, 1342722540} }};
  PathsI solution = InflatePaths(subjects, -209715, JoinType::Miter, EndType::Polygon);
  EXPECT_LE(solution[0].size() - subjects[0].size(), 1);
}

TEST(Clipper2Tests, TestOffsets4) // see #482
{
  PathsI paths = { { {0, 0}, {20000, 200},
    {40000, 0}, {40000, 50000}, {0, 50000}, {0, 0}} };
  PathsI solution = InflatePaths(paths, -5000,
    JoinType::Square, EndType::Polygon);
  //std::cout << solution[0].size() << std::endl;
  EXPECT_EQ(solution[0].size(), 5);
  paths = { { {0, 0}, {20000, 400},
    {40000, 0}, {40000, 50000}, {0, 50000}, {0, 0}} };
  solution = InflatePaths(paths, -5000,
    JoinType::Square, EndType::Polygon);
  //std::cout << solution[0].size() << std::endl;
  EXPECT_EQ(solution[0].size(), 5);
  paths = { { {0, 0}, {20000, 400},
    {40000, 0}, {40000, 50000}, {0, 50000}, {0, 0}} };
  solution = InflatePaths(paths, -5000,
    JoinType::Round, EndType::Polygon, 2, 100);
  //std::cout << solution[0].size() << std::endl;
  EXPECT_GT(solution[0].size(), 5);
  paths = { { {0, 0}, {20000, 1500},
    {40000, 0}, {40000, 50000}, {0, 50000}, {0, 0}} };
  solution = InflatePaths(paths, -5000,
    JoinType::Round, EndType::Polygon, 2, 100);
  //std::cout << solution[0].size() << std::endl;
  EXPECT_GT(solution[0].size(), 5);
}

TEST(Clipper2Tests, TestOffsets5) // modified from #593 (tests offset clean up)
{
  PathsI subject = {
    MakePath({
      524,1483, 524,2711, 610,2744, 693,2782, 773,2825, 852,2872,
      927,2924, 999,2980, 1068,3040, 1133,3103, 1195,3171, 1252,3242,
      1305,3316, 1354,3394, 1398,3473, 1437,3556, 1472,3640, 1502,3727,
      1526,3815, 1546,3904, 1560,3994, 1569,4085, 1573,4176, 1571,4267,
      1564,4358, 1552,4449, 1535,4539, 1512,4627, 1485,4714, 1452,4799,
      1414,4883, 1372,4964, 1325,5042, 1274,5117, 1218,5190, 1158,5259,
      1094,5324, 1027,5386, 956,5443, 882,5497, 805,5546, 725,5590,
      643,5630, 559,5665, 524,5677, 524,6906, 610,6939, 693,6977,
      773,7019, 852,7066, 927,7118, 999,7174, 1068,7234, 1133,7298,
      1195,7365, 1252,7436, 1305,7511, 1354,7588, 1398,7668, 1437,7750,
      1472,7835, 1502,7921, 1526,8009, 1546,8098, 1560,8188, 1569,8279,
      1573,8370, 1571,8462, 1564,8553, 1552,8643, 1535,8733, 1512,8821,
      1485,8908, 1452,8994, 1414,9077, 1372,9158, 1325,9236, 1274,9312,
      1218,9384, 1158,9453, 1094,9518, 1027,9580, 956,9638, 882,9691,
      805,9740, 725,9784, 643,9824, 559,9859, 524,9872, 524,11100,
      610,11133, 693,11171, 773,11213, 852,11261, 927,11312, 999,11368,
      1068,11428, 1133,11492, 1195,11560, 1252,11631, 1305,11705, 1354,11782,
      1398,11862, 1437,11945, 1472,12029, 1502,12115, 1526,12203, 1546,12293,
      1560,12383, 1569,12474, 1573,12565, 1571,12656, 1564,12747, 1552,12838,
      1535,12927, 1512,13016, 1485,13103, 1452,13188, 1414,13271, 1372,13352,
      1325,13431, 1274,13506, 1218,13578, 1158,13647, 1094,13713, 1027,13774,
      956,13832, 882,13885, 805,13934, 725,13979, 643,14019, 559,14053,
      524,14066, 524,15295, 610,15327, 693,15365, 773,15408, 852,15455,
      927,15507, 999,15563, 1068,15623, 1133,15687, 1195,15754, 1252,15825,
      1305,15899, 1354,15977, 1398,16057, 1437,16139, 1472,16223, 1502,16310,
      1526,16398, 1546,16487, 1560,16577, 1569,16668, 1573,16759, 1571,16850,
      1564,16942, 1552,17032, 1535,17122, 1512,17210, 1485,17297, 1452,17382,
      1414,17466, 1372,17547, 1325,17625, 1274,17700, 1218,17773, 1158,17842,
      1094,17907, 1027,17969, 956,18026, 882,18080, 805,18129, 725,18173,
      643,18213, 559,18248, 524,18260, 524,19489, 610,19522, 693,19560,
      773,19602, 852,19649, 927,19701, 999,19757, 1068,19817, 1133,19881,
      1195,19948, 1252,20019, 1305,20094, 1354,20171, 1398,20251, 1437,20333,
      1472,20418, 1502,20504, 1526,20592, 1546,20681, 1560,20771, 1569,20862,
      1573,20954, 1571,21045, 1564,21136, 1552,21226, 1535,21316, 1512,21404,
      1485,21492, 1452,21577, 1414,21660, 1372,21741, 1325,21819, 1274,21895,
      1218,21967, 1158,22036, 1094,22101, 1027,22163, 956,22221, 882,22274,
      805,22323, 725,22368, 643,22407, 559,22442, 524,22455, 524,23683,
      610,23716, 693,23754, 773,23797, 852,23844, 927,23895, 999,23951,
      1068,24011, 1133,24075, 1195,24143, 1252,24214, 1305,24288, 1354,24365,
      1398,24445, 1437,24528, 1472,24612, 1502,24698, 1526,24786, 1546,24876,
      1560,24966, 1569,25057, 1573,25148, 1571,25239, 1564,25330, 1552,25421,
      1535,25510, 1512,25599, 1485,25686, 1452,25771, 1414,25854, 1372,25935,
      1325,26014, 1274,26089, 1218,26161, 1158,26230, 1094,26296, 1027,26357,
      956,26415, 882,26468, 805,26517, 725,26562, 643,26602, 559,26636,
      524,26649, 524,27878, 610,27910, 693,27948, 773,27991, 852,28038,
      927,28090, 999,28146, 1068,28206, 1133,28270, 1195,28337, 1252,28408,
      1305,28482, 1354,28560, 1398,28640, 1437,28722, 1472,28806, 1502,28893,
      1526,28981, 1546,29070, 1560,29160, 1569,29251, 1573,29342, 1571,29434,
      1564,29525, 1552,29615, 1535,29705, 1512,29793, 1485,29880, 1452,29965,
      1414,30049, 1372,30130, 1325,30208, 1274,30283, 1218,30356, 1158,30425,
      1094,30490, 1027,30552, 956,30609, 882,30663, 805,30712, 725,30756,
      643,30796, 559,30831, 524,30843, 524,32072, 609,32105, 692,32142,
      773,32185, 851,32232, 926,32283, 998,32339, 1066,32398, 1131,32462,
      1193,32529, 1250,32600, 1303,32674, 1352,32751, 1396,32830, 1436,32912,
      1470,32996, 1483,33031, 3131,33031, 3164,32945, 3202,32862, 3244,32781,
      3291,32703, 3343,32628, 3399,32556, 3459,32487, 3523,32422, 3591,32360,
      3662,32303, 3736,32250, 3813,32201, 3893,32157, 3975,32117, 4060,32083,
      4146,32053, 4234,32028, 4323,32009, 4413,31995, 4504,31986, 4596,31982,
      4687,31984, 4778,31991, 4868,32003, 4958,32020, 5047,32043, 5134,32070,
      5219,32103, 5302,32141, 5383,32183, 5461,32230, 5537,32281, 5609,32337,
      5678,32397, 5744,32460, 5805,32528, 5863,32599, 5916,32673, 5965,32750,
      6010,32830, 6049,32912, 6084,32996, 6097,33031, 7745,33031, 7778,32945,
      7815,32862, 7858,32781, 7905,32703, 7957,32628, 8013,32556, 8073,32487,
      8137,32422, 8204,32360, 8275,32303, 8350,32250, 8427,32201, 8507,32157,
      8589,32117, 8674,32083, 8760,32053, 8848,32028, 8937,32009, 9027,31995,
      9118,31986, 9209,31982, 9301,31984, 9392,31991, 9482,32003, 9572,32020,
      9660,32043, 9747,32070, 9833,32103, 9916,32141, 9997,32183, 10075,32230,
      10151,32281, 10223,32337, 10292,32397, 10357,32460, 10419,32528, 10477,32599,
      10530,32673, 10579,32750, 10623,32830, 10663,32912, 10698,32996, 10711,33031,
      12358,33031, 12391,32945, 12429,32862, 12472,32781, 12519,32703, 12571,32628,
      12627,32556, 12687,32487, 12750,32422, 12818,32360, 12889,32303, 12963,32250,
      13041,32201, 13120,32157, 13203,32117, 13287,32083, 13374,32053, 13462,32028,
      13551,32009, 13641,31995, 13732,31986, 13823,31982, 13914,31984, 14005,31991,
      14096,32003, 14186,32020, 14274,32043, 14361,32070, 14446,32103, 14530,32141,
      14611,32183, 14689,32230, 14764,32281, 14837,32337, 14906,32397, 14971,32460,
      15033,32528, 15090,32599, 15144,32673, 15193,32750, 15237,32830, 15277,32912,
      15312,32996, 15324,33031, 16972,33031, 17005,32945, 17043,32862, 17086,32781,
      17133,32703, 17184,32628, 17240,32556, 17300,32487, 17364,32422, 17432,32360,
      17503,32303, 17577,32250, 17654,32201, 17734,32157, 17817,32117, 17901,32083,
      17987,32053, 18075,32028, 18165,32009, 18255,31995, 18346,31986, 18437,31982,
      18528,31984, 18619,31991, 18710,32003, 18799,32020, 18888,32043, 18975,32070,
      19060,32103, 19143,32141, 19224,32183, 19303,32230, 19378,32281, 19450,32337,
      19519,32397, 19585,32460, 19646,32528, 19704,32599, 19757,32673, 19806,32750,
      19851,32830, 19891,32912, 19926,32996, 19938,33031, 21586,33031, 21619,32945,
      21657,32862, 21699,32781, 21747,32703, 21798,32628, 21854,32556, 21914,32487,
      21978,32422, 22046,32360, 22117,32303, 22191,32250, 22268,32201, 22348,32157,
      22430,32117, 22515,32083, 22601,32053, 22689,32028, 22778,32009, 22869,31995,
      22959,31986, 23051,31982, 23142,31984, 23233,31991, 23324,32003, 23413,32020,
      23502,32043, 23589,32070, 23674,32103, 23757,32141, 23838,32183, 23916,32230,
      23992,32281, 24064,32337, 24133,32397, 24199,32460, 24260,32528, 24318,32599,
      24371,32673, 24420,32750, 24465,32830, 24504,32912, 24539,32996, 24552,33031,
      26200,33031, 26233,32945, 26271,32862, 26313,32781, 26360,32703, 26412,32628,
      26468,32556, 26528,32487, 26592,32422, 26659,32360, 26730,32303, 26805,32250,
      26882,32201, 26962,32157, 27044,32117, 27129,32083, 27215,32053, 27303,32028,
      27392,32009, 27482,31995, 27573,31986, 27664,31982, 27756,31984, 27847,31991,
      27937,32003, 28027,32020, 28115,32043, 28202,32070, 28288,32103, 28371,32141,
      28452,32183, 28530,32230, 28606,32281, 28678,32337, 28747,32397, 28812,32460,
      28874,32528, 28932,32599, 28985,32673, 29034,32750, 29078,32830, 29118,32912,
      29153,32996, 29166,33031, 30814,33031, 30847,32945, 30884,32862, 30927,32781,
      30974,32703, 31026,32628, 31082,32556, 31142,32487, 31206,32422, 31273,32360,
      31344,32303, 31418,32250, 31496,32201, 31576,32157, 31658,32117, 31742,32083,
      31829,32053, 31917,32028, 32006,32009, 32096,31995, 32187,31986, 32278,31982,
      32370,31984, 32461,31991, 32551,32003, 32641,32020, 32729,32043, 32816,32070,
      32902,32103, 32985,32141, 33066,32183, 33144,32230, 33219,32281, 33292,32337,
      33361,32397, 33426,32460, 33488,32528, 33545,32599, 33599,32673, 33648,32750,
      33692,32830, 33732,32912, 33767,32996, 33779,33031, 35427,33031, 35460,32946,
      35498,32863, 35540,32782, 35587,32704, 35639,32629, 35694,32557, 35754,32489,
      35818,32423, 35885,32362, 35956,32305, 36029,32252, 36106,32203, 36186,32159,
      36268,32119, 36352,32084, 36386,32072, 36386,30843, 36301,30810, 36218,30773,
      36137,30730, 36059,30683, 35983,30631, 35911,30575, 35842,30515, 35777,30451,
      35716,30384, 35658,30313, 35605,30239, 35557,30161, 35512,30081, 35473,29999,
      35438,29915, 35409,29828, 35384,29740, 35364,29651, 35350,29561, 35341,29470,
      35338,29379, 35339,29287, 35346,29196, 35358,29106, 35376,29016, 35398,28928,
      35426,28841, 35458,28755, 35496,28672, 35538,28591, 35585,28513, 35637,28438,
      35692,28365, 35752,28296, 35816,28231, 35883,28169, 35954,28112, 36028,28058,
      36105,28009, 36185,27965, 36267,27925, 36352,27890, 36386,27878, 36386,26649,
      36301,26616, 36218,26578, 36137,26536, 36059,26489, 35983,26437, 35911,26381,
      35842,26321, 35777,26257, 35716,26189, 35658,26118, 35605,26044, 35557,25967,
      35512,25887, 35473,25805, 35438,25720, 35409,25634, 35384,25546, 35364,25457,
      35350,25366, 35341,25276, 35338,25184, 35339,25093, 35346,25002, 35358,24912,
      35376,24822, 35398,24733, 35426,24646, 35458,24561, 35496,24478, 35538,24397,
      35585,24319, 35637,24243, 35692,24171, 35752,24102, 35816,24036, 35883,23975,
      35954,23917, 36028,23864, 36105,23815, 36185,23770, 36267,23731, 36352,23696,
      36386,23683, 36386,22455, 36301,22422, 36218,22384, 36137,22341, 36059,22294,
      35983,22243, 35911,22187, 35842,22127, 35777,22063, 35716,21995, 35658,21924,
      35605,21850, 35557,21773, 35512,21693, 35473,21610, 35438,21526, 35409,21440,
      35384,21352, 35364,21262, 35350,21172, 35341,21081, 35338,20990, 35339,20899,
      35346,20808, 35358,20717, 35376,20628, 35398,20539, 35426,20452, 35458,20367,
      35496,20284, 35538,20203, 35585,20124, 35637,20049, 35692,19976, 35752,19907,
      35816,19842, 35883,19780, 35954,19723, 36028,19669, 36105,19620, 36185,19576,
      36267,19536, 36352,19501, 36386,19489, 36386,18260, 36301,18227, 36218,18190,
      36137,18147, 36059,18100, 35983,18048, 35911,17992, 35842,17932, 35777,17868,
      35716,17801, 35658,17730, 35605,17655, 35557,17578, 35512,17498, 35473,17416,
      35438,17332, 35409,17245, 35384,17157, 35364,17068, 35350,16978, 35341,16887,
      35338,16796, 35339,16704, 35346,16613, 35358,16523, 35376,16433, 35398,16345,
      35426,16258, 35458,16172, 35496,16089, 35538,16008, 35585,15930, 35637,15854,
      35692,15782, 35752,15713, 35816,15648, 35883,15586, 35954,15529, 36028,15475,
      36105,15426, 36185,15382, 36267,15342, 36352,15307, 36386,15295, 36386,14066,
      36301,14033, 36218,13995, 36137,13953, 36059,13906, 35983,13854, 35911,13798,
      35842,13738, 35777,13674, 35716,13606, 35658,13535, 35605,13461, 35557,13384,
      35512,13304, 35473,13222, 35438,13137, 35409,13051, 35384,12963, 35364,12874,
      35350,12783, 35341,12693, 35338,12601, 35339,12510, 35346,12419, 35358,12328,
      35376,12239, 35398,12150, 35426,12063, 35458,11978, 35496,11895, 35538,11814,
      35585,11736, 35637,11660, 35692,11588, 35752,11519, 35816,11453, 35883,11392,
      35954,11334, 36028,11281, 36105,11232, 36185,11187, 36267,11148, 36352,11113,
      36386,11100, 36386,9872, 36301,9839, 36218,9801, 36137,9758, 36059,9711,
      35983,9660, 35911,9604, 35842,9544, 35777,9480, 35716,9412, 35658,9341,
      35605,9267, 35557,9190, 35512,9110, 35473,9027, 35438,8943, 35409,8856,
      35384,8769, 35364,8679, 35350,8589, 35341,8498, 35338,8407, 35339,8316,
      35346,8225, 35358,8134, 35376,8045, 35398,7956, 35426,7869, 35458,7784,
      35496,7700, 35538,7620, 35585,7541, 35637,7466, 35692,7393, 35752,7324,
      35816,7259, 35883,7197, 35954,7140, 36028,7086, 36105,7037, 36185,6993,
      36267,6953, 36352,6918, 36386,6906, 36386,5677, 36301,5644, 36218,5607,
      36137,5564, 36059,5517, 35983,5465, 35911,5409, 35842,5349, 35777,5285,
      35716,5218, 35658,5147, 35605,5072, 35557,4995, 35512,4915, 35473,4833,
      35438,4748, 35409,4662, 35384,4574, 35364,4485, 35350,4395, 35341,4304,
      35338,4213, 35339,4121, 35346,4030, 35358,3940, 35376,3850, 35398,3762,
      35426,3675, 35458,3589, 35496,3506, 35538,3425, 35585,3347, 35637,3271,
      35692,3199, 35752,3130, 35816,3065, 35883,3003, 35954,2945, 36028,2892,
      36105,2843, 36185,2799, 36267,2759, 36352,2724, 36386,2711, 36386,1483,
      36301,1450, 36218,1413, 36138,1370, 36060,1323, 35985,1272, 35913,1216,
      35844,1156, 35779,1093, 35718,1026, 35660,955, 35607,881, 35558,804,
      35514,725, 35475,643, 35440,559, 35427,524, 33779,524, 33747,610,
      33709,693, 33666,773, 33619,852, 33567,927, 33511,999, 33451,1068,
      33387,1133, 33320,1195, 33249,1252, 33175,1305, 33097,1354, 33017,1398,
      32935,1437, 32851,1472, 32764,1502, 32676,1526, 32587,1546, 32497,1560,
      32406,1569, 32315,1573, 32223,1571, 32132,1564, 32042,1552, 31952,1535,
      31864,1512, 31777,1485, 31691,1452, 31608,1414, 31527,1372, 31449,1325,
      31374,1274, 31301,1218, 31232,1158, 31167,1094, 31105,1027, 31048,956,
      30994,882, 30945,805, 30901,725, 30861,643, 30826,559, 30814,524,
      29166,524, 29133,610, 29095,693, 29052,773, 29005,852, 28954,927,
      28898,999, 28838,1068, 28774,1133, 28706,1195, 28635,1252, 28561,1305,
      28484,1354, 28404,1398, 28321,1437, 28237,1472, 28150,1502, 28063,1526,
      27973,1546, 27883,1560, 27792,1569, 27701,1573, 27610,1571, 27519,1564,
      27428,1552, 27338,1535, 27250,1512, 27163,1485, 27078,1452, 26994,1414,
      26914,1372, 26835,1325, 26760,1274, 26687,1218, 26618,1158, 26553,1094,
      26491,1027, 26434,956, 26380,882, 26331,805, 26287,725, 26247,643,
      26212,559, 26200,524, 24552,524, 24519,610, 24481,693, 24439,773,
      24391,852, 24340,927, 24284,999, 24224,1068, 24160,1133, 24092,1195,
      24021,1252, 23947,1305, 23870,1354, 23790,1398, 23708,1437, 23623,1472,
      23537,1502, 23449,1526, 23360,1546, 23269,1560, 23178,1569, 23087,1573,
      22996,1571, 22905,1564, 22814,1552, 22725,1535, 22636,1512, 22549,1485,
      22464,1452, 22381,1414, 22300,1372, 22221,1325, 22146,1274, 22074,1218,
      22005,1158, 21939,1094, 21878,1027, 21820,956, 21767,882, 21718,805,
      21673,725, 21633,643, 21599,559, 21586,524, 19938,524, 19905,610,
      19867,693, 19825,773, 19778,852, 19726,927, 19670,999, 19610,1068,
      19546,1133, 19478,1195, 19407,1252, 19333,1305, 19256,1354, 19176,1398,
      19094,1437, 19009,1472, 18923,1502, 18835,1526, 18746,1546, 18656,1560,
      18565,1569, 18473,1573, 18382,1571, 18291,1564, 18201,1552, 18111,1535,
      18022,1512, 17935,1485, 17850,1452, 17767,1414, 17686,1372, 17608,1325,
      17532,1274, 17460,1218, 17391,1158, 17325,1094, 17264,1027, 17206,956,
      17153,882, 17104,805, 17059,725, 17020,643, 16985,559, 16972,524,
      15324,524, 15291,610, 15254,693, 15211,773, 15164,852, 15112,927,
      15056,999, 14996,1068, 14932,1133, 14865,1195, 14794,1252, 14719,1305,
      14642,1354, 14562,1398, 14480,1437, 14395,1472, 14309,1502, 14221,1526,
      14132,1546, 14042,1560, 13951,1569, 13860,1573, 13768,1571, 13677,1564,
      13587,1552, 13497,1535, 13409,1512, 13322,1485, 13236,1452, 13153,1414,
      13072,1372, 12994,1325, 12918,1274, 12846,1218, 12777,1158, 12712,1094,
      12650,1027, 12592,956, 12539,882, 12490,805, 12446,725, 12406,643,
      12371,559, 12358,524, 10711,524, 10678,610, 10640,693, 10597,773,
      10550,852, 10498,927, 10442,999, 10382,1068, 10319,1133, 10251,1195,
      10180,1252, 10106,1305, 10028,1354, 9949,1398, 9866,1437, 9782,1472,
      9695,1502, 9607,1526, 9518,1546, 9428,1560, 9337,1569, 9246,1573,
      9155,1571, 9064,1564, 8973,1552, 8883,1535, 8795,1512, 8708,1485,
      8623,1452, 8539,1414, 8458,1372, 8380,1325, 8305,1274, 8232,1218,
      8163,1158, 8098,1094, 8036,1027, 7979,956, 7925,882, 7876,805,
      7832,725, 7792,643, 7757,559, 7745,524, 6097,524, 6064,610,
      6026,693, 5983,773, 5936,852, 5885,927, 5829,999, 5769,1068,
      5705,1133, 5637,1195, 5566,1252, 5492,1305, 5415,1354, 5335,1398,
      5252,1437, 5168,1472, 5082,1502, 4994,1526, 4904,1546, 4814,1560,
      4723,1569, 4632,1573, 4541,1571, 4450,1564, 4359,1552, 4270,1535,
      4181,1512, 4094,1485, 4009,1452, 3926,1414, 3845,1372, 3766,1325,
      3691,1274, 3619,1218, 3550,1158, 3484,1094, 3423,1027, 3365,956,
      3312,882, 3263,805, 3218,725, 3178,643, 3143,559, 3131,524,
      1483,524, 1450,609, 1413,692, 1370,773, 1323,851, 1272,926,
      1216,998, 1156,1066, 1093,1131, 1026,1193, 955,1250, 881,1303,
      804,1352, 725,1396, 643,1436, 559,1470
      }),
    MakePath({ -47877,-47877, 84788,-47877, 84788,81432, -47877,81432 })
  };
  PathsI solution = InflatePaths(subject, -10000, JoinType::Round, EndType::Polygon);
  EXPECT_EQ(solution.size(), 2);
}

TEST(Clipper2Tests, TestOffsets6) // also modified from #593 (tests rounded ends)
{
  PathsI subjects = {
    {{620,620}, {-620,620}, {-620,-620}, {620,-620}},
    {{20,-277}, {42,-275}, {59,-272}, {80,-266}, {97,-261}, {114,-254},
    {135,-243}, {149,-235}, {167,-222}, {182,-211}, {197,-197},
    {212,-181}, {223,-167}, {234,-150}, {244,-133}, {253,-116},
    {260,-99}, {267,-78}, {272,-61}, {275,-40}, {278,-18}, {276,-39},
    {272,-61}, {267,-79}, {260,-99}, {253,-116}, {245,-133}, {235,-150},
    {223,-167}, {212,-181}, {197,-197}, {182,-211}, {168,-222}, {152,-233},
    {135,-243}, {114,-254}, {97,-261}, {80,-267}, {59,-272}, {42,-275}, {20,-278}}
  };
  const Scalar offset = -50;
  Clipper2Lib::ClipperOffset offseter;
  Clipper2Lib::PathsI tmpSubject;
  offseter.AddPaths(subjects, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Polygon);
  Clipper2Lib::PathsI solution;
  offseter.Execute(offset, solution);
  EXPECT_EQ(solution.size(), 2);
  Scalar area = Area<Integer>(solution[1]);
  EXPECT_LT(area, -47500);
}

TEST(Clipper2Tests, TestOffsets7) // (#593 & #715)
{
  PathsI solution;
  PathsI subject = { MakePath({0,0, 100,0, 100,100, 0,100}) };
  solution = InflatePaths(subject, -50, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 0);
  subject.push_back(MakePath({ 40,60, 60,60, 60,40, 40,40 }));
  solution = InflatePaths(subject, 10, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 1);
  reverse(subject[0].begin(), subject[0].end());
  reverse(subject[1].begin(), subject[1].end());
  solution = InflatePaths(subject, 10, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 1);
  subject.resize(1);
  solution = InflatePaths(subject, -50, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 0);
}

struct OffsetQual
{
  PointS smallestInSub;   // smallestInSub & smallestInSol are the points in subject and solution
  PointS smallestInSol;   // that define the place that most falls short of the expected offset
  PointS largestInSub;    // largestInSub & largestInSol are the points in subject and solution
  PointS largestInSol;    // that define the place that most exceeds the expected offset
};

template<typename T>
inline PointS GetClosestPointOnSegment(const PointS& offPt,
  const Point<T>& seg1, const Point<T>& seg2)
{
  if (seg1.x == seg2.x && seg1.y == seg2.y) return PointS(seg1);
  Scalar dx = static_cast<Scalar>(seg2.x - seg1.x);
  Scalar dy = static_cast<Scalar>(seg2.y - seg1.y);
  Scalar q = (
    (offPt.x - static_cast<Scalar>(seg1.x)) * dx +
    (offPt.y - static_cast<Scalar>(seg1.y)) * dy) /
    (Sqr(dx) + Sqr(dy));
  q = (q < 0) ? 0 : (q > 1) ? 1 : q;
  return PointS(
    static_cast<Scalar>(seg1.x) + (q * dx),
    static_cast<Scalar>(seg1.y) + (q * dy));
}

template<typename T>
static OffsetQual GetOffsetQuality(const Path<T>& subject, const Path<T>& solution, const Scalar delta)
{
  if (!subject.size() || !solution.size()) return OffsetQual();
  Scalar desiredDistSqr = delta * delta;
  Scalar smallestSqr = desiredDistSqr, largestSqr = desiredDistSqr;
  Scalar deviationsSqr = 0;
  OffsetQual oq;
  const size_t subVertexCount = 4; // 1 .. 100 :)
  const Scalar subVertexFrac = 1.0 / subVertexCount;
  Point<T> solPrev = solution[solution.size() - 1];
  for (const Point<T>& solPt0 : solution)
  {
    for (size_t i = 0; i < subVertexCount; ++i)
    {
      // divide each edge in solution into series of sub-vertices (solPt),
      PointS solPt = PointS(
        static_cast<Scalar>(solPrev.x) + static_cast<Scalar>(solPt0.x - solPrev.x) * subVertexFrac * i,
        static_cast<Scalar>(solPrev.y) + static_cast<Scalar>(solPt0.y - solPrev.y) * subVertexFrac * i);
      // now find the closest point in subject to each of these solPt.
      PointS closestToSolPt;
      Scalar closestDistSqr = std::numeric_limits<Scalar>::infinity();
      Point<T> subPrev = subject[subject.size() - 1];
      for (size_t i = 0; i < subject.size(); ++i)
      {
        PointS closestPt = ::GetClosestPointOnSegment(solPt, subject[i], subPrev);
        subPrev = subject[i];
        const Scalar sqrDist = DistanceSqr(closestPt, solPt);
        if (sqrDist < closestDistSqr) {
          closestDistSqr = sqrDist;
          closestToSolPt = closestPt;
        };
      }
      // we've now found solPt's closest pt in subject (closestToSolPt).
      // but how does the distance between these 2 points compare with delta
      // ideally - Distance(closestToSolPt, solPt) == delta;
      // see how this distance compares with every other solPt
      if (closestDistSqr < smallestSqr) {
        smallestSqr = closestDistSqr;
        oq.smallestInSub = closestToSolPt;
        oq.smallestInSol = solPt;
      }
      if (closestDistSqr > largestSqr) {
        largestSqr = closestDistSqr;
        oq.largestInSub = closestToSolPt;
        oq.largestInSol = solPt;
      }
    }
    solPrev = solPt0;
  }
  return oq;
}

TEST(Clipper2Tests, TestOffsets8) // (#724)
{
  PathsI subject = { MakePath({
       91759700, -49711991,    83886095, -50331657,
       -872415388, -50331657,  -880288993, -49711991,  -887968725, -47868251,
       -895265482, -44845834,  -901999593, -40719165,  -908005244, -35589856,
       -913134553, -29584205,  -917261224, -22850094,  -920283639, -15553337,
       -922127379, -7873605,   -922747045, 0,          -922747045, 1434498600,
       -922160557, 1442159790, -920414763, 1449642437, -917550346, 1456772156,
       -913634061, 1463382794, -908757180, 1469320287, -903033355, 1474446264,
       -896595982, 1478641262, -889595081, 1481807519, -882193810, 1483871245,
       -876133965, 1484596521, -876145751, 1484713389, -875781839, 1485061090,
       -874690056, 1485191762, -874447580, 1485237014, -874341490, 1485264094,
       -874171960, 1485309394, -873612294, 1485570372, -873201878, 1485980788,
       -872941042, 1486540152, -872893274, 1486720070, -872835064, 1487162210,
       -872834788, 1487185500, -872769052, 1487406000, -872297948, 1487583168,
       -871995958, 1487180514, -871995958, 1486914040, -871908872, 1486364208,
       -871671308, 1485897962, -871301302, 1485527956, -870835066, 1485290396,
       -870285226, 1485203310, -868659019, 1485203310, -868548443, 1485188472,
       -868239649, 1484791011, -868239527, 1484783879, -838860950, 1484783879,
       -830987345, 1484164215, -823307613, 1482320475, -816010856, 1479298059,
       -809276745, 1475171390, -803271094, 1470042081, -752939437, 1419710424,
       -747810128, 1413704773, -743683459, 1406970662, -740661042, 1399673904,
       -738817302, 1391994173, -738197636, 1384120567, -738197636, 1244148246,
       -738622462, 1237622613, -739889768, 1231207140, -802710260, 995094494,
       -802599822, 995052810,  -802411513, 994586048,  -802820028, 993050638,
       -802879992, 992592029,  -802827240, 992175479,  -802662144, 991759637,
       -802578556, 991608039,  -802511951, 991496499,  -801973473, 990661435,
       -801899365, 990554757,  -801842657, 990478841,  -801770997, 990326371,
       -801946911, 989917545,  -801636397, 989501855,  -801546099, 989389271,
       -800888669, 988625013,  -800790843, 988518907,  -800082405, 987801675,
       -799977513, 987702547,  -799221423, 987035738,  -799109961, 986944060,
       -798309801, 986330832,  -798192297, 986247036,  -797351857, 985690294,
       -797228867, 985614778,  -796352124, 985117160,  -796224232, 985050280,
       -795315342, 984614140,  -795183152, 984556216,  -794246418, 984183618,
       -794110558, 984134924,  -793150414, 983827634,  -793011528, 983788398,
       -792032522, 983547874,  -791891266, 983518284,  -790898035, 983345662,
       -790755079, 983325856,  -789752329, 983221956,  -789608349, 983212030,
       -787698545, 983146276,  -787626385, 983145034,  -536871008, 983145034,
       -528997403, 982525368,  -521317671, 980681627,  -514020914, 977659211,
       -507286803, 973532542,  -501281152, 968403233,  -496151843, 962397582,
       -492025174, 955663471,  -489002757, 948366714,  -487159017, 940686982,
       -486539351, 932813377,  -486539351, 667455555,  -486537885, 667377141,
       -486460249, 665302309,  -486448529, 665145917,  -486325921, 664057737,
       -486302547, 663902657,  -486098961, 662826683,  -486064063, 662673784,
       -485780639, 661616030,  -485734413, 661466168,  -485372735, 660432552,
       -485315439, 660286564,  -484877531, 659282866,  -484809485, 659141568,
       -484297795, 658173402,  -484219379, 658037584,  -483636768, 657110363,
       -483548422, 656980785,  -482898150, 656099697,  -482800368, 655977081,
       -482086070, 655147053,  -481979398, 655032087,  -481205068, 654257759,
       -481090104, 654151087,  -480260074, 653436789,  -480137460, 653339007,
       -479256372, 652688735,  -479126794, 652600389,  -478199574, 652017779,
       -478063753, 651939363,  -477095589, 651427672,  -476954289, 651359626,
       -475950593, 650921718,  -475804605, 650864422,  -474770989, 650502744,
       -474621127, 650456518,  -473563373, 650173094,  -473410475, 650138196,
       -472334498, 649934610,  -472179420, 649911236,  -471091240, 649788626,
       -470934848, 649776906,  -468860016, 649699272,  -468781602, 649697806,
       -385876037, 649697806,  -378002432, 649078140,  -370322700, 647234400,
       -363025943, 644211983,  -356291832, 640085314,  -350286181, 634956006,
       -345156872, 628950354,  -341030203, 622216243,  -338007786, 614919486,
       -336164046, 607239755,  -335544380, 599366149,  -335544380, 571247184,
       -335426942, 571236100,  -335124952, 570833446,  -335124952, 569200164,
       -335037864, 568650330,  -334800300, 568184084,  -334430294, 567814078,
       -333964058, 567576517,  -333414218, 567489431,  -331787995, 567489431,
       -331677419, 567474593,  -331368625, 567077133,  -331368503, 567070001,
       -142068459, 567070001,  -136247086, 566711605,  -136220070, 566848475,
       -135783414, 567098791,  -135024220, 567004957,  -134451560, 566929159,
       -134217752, 566913755,  -133983942, 566929159,  -133411282, 567004957,
       -132665482, 567097135,  -132530294, 567091859,  -132196038, 566715561,
       -132195672, 566711157,  -126367045, 567070001,  -33554438, 567070001,
       -27048611, 566647761,   -20651940, 565388127,   -14471751, 563312231,
       -8611738, 560454902,    36793963, 534548454,    43059832, 530319881,
       48621743, 525200596,    53354240, 519306071,    57150572, 512769270,
       59925109, 505737634,    61615265, 498369779,    62182919, 490831896,
       62182919, 474237629,    62300359, 474226543,    62602349, 473823889,
       62602349, 472190590,    62689435, 471640752,    62926995, 471174516,
       63297005, 470804506,    63763241, 470566946,    64313081, 470479860,
       65939308, 470479860,    66049884, 470465022,    66358678, 470067562,
       66358800, 470060430,    134217752, 470060430,   134217752, 0,
       133598086, -7873605,    131754346, -15553337,   128731929, -22850094,
       124605260, -29584205,   119475951, -35589856,   113470300, -40719165,
       106736189, -44845834,   99439432, -47868251,    91759700, -49711991
    }) };
  Scalar offset = -50329979.277800001, arc_tol = 5000;
  PathsI solution = InflatePaths(subject, offset, JoinType::Round, EndType::Polygon, 2, arc_tol);
  OffsetQual oq = GetOffsetQuality(subject[0], solution[0], offset);
  Scalar smallestDist = Distance(oq.smallestInSub, oq.smallestInSol);
  Scalar largestDist = Distance(oq.largestInSub, oq.largestInSol);
  const Scalar rounding_tolerance = 1.0;
  offset = std::abs(offset);
  //std::cout << std::setprecision(0) << std::fixed;
  //std::cout << "Expected delta           : " << offset << std::endl;
  //std::cout << "Smallest delta           : " << smallestDist << " (" << smallestDist - offset << ")" << std::endl;
  //std::cout << "Largest delta            : " << largestDist << " (" << largestDist - offset << ")" << std::endl;
  //std::cout << "Coords of smallest delta : " << oq.smallestInSub << " and " << oq.smallestInSol << std::endl;
  //std::cout << "Coords of largest delta  : " << oq.largestInSub << " and " << oq.largestInSol << std::endl;
  //std::cout << std::endl;
  //SvgWriter svg;
  //SvgAddSubject(svg, subject, FillRule::NonZero);
  //SvgAddSolution(svg, solution, FillRule::NonZero, false);
  //std::string filename = "offset_test.svg";
  //SvgSaveToFile(svg, filename, 800, 600, 10);
  EXPECT_LE(offset - smallestDist - rounding_tolerance, arc_tol);
  EXPECT_LE(largestDist - offset  - rounding_tolerance, arc_tol);
}

TEST(Clipper2Tests, TestOffsets9) // (#733)
{
  // solution orientations should match subject orientations UNLESS
  // reverse_solution is set true in ClipperOffset's constructor
  // start subject's orientation positive ...
  PathsI subject{ MakePath({100,100, 200,100, 200, 400, 100, 400}) };
  PathsI solution = InflatePaths(subject, 50, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 1);
  EXPECT_TRUE(IsPositive(solution[0]));
  // reversing subject's orientation should not affect delta direction
  // (ie where positive deltas inflate).
  std::reverse(subject[0].begin(), subject[0].end());
  solution = InflatePaths(subject, 50, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 1);
  EXPECT_TRUE(std::fabs(Area(solution[0])) > std::fabs(Area(subject[0])));
  EXPECT_FALSE(IsPositive(solution[0]));
  ClipperOffset co(2, 0, false, true); // last param. reverses solution
  co.AddPaths(subject, JoinType::Miter, EndType::Polygon);
  co.Execute(50, solution);
  EXPECT_EQ(solution.size(), 1);
  EXPECT_TRUE(std::fabs(Area(solution[0])) > std::fabs(Area(subject[0])));
  EXPECT_TRUE(IsPositive(solution[0]));
  // add a hole (ie has reverse orientation to outer path)
  subject.push_back( MakePath({130,130, 170,130, 170,370, 130,370}) );
  solution = InflatePaths(subject, 30, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 1);
  EXPECT_FALSE(IsPositive(solution[0]));
  co.Clear(); // should still reverse solution orientation
  co.AddPaths(subject, JoinType::Miter, EndType::Polygon);
  co.Execute(30, solution);
  EXPECT_EQ(solution.size(), 1);
  EXPECT_TRUE(std::fabs(Area(solution[0])) > std::fabs(Area(subject[0])));
  EXPECT_TRUE(IsPositive(solution[0]));
  solution = InflatePaths(subject, -15, JoinType::Miter, EndType::Polygon);
  EXPECT_EQ(solution.size(), 0);
}

TEST(Clipper2Tests, TestOffsets10) // see #715
{
  PathsI subjects = {
         {{508685336, -435806096},
          {509492982, -434729201},
          {509615525, -434003092},
          {509615525, 493372891},
          {509206033, 494655198},
          {508129138, 495462844},
          {507403029, 495585387},
          {-545800889, 495585387},
          {-547083196, 495175895},
          {-547890842, 494099000},
          {-548013385, 493372891},
          {-548013385, -434003092},
          {-547603893, -435285399},
          {-546526998, -436093045},
          {-545800889, -436215588},
          {507403029, -436215588}},
         {{106954765, -62914568},
          {106795129, -63717113},
          {106340524, -64397478},
          {105660159, -64852084},
          {104857613, -65011720},
          {104055068, -64852084},
          {103374703, -64397478},
          {102920097, -63717113},
          {102760461, -62914568},
          {102920097, -62112022},
          {103374703, -61431657},
          {104055068, -60977052},
          {104857613, -60817416},
          {105660159, -60977052},
          {106340524, -61431657},
          {106795129, -62112022}} };

  Clipper2Lib::ClipperOffset offseter(2, 104857.61318750000);
  PathsI solution;
  offseter.AddPaths(subjects, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Polygon);
  offseter.Execute(-2212495.6382562499, solution);
  EXPECT_EQ(solution.size(), 2);
}

