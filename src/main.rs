use trc::run;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    match sys_locale::get_locale() {
        None => {
            rust_i18n::set_locale("en");
        }
        Some(locale) => {
            rust_i18n::set_locale(&locale);
        }
    }
    run().unwrap();
    Ok(())
}
