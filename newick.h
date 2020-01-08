#pragma once

#include "tree.h"
#include <string>
#include <stdexcept>
#include <boost/algorithm/string/replace.hpp>

typedef const char** Reader;
std::vector<Tree<std::string>*> parseFile(std::string text);
Tree<std::string>* parseTree(Reader reader);

void expectValue(Reader reader, char expected);

std::vector<Tree<std::string>*> parseChildren(Reader reader);

std::vector<Tree<std::string>*> _parseFile(Reader reader);

std::string readValue(Reader reader);

