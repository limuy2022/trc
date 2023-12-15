use clap::Parser;

#[derive(Parser)]
pub struct ParamsCil {
    pattern: String,

}

pub fn compile() {
    let args = ParamsCil::parse();
}
