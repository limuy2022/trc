use rust_i18n::t;
use trc::run;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    rust_i18n::set_locale(&sys_locale::get_locale().unwrap());
    run().unwrap();
    Ok(())
}
