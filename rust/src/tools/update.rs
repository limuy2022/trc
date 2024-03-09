use std::{error::Error, fmt::Display};

static GITHUB_API_URL: &str = "http://185.106.176.199/trc_version";

pub fn check_version() -> Result<(), Box<dyn Error>> {
    Ok(())
}

#[derive(Debug)]
pub struct UpdateError {}

impl Error for UpdateError {}

impl Display for UpdateError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            "Only support Linux.Users on other platforms should update program by hand"
        )
    }
}

impl UpdateError {
    pub fn new() -> Self {
        Self {}
    }
}

pub fn update() -> Result<(), UpdateError> {
    if !cfg!(target_os = "linux") {
        return Err(UpdateError::new());
    }
    Ok(())
}
