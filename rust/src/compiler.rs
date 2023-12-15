mod token;
use std::fs;

pub struct compiler {
    input:fs::File
}

impl compiler {
    fn new(f:fs::File) -> compiler {
        compiler {
            input:f
        }
    }
}
