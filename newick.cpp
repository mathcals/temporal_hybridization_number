//
// Created by sander on 23-6-19.
//

#include "tree.h"
#include "newick.h"
#include <string>
#include <stdexcept>
#include <boost/algorithm/string/replace.hpp>



std::vector<Tree<std::string>*> parseFile(std::string text) {
    boost::replace_all(text, "\n", "");
    boost::replace_all(text, "\r", "");
    boost::replace_all(text, " ", "");
    boost::replace_all(text, "\t", "");
    const char* cstr = text.c_str();
    const Reader reader = &cstr;
    return _parseFile(reader);
}

std::vector<Tree<std::string>*> _parseFile(Reader reader) {
    std::vector<Tree<std::string>*> ret;
    while (true) {
        if (**reader == '\0') {
            break;
        }
        ret.push_back(parseTree(reader));
        expectValue(reader, ';');
    }
    return ret;
}

Tree<std::string>* parseTree(Reader reader) {
    std::vector<Tree<std::string>*> children;
    std::string name;
    if (**reader == '(') {
        expectValue(reader, '(');
        children = parseChildren(reader);
        expectValue(reader, ')');
    }
    name = readValue(reader);
    return new Tree<std::string>(children, name);
}

std::string readValue(Reader reader) {
    std::string value;
    while (**reader != '(' &&  **reader != ')' && **reader != ',' && **reader != ';' && **reader != '\0') {
        value += **reader;
        (*reader)++;
    }
    return value;
}

std::vector<Tree<std::string>*> parseChildren(Reader reader) {
    std::vector<Tree<std::string>*> ret;
    bool first = true;
    while (**reader != '\0' && **reader != ')') {
        if (!first) {
            expectValue(reader, ',');
        }
        else {
            first = false;
        }
        ret.push_back(parseTree(reader));


    }
    return ret;
}

void expectValue(Reader reader, char expected) {
    if (**reader != expected) {
        throw std::invalid_argument("Unexpected char");
    }
    (*reader)++;
}