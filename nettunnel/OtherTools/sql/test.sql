-- sql语句文件

create table if not exists test_table (
    id integer primary key,
    name varchar(50),
    age integer
);

-- 创建name索引
create index if not exists idx_name on test_table(name);
-- dorp index idx_name on test_table;
-- alter table test_table dorp index idx_name;

-- 利用trigger自动删除，只保留大10个
create trigger if not exists trg_before_insert after insert on test_table
begin
    --select name from test_table where id = new.id;
    delete from test_table where id not in
        (select id from test_table order by id desc limit 10);
end;
