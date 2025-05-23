//! link different modules
use crate::compiler::ValuePool;
use libcore::*;
use std::collections::HashSet;

pub fn link<'a>(data_iter: impl Iterator<Item = &'a mut StaticData>) -> StaticData {
    let mut data = StaticData::new();
    let mut value_pool = ValuePool::new();
    // 去除重复的依赖dll模块
    let mut dup_dll: HashSet<String> = HashSet::new();
    for i in data_iter {
        // 插入dll
        i.dll_module_should_loaded.iter().for_each(|element| {
            dup_dll.insert(element.clone());
        });
        // 更新符号表
        data.global_sym_table_sz += i.global_sym_table_sz;
        // 依次链接每一个模块
        // 先处理常量池
        // 将常量加入常量池并建立映射关系
        value_pool.extend_pool(i);
        // copy inst
        // 改变函数的映射
        let mut iter_of_function = i.funcs_pos.iter();
        let mut function_expected_pos = iter_of_function.next();
        let mut func_record_tmp = vec![];
        for (inst_ori_id, j) in i.inst.iter().enumerate() {
            // 改变指令对常量池的引用
            let mut added = j.clone();
            match j.opcode {
                Opcode::LoadString => {
                    let idx: usize = added.operand.0.into();
                    added.operand.0 = Opidx(
                        *value_pool.get_string(&i.constpool.stringpool[idx]).unwrap()
                            as OpidxInternal,
                    )
                }
                Opcode::LoadFloat => {
                    // TODO:improve the performance
                    let idx: usize = added.operand.0.into();
                    added.operand.0 = Opidx(
                        *value_pool
                            .get_float(&i.constpool.floatpool[idx].to_string())
                            .unwrap() as OpidxInternal,
                    )
                }
                Opcode::LoadBigInt => { /* TODO:bigint */ }
                Opcode::CallCustom => {
                    *added.operand.0 += data.funcs_pos.len() as OpidxInternal;
                }
                _ => {}
            }
            // 如果位于函数定义点，修正对应的函数定义点
            if let Some(pos) = function_expected_pos
                && inst_ori_id == pos.func_addr
            {
                function_expected_pos = iter_of_function.next();
                let mut added = pos.clone();
                // 该项指令还没有加入，所以是next
                added.func_addr = data.get_next_opcode_id().into();
                func_record_tmp.push(added);
            }
            data.inst.push(added);
        }
        // 合并函数定义点
        data.funcs_pos.extend(func_record_tmp);
        // 行号表直接合并
        data.line_table.extend(i.line_table.iter());
    }
    data.constpool = value_pool.store_val_to_vm();
    data.dll_module_should_loaded = dup_dll.into_iter().collect();
    data
}

#[cfg(test)]
mod test {
    use super::*;
    use crate::compiler::StaticData;

    #[test]
    fn test_link() {
        // TODO:add pool test
        let mut data1 = StaticData::new();
        data1.dll_module_should_loaded = vec!["test.dll".to_string()];
        data1.inst = vec![Inst::new_single(Opcode::LoadInt, Opidx(1))];
        let mut data2 = StaticData::new();
        data2.dll_module_should_loaded = vec!["test.dll2".to_string(), "test.dll".to_string()];
        data2.inst = vec![Inst::new_single(Opcode::LoadInt, Opidx(3))];
        let mut data = vec![data1, data2];
        let data = link(data.iter_mut());
        assert_eq!(
            HashSet::<String>::from_iter(data.dll_module_should_loaded.into_iter()),
            HashSet::from_iter(vec!["test.dll2".to_string(), "test.dll".to_string()].into_iter())
        );
        assert_eq!(
            data.inst,
            vec![
                Inst::new_single(Opcode::LoadInt, Opidx(1)),
                Inst::new_single(Opcode::LoadInt, Opidx(3)),
            ]
        );
    }
}
