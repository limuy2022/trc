#include <Compiler/grammar_env.h>

namespace trc::compiler {
void grammar_data_control::array_get_value(treenode* head) {
}

void grammar_data_control::create_array(treenode* head, const vecs& code) {
}

void grammar_data_control::map_get_value(treenode* head) {
}

void grammar_data_control::create_map(treenode* head, const vecs& code) {
}

void grammar_data_control::compile_get_value(treenode* head, const vecs& code) {
}

void grammar_data_control::compile_create_obj(
    treenode* head, const vecs& code) {
}

grammar_data_control::~grammar_data_control() {
    array_list.clear();
    map_list.clear();
}
}