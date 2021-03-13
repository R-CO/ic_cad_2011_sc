#include "../src/verilog_parser.hpp"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

#include "../src/little_tools.hpp"

const char *const kTestCase1FilePath =
    "../test_cases/A1_testcase2/case1_ISO4/top.v";

TEST(VerilogParser, parseFile) {
  // get parse result via original funcitons
  std::vector<std::string> files_content(1);
  int file_index = 0;
  EXPECT_TRUE(ReadVerilogFile(kTestCase1FilePath, files_content, file_index));
  std::vector<std::string> verilog_statements;
  StoreVerilogStatement(files_content, verilog_statements, file_index);
  std::map<std::string, Module> module_map_original;
  ParseVerilogFile(verilog_statements, module_map_original);
  EXPECT_EQ(module_map_original.size(), 4);

  // get parse result via class rco::VerilogParser
  rco::ModuleMap module_map_newer;
  rco::VerilogParser verilog_parser;
  EXPECT_TRUE(verilog_parser.parseFile(std::string(kTestCase1FilePath),
                                       module_map_newer));
  EXPECT_EQ(module_map_newer.size(), 4);

  {
    const std::string target_module_name("Top");
    EXPECT_NE(module_map_original.count(target_module_name), 0);
    EXPECT_NE(module_map_newer.count(target_module_name), 0);
    auto itor_original = module_map_original.find(target_module_name);
    auto itor_newer = module_map_newer.find(target_module_name);
    if (itor_original == module_map_original.end() ||
        itor_newer == module_map_newer.end()) {
      return;
    }
    EXPECT_EQ(itor_newer->second, itor_original->second);
  }

  {
    const std::string target_module_name("Block1");
    EXPECT_NE(module_map_original.count(target_module_name), 0);
    EXPECT_NE(module_map_newer.count(target_module_name), 0);
    auto itor_original = module_map_original.find(target_module_name);
    auto itor_newer = module_map_newer.find(target_module_name);
    if (itor_original == module_map_original.end() ||
        itor_newer == module_map_newer.end()) {
      return;
    }
    EXPECT_EQ(itor_newer->second, itor_original->second);
  }

  {
    const std::string target_module_name("Block2");
    EXPECT_NE(module_map_original.count(target_module_name), 0);
    EXPECT_NE(module_map_newer.count(target_module_name), 0);
    auto itor_original = module_map_original.find(target_module_name);
    auto itor_newer = module_map_newer.find(target_module_name);
    if (itor_original == module_map_original.end() ||
        itor_newer == module_map_newer.end()) {
      return;
    }
    EXPECT_EQ(itor_newer->second, itor_original->second);
  }

  {
    const std::string target_module_name("Block3");
    EXPECT_NE(module_map_original.count(target_module_name), 0);
    EXPECT_NE(module_map_newer.count(target_module_name), 0);
    auto itor_original = module_map_original.find(target_module_name);
    auto itor_newer = module_map_newer.find(target_module_name);
    if (itor_original == module_map_original.end() ||
        itor_newer == module_map_newer.end()) {
      return;
    }
    EXPECT_EQ(itor_newer->second, itor_original->second);
  }
}

TEST(LittleTools, TrimLeft_Empty) {
  const std::string result_str("");
  std::string test_str = "   " + result_str;
  TrimLeft(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
  test_str = "\t   \t\t" + result_str;
  TrimLeft(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
}

TEST(LittleTools, TrimLeft) {
  const std::string result_str("This is a test.    \t\t   ");
  std::string test_str = "   " + result_str;
  TrimLeft(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
  test_str = "\t   \t\t" + result_str;
  TrimLeft(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
}

TEST(LittleTools, TrimRight) {
  const std::string result_str("    \t\t\tThis is a test.");
  std::string test_str = result_str + "    ";
  TrimRight(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
  test_str = result_str + "\t    \t   \t\t\t";
  TrimRight(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
}

TEST(LittleTools, Trim) {
  const std::string result_str("This is a test.");
  std::string test_str = "   " + result_str + "    ";
  Trim(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
  test_str = "\t   \t\t" + result_str + "\t    \t   \t\t\t";
  Trim(test_str, " \t");
  EXPECT_EQ(test_str, result_str);
}
