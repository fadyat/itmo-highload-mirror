drop table if exists scores;

create table scores (
    student_id int primary key,
    hw1 int,
    hw2 int,
    hw3 int
);

insert into
    scores (student_id, hw1, hw2, hw3)
values
    (1, 15, 10, null),
    (2, 14, 11, 14),
    (3, 13, null, 13),
    (4, 12, 12, 12),
    (5, 11, 13, 11),
    (6, 10, 14, 10);
