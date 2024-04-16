//! generate ctrc file
//! ctrc file is trc's compiled object
//! can be loaded and runned by vm without compiling

use libcore::{FuncStorage, Inst, Opcode, OpcodeTy, StaticData};
use std::{
    io::{BufReader, BufWriter, Read, Write},
    mem::size_of,
};

type LenSize = u64;
type FloatTy = f64;
const MAGIC_NUMBER: u64 = 0xA1B5C6D7u64;

macro_rules! read_integer {
    ($f:expr, $U:ty) => {{
        let mut len = [0u8; size_of::<$U>()];
        $f.read_exact(&mut len)?;
        <$U>::from_le_bytes(len)
    }};
}

fn read_lensize<T: Read>(f: &mut BufReader<T>) -> anyhow::Result<usize> {
    Ok(read_integer!(f, LenSize) as usize)
}

pub fn convert_to_lensize<T: TryInto<LenSize>>(value: T) -> LenSize
where
    <T as TryInto<u64>>::Error: std::fmt::Debug,
{
    value.try_into().unwrap()
}

fn write_integer<T: Write, U: TryInto<LenSize>>(f: &mut BufWriter<T>, len: U) -> anyhow::Result<()>
where
    <U as TryInto<u64>>::Error: Send + Sync + std::fmt::Debug,
{
    let tmp: LenSize = convert_to_lensize(len);
    f.write_all(&(tmp).to_le_bytes())?;
    Ok(())
}

fn write_float<T: Write>(f: &mut BufWriter<T>, val: FloatTy) -> anyhow::Result<()> {
    write_integer(f, val.to_bits())
}

fn read_float<T: Read>(f: &mut BufReader<T>) -> anyhow::Result<f64> {
    let val = read_integer!(f, u64);
    Ok(FloatTy::from_bits(val))
}

fn write_string<T: Write>(f: &mut BufWriter<T>, s: &str) -> anyhow::Result<()> {
    write_integer(f, s.len())?;
    f.write_all(s.as_bytes()).unwrap();
    Ok(())
}

fn read_string<T: Read>(f: &mut BufReader<T>) -> anyhow::Result<String> {
    let len = read_lensize(f)?;
    let mut buf = vec![0u8; len];
    f.read_exact(&mut buf)?;
    let buf = String::from_utf8(buf)?;
    Ok(buf)
}

fn write_bytecodes<T: Write>(f: &mut BufWriter<T>, data: &StaticData) -> anyhow::Result<()> {
    // 先写入个数
    write_integer(f, data.inst.len())?;
    for i in &data.inst {
        write_integer(f, i.opcode as libcore::OpcodeTy)?;
        let opnums = i.opcode.get_opcode_arg_nums();
        if opnums > 0 {
            write_integer(f, i.operand.0)?;
            if opnums > 1 {
                write_integer(f, i.operand.1)?;
            }
        }
    }
    Ok(())
}

fn load_bytecodes<T: Read>(f: &mut BufReader<T>, data: &mut StaticData) -> anyhow::Result<()> {
    let len = read_lensize(f)?;
    data.inst.reserve(len);
    for _ in 0..len {
        let opcode = Opcode::try_from(read_integer!(f, OpcodeTy))?;
        let opnums = opcode.get_opcode_arg_nums();
        let inst = if opnums > 0 {
            (
                read_lensize(f)?,
                if opnums > 1 { read_lensize(f)? } else { 0 },
            )
        } else {
            (0, 0)
        };
        let inst = Inst::new_double(opcode, inst.0, inst.1);
        data.inst.push(inst);
    }
    Ok(())
}

fn write_deps<T: Write>(f: &mut BufWriter<T>, data: &StaticData) -> anyhow::Result<()> {
    // 写入依赖模块的个数
    write_integer(f, data.dll_module_should_loaded.len())?;
    for i in &data.dll_module_should_loaded {
        write_string(f, i)?;
    }
    Ok(())
}

fn load_deps<T: Read>(f: &mut BufReader<T>, data: &mut StaticData) -> anyhow::Result<()> {
    // 读取依赖模块的个数
    let len = read_lensize(f)?;
    data.dll_module_should_loaded.reserve(len);
    for _ in 0..len {
        let buf = read_string(f)?;
        data.dll_module_should_loaded.push(buf);
    }
    Ok(())
}

fn write_const_pool<T: Write>(f: &mut BufWriter<T>, data: &StaticData) -> anyhow::Result<()> {
    // int pool
    write_integer(f, data.constpool.intpool.len())?;
    for i in &data.constpool.intpool {
        write_integer(f, *i)?;
    }
    // float pool
    write_integer(f, data.constpool.floatpool.len())?;
    for i in &data.constpool.floatpool {
        write_float(f, *i)?;
    }
    // string pool
    write_integer(f, data.constpool.stringpool.len())?;
    for i in &data.constpool.stringpool {
        write_string(f, i)?;
    }
    Ok(())
}

