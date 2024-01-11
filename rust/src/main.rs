
use trc::run;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    gettextrs::setlocale(gettextrs::LocaleCategory::LcAll, "");
    gettextrs::bindtextdomain("trans", "locales")?;
    gettextrs::textdomain("trans")?;
    run();
    Ok(())
}
