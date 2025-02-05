fn main() -> shadow_rs::SdResult<()> {
    println!("cargo:rerun-if-changed=locales");
    shadow_rs::ShadowBuilder::builder().build()?;
    Ok(())
}
