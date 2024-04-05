pub mod ac;
pub mod deque;
pub mod fenwick;
pub mod forward_list;
pub mod hash_map;
pub mod heap;
pub mod list;
pub mod map;
pub mod priority_queue;
pub mod queue;
pub mod sam;
pub mod set;
pub mod splay;
pub mod st;
pub mod stack;
pub mod trie;
pub mod union;
pub mod vec;

use derive::def_module;
use libcore::*;
use sam::Sam;

def_module!(module_name=ds, classes=[Sam => sam]);