fn load_const_pool<T: Read>(f: &mut BufReader<T>, data: &mut StaticData) -> anyhow::Result<()> {
    // int pool
    let len = read_lensize(f)?;
    data.constpool.intpool.reserve(len);
    for _ in 0..len {
        let val = read_integer!(f, i64);
        data.constpool.intpool.push(val);
    }
    // float pool
    let len = read_lensize(f)?;
    data.constpool.floatpool.reserve(len);
    for _ in 0..len {
        let val = read_float(f)?;
        data.constpool.floatpool.push(val);
    }
    // string pool
    let len = read_lensize(f)?;
    data.constpool.stringpool.reserve(len);
    for _ in 0..len {
        let val = read_string(f)?;
        data.constpool.stringpool.push(val);
    }
    Ok(())
}

fn write_symbol_table<T: Write>(f: &mut BufWriter<T>, data: &StaticData) -> anyhow::Result<()> {
    write_integer(f, data.sym_table_sz)?;
    Ok(())
}

fn load_symbol_table<T: Read>(f: &mut BufReader<T>, data: &mut StaticData) -> anyhow::Result<()> {
    data.sym_table_sz = read_integer!(f, usize);
    Ok(())
}

fn write_line_table(f: &mut BufWriter<impl Write>, data: &StaticData) -> anyhow::Result<()> {
    write_integer(f, data.line_table.len())?;
    for i in &data.line_table {
        write_integer(f, *i)?;
    }
    Ok(())
}

fn load_line_table(f: &mut BufReader<impl Read>, data: &mut StaticData) -> anyhow::Result<()> {
    let len = read_integer!(f, usize);
    data.line_table.reserve(len);
    for _ in 0..len {
        data.line_table.push(read_integer!(f, usize));
    }
    Ok(())
}

fn write_function_info(f: &mut BufWriter<impl Write>, data: &StaticData) -> anyhow::Result<()> {
    write_integer(f, data.funcs_pos.len())?;
    for i in &data.funcs_pos {
        write_integer(f, i.func_addr)?;
        write_integer(f, i.var_table_sz)?;
    }
    Ok(())
}

fn load_function_info(f: &mut BufReader<impl Read>, data: &mut StaticData) -> anyhow::Result<()> {
    let len = read_integer!(f, usize);
    data.funcs_pos.reserve(len);
    for _ in 0..len {
        let func_addr = read_integer!(f, usize);
        let var_table_sz = read_integer!(f, usize);
        data.funcs_pos
            .push(FuncStorage::new(func_addr, var_table_sz));
    }
    Ok(())
}

fn write_magic_number(f: &mut BufWriter<impl Write>) -> anyhow::Result<()> {
    write_integer(f, MAGIC_NUMBER)?;
    Ok(())
}

fn load_magic_number(f: &mut BufReader<impl Read>) -> anyhow::Result<()> {
    let _magic = read_integer!(f, u64);
    Ok(())
}

pub fn check_if_ctrc_file(f: &mut BufReader<impl Read>) -> anyhow::Result<bool> {
    let magic = read_integer!(f, u64);
    Ok(magic == MAGIC_NUMBER)
}

/// 生成ctrc
/// write_to_path: ctrc文件的路径
/// data: 静态数据
pub fn write_to_ctrc(data: &StaticData, write_to_path: &str) -> anyhow::Result<()> {
    let f = std::fs::File::create(write_to_path).unwrap();
    let mut writer = BufWriter::new(f);
    write_magic_number(&mut writer)?;
    write_deps(&mut writer, data)?;
    write_symbol_table(&mut writer, data)?;
    write_bytecodes(&mut writer, data)?;
    write_const_pool(&mut writer, data)?;
    write_function_info(&mut writer, data)?;
    write_line_table(&mut writer, data)?;
    writer.flush()?;
    Ok(())
}

pub fn load_from_reader_without_magic(f: &mut BufReader<impl Read>) -> anyhow::Result<StaticData> {
    let mut data = StaticData::default();
    load_deps(f, &mut data)?;
    load_symbol_table(f, &mut data)?;
    load_bytecodes(f, &mut data)?;
    load_const_pool(f, &mut data)?;
    load_function_info(f, &mut data)?;
    load_line_table(f, &mut data)?;
    Ok(data)
}

pub fn load_from_ctrc(load_path: &str) -> anyhow::Result<StaticData> {
    let f = std::fs::File::open(load_path)?;
    let mut reader = BufReader::new(f);
    load_magic_number(&mut reader)?;
    let data = load_from_reader_without_magic(&mut reader)?;
    Ok(data)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_magic_number() {
        let f = std::fs::File::create("tests/testdata/utils/ctrc/magic_number_with.ctrc").unwrap();
        let mut writer = BufWriter::new(f);
        write_magic_number(&mut writer).unwrap();
        writer.flush().unwrap();
        drop(writer);
        let f = std::fs::File::open("tests/testdata/utils/ctrc/magic_number_with.ctrc").unwrap();
        let mut reader = BufReader::new(f);
        assert!(check_if_ctrc_file(&mut reader).unwrap());
        let f = std::fs::File::open("tests/testdata/utils/ctrc/magic_number_without.ctrc").unwrap();
        let mut reader = BufReader::new(f);
        assert!(!check_if_ctrc_file(&mut reader).unwrap());
    }
}
