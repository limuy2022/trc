#[test]
fn test_load_library() {
    let lib = unsafe { libloading::Library::new(libloading::library_filename("stdlib")).unwrap() };
    let (_module, _storage) = trc::base::dll::load_module_storage(&lib);
}
